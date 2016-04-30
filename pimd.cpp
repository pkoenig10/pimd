#include <cstring>
#include <vector>

#include "mailbox.h"
#include "pimd.h"


#define BUS_TO_PHYS(addr) ((addr) & ~0xC0000000)

#define GPU_MEM_FLG 0xC // cached=0xC; direct=0x4

#define GPU_QPUS 8
#define GPU_LEN 1024


inline unsigned gpu_addr(unsigned *arm_ptr, unsigned *gpu_ptr, unsigned *p) {
    return (unsigned)(gpu_ptr + (p - arm_ptr));
}

class PimdInfo {
private:
    PimdOp *ops;
    int num_ops;
    PimdArg *args;
    int num_args;

    int next_arg = 0;
    int next_tmu_arg = 0;

public:
    std::vector<unsigned> qpu_code;
    std::vector<PimdArg> qpu_args;
    int num_inputs = 0;
    int num_outputs = 0;

    PimdInfo(PimdOp *ops, int num_ops, PimdArg *args, int num_args) :
    ops(ops), num_ops(num_ops), args(args), num_args(num_args) {
        for (int i=0; i<num_ops; i++) {
            if (pimd_op_num_args(ops[i])) {
                if (ops[i] == OP_STORE) {
                    args[next_arg].type = PimdArg::ADDR;
                }
                next_arg++;
            }
        }
        next_arg = 0;

        code_append(INST_START);

        for (int i=0; i<4; i++) {
            tmu_fetch();
        }

        for (int i=0; i<num_ops; i++) {
            if (pimd_op_num_args(ops[i])) {
                load_arg(args[next_arg++]);
            }

            code_append(ops[i]);
        }

        code_append(INST_END);
    }

    void code_append(PimdOp op) {
        qpu_code.insert(qpu_code.end(), pimd_op_code(op), pimd_op_code(op) + pimd_op_size(op));
    }

    void tmu_fetch() {
        for(; next_tmu_arg < num_args; next_tmu_arg++) {
            if (args[next_tmu_arg].type == PimdArg::VECTOR) {
                code_append(INST_TMU);
                qpu_args.push_back(args[next_tmu_arg++]);
                break;
            }
        }
    }

    void tmu_recv() {
        code_append(INST_LDTMU);
        tmu_fetch();
    }

    void load_arg(PimdArg arg) {
        switch(arg.type) {
        case PimdArg::SCALAR:
            code_append(INST_UNIF);
            qpu_args.push_back(arg);
            break;

        case PimdArg::PimdArg::VECTOR:
            num_inputs++;
            tmu_recv();
            break;

        case PimdArg::PimdArg::ADDR:
            num_outputs++;
            qpu_args.push_back(arg);
            break;
        }
    }
};


int pimd_open() {
    int mb = mbox_open();

    if (qpu_enable(mb, 1)) {
        mbox_close(mb);
        return 0;
    }

    return mb;
}

int pimd_close(int mb) {
    qpu_enable(mb, 0);

    mbox_close(mb);

    return 0;
}

int pimd_execute(int mb, PimdOp *ops, int num_ops, PimdArg *args, int num_args, int len, int timeout) {
    if (len < 0 || len > GPU_LEN) {
        return -1;
    }

    int ops_args = 0;
    for (int i=0; i<num_ops; i++) {
        ops_args += pimd_op_num_args(ops[i]);
    }
    if (ops_args != num_args) {
        return -2;
    }

    PimdInfo info = PimdInfo(ops, num_ops, args, num_args);

    unsigned code_size = info.qpu_code.size();
    unsigned inputs_size = info.num_inputs * GPU_LEN;
    unsigned outputs_size = info.num_outputs * GPU_LEN;
    unsigned uniforms_size = info.qpu_args.size();
    unsigned messages_size = 2 * GPU_QPUS;

    unsigned mem_size = (code_size + inputs_size + outputs_size + uniforms_size + messages_size) * sizeof(unsigned);
    unsigned handle = mem_alloc(mb, mem_size, 4096, GPU_MEM_FLG);
    if (!handle) {
        return -3;
    }
    unsigned *gpu_ptr = (unsigned*)mem_lock(mb, handle);
    unsigned *arm_ptr = (unsigned*)mapmem(BUS_TO_PHYS((unsigned)gpu_ptr), mem_size);

    unsigned *p = arm_ptr;

    unsigned *code = p;
    std::copy(info.qpu_code.begin(), info.qpu_code.end(), code);
    p += code_size;

    unsigned *inputs = p;
    p += inputs_size;

    unsigned *outputs = p;
    p += outputs_size;

    unsigned *uniforms = p;
    unsigned *uniforms_input = inputs;
    unsigned *uniforms_output = outputs;
    for(auto const& arg: info.qpu_args) {
        switch(arg.type) {
        case PimdArg::SCALAR:
            *p++ = arg.u;
            break;

        case PimdArg::VECTOR:
            std::memcpy(uniforms_input, arg.p, len);
            *p++ = gpu_addr(arm_ptr, gpu_ptr, uniforms_input);
            uniforms_input += GPU_LEN;
            break;

        case PimdArg::ADDR:
            *p++ = gpu_addr(arm_ptr, gpu_ptr, uniforms_output);
            uniforms_output += GPU_LEN;
            break;
        }
    }

    unsigned *messages = p;
    for (int i=0; i<GPU_QPUS; ++i) {
        *p++ = gpu_addr(arm_ptr, gpu_ptr, uniforms);
        *p++ = gpu_addr(arm_ptr, gpu_ptr, code);
    }

    if (execute_qpu(mb, GPU_QPUS, gpu_addr(arm_ptr, gpu_ptr, messages), 1, timeout)) {
        unmapmem(arm_ptr, mem_size);
        mem_unlock(mb, handle);
        mem_free(mb, handle);
        return -4;
    }

    unsigned *return_output = outputs;
    for(auto const& arg: info.qpu_args) {
        if (arg.type == PimdArg::ADDR) {
            std::memcpy(arg.p, return_output, len);
            return_output += GPU_LEN;
        }
    }

    unmapmem(arm_ptr, mem_size);
    mem_unlock(mb, handle);
    mem_free(mb, handle);

    return 0;
}
