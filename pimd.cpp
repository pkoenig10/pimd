#include <cstring>
#include <queue>
#include <vector>

#include "mailbox.h"
#include "pimd.h"
#include "pimd_op.h"


#define BUS_TO_PHYS(addr) ((addr) & ~0xC0000000)

#define GPU_MEM_FLG 0xC // cached=0xC; direct=0x4

#define GPU_QPUS 8
#define GPU_LEN 1024


struct PimdArgInfo {
    int index;
    PimdOpType type;
};


int pimd_open() {
    int mb = mbox_open();

    if (qpu_enable(mb, 1)) {
        mbox_close(mb);
        return 0;
    }

    return mb;
}

void pimd_close(int mb) {
    qpu_enable(mb, 0);

    mbox_close(mb);
}


class PimdFunction::Info {
public:
    int mb;
    std::vector<unsigned> code;
    std::vector<PimdArgInfo> args;
    int num_inputs = 0;
    int num_outputs = 0;

    unsigned handle = 0;
    int mem_len;
    unsigned *code_ptr;
    unsigned *inputs_ptr;
    unsigned *outputs_ptr;
    unsigned *uniforms_ptr;
    unsigned *messages_ptr;

    Info(int mb, PimdOp *ops, int num_ops) : mb(mb), ops(ops), num_ops(num_ops) {
        std::queue<PimdArgInfo> sargs;
        std::queue<PimdArgInfo> vargs;

        int next_arg = 0;
        for (int i=0; i<num_ops; i++) {
            switch(pimd_op_type(ops[i])) {
            case ADDR:
                num_outputs++;
            case SCALAR:
                sargs.push({next_arg++, pimd_op_type(ops[i])});
                break;

            case VECTOR:
                num_inputs++;
                vargs.push({next_arg++, pimd_op_type(ops[i])});
                break;

            default:
                break;
            }
        }

        code_append(INST_START);

        for (int i=0; i<4 && !vargs.empty(); i++) {
            code_append(INST_TMU);
            args.push_back(vargs.front());
            vargs.pop();
        }

        for (int i=0; i<num_ops; i++) {
            if (ops[i] >= INST_START) {
                continue;
            }

            switch(pimd_op_type(ops[i])) {
            case SCALAR:
                code_append(INST_UNIF);
            case ADDR:
                args.push_back(sargs.front());
                sargs.pop();
                break;

            case VECTOR:
                code_append(INST_LDTMU);
                if (!vargs.empty()) {
                    code_append(INST_TMU);
                    args.push_back(vargs.front());
                    vargs.pop();
                }
                break;

            default:
                break;
            }

            code_append(ops[i]);
        }

        code_append(INST_END);
    }

    int alloc(int len) {
        if (handle) {
            return 0;
        }

        mem_len = (len + GPU_LEN - 1) & ~(GPU_LEN - 1);

        unsigned code_size = code.size();
        unsigned inputs_size = num_inputs * mem_len;
        unsigned outputs_size = num_outputs * mem_len;
        unsigned uniforms_size = args.size() * mem_len / GPU_LEN + 1;
        unsigned messages_size = 2 * GPU_QPUS;

        mem_size = (code_size + inputs_size + outputs_size + uniforms_size + messages_size) * sizeof(unsigned);
        handle = mem_alloc(mb, mem_size, 4096, GPU_MEM_FLG);
        if (!handle) {
            return -1;
        }
        len = len;
        gpu_ptr = (unsigned*)mem_lock(mb, handle);
        arm_ptr = (unsigned*)mapmem(BUS_TO_PHYS((unsigned)gpu_ptr), mem_size);

        unsigned *p = arm_ptr;

        code_ptr = p;
        std::copy(code.begin(), code.end(), code_ptr);
        p += code_size;

        inputs_ptr = p;
        p += inputs_size;

        outputs_ptr = p;
        p += outputs_size;

        uniforms_ptr = p;
        p += uniforms_size;

        messages_ptr = p;
        for (int i=0; i<GPU_QPUS; ++i) {
            *p++ = gpu_addr(uniforms_ptr);
            *p++ = gpu_addr(code_ptr);
        }

        return 0;
    }

    void free() {
        if (!handle) {
            return;
        }

        unmapmem(arm_ptr, mem_size);
        mem_unlock(mb, handle);
        mem_free(mb, handle);

        handle = 0;
    }

    unsigned gpu_addr(unsigned *p) {
        return (unsigned)(gpu_ptr + (p - arm_ptr));
    }

private:
    PimdOp *ops;
    int num_ops;

    unsigned mem_size;
    unsigned *gpu_ptr;
    unsigned *arm_ptr;

    void code_append(PimdOp op) {
        code.insert(code.end(), pimd_op_code(op), pimd_op_code(op) + pimd_op_size(op));
    }
};


PimdFunction::PimdFunction(int mb, PimdOp *ops, int num_ops) {
    info = new Info(mb, ops, num_ops);
}

PimdFunction::~PimdFunction() {
    free();
    delete info;
}

int PimdFunction::call(PimdArg *args, int num_args, int len, int timeout) {
    if (num_args != info->args.size()) {
        return -1;
    }

    if (info->handle && len > info->mem_len) {
        free();
    }

    if (!info->handle && info->alloc(len)) {
        return -2;
    }

    int num_loop = (len + GPU_LEN - 1) / GPU_LEN;

    unsigned *p = info->uniforms_ptr;
    *p++ = num_loop;
    for (int i=0; i<num_loop; i++) {
        unsigned *uniforms_input = info->inputs_ptr;
        unsigned *uniforms_output = info->outputs_ptr;
        for(auto const& arg_info: info->args) {
            PimdArg arg = args[arg_info.index];
            switch(arg_info.type) {
            case SCALAR:
                *p++ = arg.u;
                break;

            case VECTOR:
                if (i == 0) {
                    std::memcpy(uniforms_input, arg.p, len * sizeof(unsigned));
                }
                *p++ = info->gpu_addr(uniforms_input);
                uniforms_input += GPU_LEN;
                break;

            case ADDR:
                *p++ = info->gpu_addr(uniforms_output);
                uniforms_output += GPU_LEN;
                break;

            default:
                break;
            }
        }
    }

    if (execute_qpu(info->mb, GPU_QPUS, info->gpu_addr(info->messages_ptr), 1, timeout)) {
        return -4;
    }

    unsigned *return_output = info->outputs_ptr;
    for(auto const& arg_info: info->args) {
        PimdArg arg = args[arg_info.index];
        if (arg_info.type == ADDR) {
            std::memcpy(arg.p, return_output, len * sizeof(unsigned));
            return_output += GPU_LEN;
        }
    }

    return 0;
}

void PimdFunction::free() {
    info->free();
}
