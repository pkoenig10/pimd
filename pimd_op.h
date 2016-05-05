#ifndef PIMD_OP_H_
#define PIMD_OP_H_

enum PimdOpType {
	NONE,
	VARIABLE,
	SAVE,
	SCALAR,
	VECTOR,
	STORE,
};

PimdOpType pimd_op_type(PimdOp op);
unsigned pimd_op_size(PimdOp op);
unsigned *pimd_op_code(PimdOp op);

#endif
