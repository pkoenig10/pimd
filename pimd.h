#ifndef PIMD_H_
#define PIMD_H_

enum PimdOp {
    OP_SLOAD,
    OP_VLOAD,
    OP_STORE,
    OP_OPEN,
    OP_SAVE,
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
    OP_MUL24,
    OP_MUL,
    OP_FADD,
    OP_FSUB,
    OP_FMIN,
    OP_FMAX,
    OP_FMINABS,
    OP_FMAXABS,
    OP_FMUL,
    OP_SADD,
    OP_SSUB,
    OP_SMIN,
    OP_SMAX,
    OP_SABS,
    OP_SAND,
    OP_SOR,
    OP_SXOR,
    OP_SSHL,
    OP_SSHR,
    OP_SASR,
    OP_SROR,
    OP_SMUL24,
    OP_SMUL,
    OP_SFADD,
    OP_SFSUB,
    OP_SFMIN,
    OP_SFMAX,
    OP_SFMINABS,
    OP_SFMAXABS,
    OP_SFMUL,
    OP_VADD,
    OP_VSUB,
    OP_VMIN,
    OP_VMAX,
    OP_VABS,
    OP_VAND,
    OP_VOR,
    OP_VXOR,
    OP_VSHL,
    OP_VSHR,
    OP_VASR,
    OP_VROR,
    OP_VMUL24,
    OP_VMUL,
    OP_VFADD,
    OP_VFSUB,
    OP_VFMIN,
    OP_VFMAX,
    OP_VFMINABS,
    OP_VFMAXABS,
    OP_VFMUL,
    OP_NOT,
    OP_CLZ,
    OP_ITOF,
    OP_FTOI,
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
    INST_OPEN0,
    INST_OPEN1,
    INST_OPEN2,
    INST_OPEN3,
    INST_SAVE0,
    INST_SAVE1,
    INST_SAVE2,
    INST_SAVE3,
};

enum PimdVar {
    VAR_0,
    VAR_1,
    VAR_2,
    VAR_3,
};

union PimdArg {
    unsigned u;
    int i;
    float f;
    void *p;

    PimdArg(int i) : i(i) {}
    PimdArg(float f) : f(f) {}
    PimdArg(double d) : f((float)d) {}
    PimdArg(void *p) : p(p) {}
};

int pimd_open();
void pimd_close(int mb);

class PimdFunction {
private:
    class Info;
    Info *info;

public:
    PimdFunction(int mb, PimdOp *ops, int num_ops);
    ~PimdFunction();
    
    int call(PimdArg *args, int num_args, int len, int timeout);
    int call(PimdArg *args, int num_args, int len, int timeout, float *runtime);
    void free();
};

#endif
