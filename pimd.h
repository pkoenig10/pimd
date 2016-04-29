#ifndef PIMD_H_
#define PIMD_H_

enum PimdOp {
    OP_LOAD = 0,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MIN,
    OP_MAX,
    OP_ABS,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_SHL,
    OP_SHR,
    OP_ASR,
    OP_ROR,
    OP_FADD,
    OP_FSUB,
    OP_FMIN,
    OP_FMAX,
    OP_FMINABS,
    OP_FMAXABS,
    OP_NOT,
    OP_CLZ,
    OP_ITOF,
    OP_FTOI,
    OP_MUL24,
    OP_MUL,
    OP_FMUL,
    OP_EXP,
    OP_LOG,
    OP_RECIP,
    OP_RECIPSQRT,
    OP_SQRT,
    INST_START,
    INST_END,
    INST_UNIF,
    INST_TMU,
    INST_LDTMU,
};

struct PimdArg {
    enum {SCALAR, VECTOR, ADDR} type;
    union {
        unsigned u;
        int i;
        float f;
        void *p;
    };

	PimdArg(int i) : type(SCALAR), i(i) {}
	PimdArg(float f) : type(SCALAR), f(f) {}
	PimdArg(void *p) : type(VECTOR), p(p) {}
};

int pimd_open();
int pimd_close(int mb);

int pimd_op_num_args(PimdOp op);
unsigned pimd_op_size(PimdOp op);
unsigned *pimd_op_code(PimdOp op);

int pimd_execute(int mb, PimdOp *ops, int num_ops, PimdArg *args, int num_args, int len, int timeout);

#endif
