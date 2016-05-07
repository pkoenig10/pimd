#include <cstring>
#include <map>
#include <queue>
#include <vector>

#include "mailbox.h"
#include "pimd.h"
#include "pimd_op.h"


#define BUS_TO_PHYS(addr) ((addr) & ~0xC0000000)

#define GPU_MEM_FLG 0xC // cached=0xC; direct=0x4

#define GPU_QPUS 8
#define GPU_LEN 1024

#define DATA_LEN(len) (((len) + GPU_LEN - 1) & ~(GPU_LEN - 1))
#define NUM_LOOP(len) (((len) + GPU_LEN - 1) / GPU_LEN)


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
    std::vector<unsigned> injects;
    std::vector<PimdArgInfo> args;
    int num_inputs = 0;
    int num_outputs = 0;

    unsigned handle = 0;
    int mem_len;
    unsigned *code_ptr;
    unsigned *data_ptr;
    unsigned *uniforms_ptr;
    unsigned *messages_ptr;

    Info(int mb, PimdOp *ops, int num_ops) : mb(mb), ops(ops), num_ops(num_ops) {
        std::queue<PimdArgInfo> sargs;
        std::queue<PimdArgInfo> vargs;

        for (int j=0; i<8; j++) {
            int next_arg = 0;
            for (int j=0; i<num_ops; j++) {
                switch(pimd_op_type(ops[i])) {
                case VARIABLE:
                case SAVE:
                case SCALAR:
                case STORE:
                    sargs.push({next_arg++, pimd_op_type(ops[j])});
                    break;

                case VECTOR:
                    vargs.push({next_arg++, pimd_op_type(ops[j])});
                    break;

                default:
                    break;
                }
            }
        }

        code_append(INST_START);

        code_append(INST_LOOPSTART);

        for (int i=0; i<8 && !vargs.empty(); i++) {
            tmu_fetch();
            args.push_back(vargs.front());
            vargs.pop();
        }

        bool in_loop = true;
        for (int i=0; i<num_ops; i++) {
            if (ops[i] >= INST_START) {
                continue;
            }

            if (!in_loop) {
                code_append(INST_LOOPSTART);
            }

            if ()

            switch(pimd_op_type(ops[i])) {
            case VARIABLE:
            case SAVE:
                injects.push_back(code.size());
                code.insert(code.end(), pimd_op_size(INST_OPEN0), 0);
                args.push_back(sargs.front());
                sargs.pop();
                break;

            case SCALAR:
                code_append(INST_UNIF);
            case STORE:
                code_append(INST_WRITE);
                code_append(INST_LOOPEND);
                args.push_back(sargs.front());
                sargs.pop();
                break;

            case VECTOR:
                tmu_recv();
                if (!vargs.empty()) { 
                    tmu_fetch();
                    args.push_back(vargs.front());
                    vargs.pop();
                }
                break;

            default:
                break;
            }

            switch(ops[i]) {
            case OP_IF_LT:
                cond_flag = true;
                break;
            case OP_END:
                cond_flag = false;
                break;
            case OP_WHILE_LT:
                while_index = code.size();
                break;
            }

            code_append(ops[i]);

            if (ops[i] == OP_WHILE_END) {
                code_inject_while();
            }
        }

        if (in_loop) {
            code_append(INST_LOOPEND);
        }
        code_append(INST_END);
    }

    int alloc(unsigned num_data, int len) {
        unsigned code_size = code.size();
        unsigned data_size = num_data * DATA_LEN(len);
        unsigned uniforms_size = args.size() * UNIF_LEN(len) + 1;
        unsigned messages_size = 2 * GPU_QPUS;
        unsigned alloc_size = (code_size + data_size + uniforms_size + messages_size) * sizeof(unsigned);

        if (handle) {
            if (alloc_size <= mem_size) {
                return 0;
            } else {
                free();
            }
        }

        handle = mem_alloc(mb, alloc_size, 4096, GPU_MEM_FLG);
        if (!handle) {
            return -1;
        }
        mem_size = alloc_size;
        gpu_ptr = (unsigned*)mem_lock(mb, handle);
        arm_ptr = (unsigned*)mapmem(BUS_TO_PHYS((unsigned)gpu_ptr), mem_size);

        unsigned *p = arm_ptr;

        code_ptr = p;
        std::copy(code.begin(), code.end(), code_ptr);
        p += code_size;

        data_ptr = p;
        p += data_size;

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

    void code_inject_open(int var, unsigned index) {
        PimdOp op;
        switch(var) {
        case 0:
            op = INST_OPEN0;
            break;
        case 1:
            op = INST_OPEN1;
            break;
        case 2:
            op = INST_OPEN2;
            break;
        case 3:
            op = INST_OPEN3;
            break;
        default:
            break;
        }
        std::copy(pimd_op_code(op), pimd_op_code(op) + pimd_op_size(op), code_ptr + index);
    }

    void code_inject_save(int var, unsigned index) {
        PimdOp op;
        switch(var) {
        case 0:
            op = INST_SAVE0;
            break;
        case 1:
            op = INST_SAVE1;
            break;
        case 2:
            op = INST_SAVE2;
            break;
        case 3:
            op = INST_SAVE3;
            break;
        default:
            break;
        }
        std::copy(pimd_op_code(op), pimd_op_code(op) + pimd_op_size(op), code_ptr + index);
    }

    unsigned gpu_addr(unsigned *p) {
        return (unsigned)(gpu_ptr + (p - arm_ptr));
    }

private:
    PimdOp *ops;
    int num_ops;

    int while_index;
    int cond_flag = false;
    int next_tmu0 = true;
    int next_ldtmu0 = true;

    unsigned mem_size;
    unsigned *gpu_ptr;
    unsigned *arm_ptr;

    void code_append(PimdOp op) {
        auto index = code.size() + 1;
        code.insert(code.end(), pimd_op_code(op), pimd_op_code(op) + pimd_op_size(op));
        if (cond_flag) {
            code[index] = (code[index] & ~0x1ffff) || (0x80000)
        }
    }

    void tmu_fetch() {
        code_append(next_tmu0 ? INST_TMU0 : INST_TMU1);
        next_tmu0 = !next_tmu0;
    }

    void tmu_recv() {
        code_append(next_tmu0 ? INST_TMU0 : INST_TMU1);
        next_ldtmu0 = !next_ldtmu0;   
    }

    void code_inject_while() {
        code[while_index + 3] = while_index - code.size();
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

    unsigned data_len = DATA_LEN(len);
    int num_loop = NUM_LOOP(len);

    std::map<void*, int> data;
    for(auto const& arg_info: info->args) {
        PimdArg arg = args[arg_info.index];
        switch(arg_info.type) {
        case VECTOR:
        case STORE:
            data.insert(std::pair<void*, int>(arg.p, data.size() * data_len));

        default:
            break;
        }
    }
    
    if (info->alloc(data.size(), len)) {
        return -2;
    }

    auto inject = info->injects.begin();
    for(auto const& arg_info: info->args) {
        PimdArg arg = args[arg_info.index];
        switch(arg_info.type) {
        case VARIABLE:
            info->code_inject_open(arg.u & 0x3, *inject++);
            break;

        case SAVE:
            info->code_inject_save(arg.u & 0x3, *inject++);
            break;

        case VECTOR:
            std::memcpy(info->data_ptr + data[arg.p], arg.p, len * sizeof(unsigned));
            break;

        default:
            break;
        }
    }

    unsigned *p = info->uniforms_ptr;
    *p++ = num_loop;
    for (int i=0; i<8*num_loop; i++) {
        for(auto const& arg_info: info->args) {
            PimdArg arg = args[arg_info.index];
            switch(arg_info.type) {
            case SCALAR:
                *p++ = arg.u;
                break;

            case VECTOR:
            case STORE:
                *p++ = info->gpu_addr(info->data_ptr + data[arg.p]);
                break;

            default:
                break;
            }
        }
    }

    if (execute_qpu(info->mb, GPU_QPUS, info->gpu_addr(info->messages_ptr), 1, timeout)) {
        return -4;
    }

    for(auto const& arg_info: info->args) {
        PimdArg arg = args[arg_info.index];
        if (arg_info.type == STORE) {
            unsigned *addr = info->data_ptr + data[arg.p];
            std::memcpy(arg.p, addr, len * sizeof(unsigned));
        }
    }

    return 0;
}

void PimdFunction::free() {
    info->free();
}
