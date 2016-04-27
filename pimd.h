#ifndef PIMD_H_
#define PIMD_H_

#include <mailbox.h>

enum PimdOp {
    OP_SLOAD = 0,
    OP_VLOAD,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MIN,
    OP_MAX,
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
    OP_DIV,
    OP_FMUL,
    OP_FDIV,
};

enum PimdArgType {SCALAR, VECTOR};
struct PimdArg {
	PimdArgType type;
	unsigned arg;

	PimdArg(int x) : type(SCALAR), arg((unsigned)x) {}
	PimdArg(float x) : type(SCALAR), arg((unsigned)x) {}
	PimdArg(void *p) : type(VECTOR), arg((unsigned)p) {}
};

int pimd_open();
int pimd_close();

int pimd_op_num_args(PimdOp op);
unsigned pimd_op_size(PimdOp op);
unsigned *pimd_op_code(PimdOp op);

int pimd_execute(PimdOp *ops, int num_ops, PimdArg *args, int num_args, int size, int chunk_size);

#endif
