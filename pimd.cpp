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
    unsigned mem_size;
    unsigned *gpu_ptr;
    unsigned *arm_ptr;
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

    unsigned gpu_addr(unsigned *p) {
        return (unsigned)(gpu_ptr + (p - arm_ptr));
    }

private:
    PimdOp *ops;
    int num_ops;

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

    if (!info->handle && alloc()) {
        return -2;
    }

    unsigned *p = info->uniforms_ptr;
    unsigned *uniforms_input = info->inputs_ptr;
    unsigned *uniforms_output = info->outputs_ptr;
    for(auto const& arg_info: info->args) {
        PimdArg arg = args[arg_info.index];
        switch(arg_info.type) {
        case SCALAR:
            *p++ = arg.u;
            break;

        case VECTOR:
            std::memcpy(uniforms_input, arg.p, len * sizeof(unsigned));
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

int PimdFunction::alloc() {
    if (info->handle) {
        return 0;
    }

    unsigned code_size = info->code.size();
    unsigned inputs_size = info->num_inputs * GPU_LEN;
    unsigned outputs_size = info->num_outputs * GPU_LEN;
    unsigned uniforms_size = info->args.size();
    unsigned messages_size = 2 * GPU_QPUS;

    info->mem_size = (code_size + inputs_size + outputs_size + uniforms_size + messages_size) * sizeof(unsigned);
    info->handle = mem_alloc(info->mb, info->mem_size, 4096, GPU_MEM_FLG);
    if (!info->handle) {
        return -1;
    }
    info->gpu_ptr = (unsigned*)mem_lock(info->mb, info->handle);
    info->arm_ptr = (unsigned*)mapmem(BUS_TO_PHYS((unsigned)info->gpu_ptr), info->mem_size);

    unsigned *p = info->arm_ptr;

    info->code_ptr = p;
    std::copy(info->code.begin(), info->code.end(), info->code_ptr);
    p += code_size;

    info->inputs_ptr = p;
    p += inputs_size;

    info->outputs_ptr = p;
    p += outputs_size;

    info->uniforms_ptr = p;
    p += uniforms_size;

    info->messages_ptr = p;
    for (int i=0; i<GPU_QPUS; ++i) {
        *p++ = info->gpu_addr(info->uniforms_ptr);
        *p++ = info->gpu_addr(info->code_ptr);
    }

    return 0;
}

void PimdFunction::free() {
    if (!info->handle) {
        return;
    }
    unmapmem(info->arm_ptr, info->mem_size);
    mem_unlock(info->mb, info->handle);
    mem_free(info->mb, info->handle);
    info->handle = 0;
}
