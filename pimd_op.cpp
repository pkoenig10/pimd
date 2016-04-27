#include "pimd.h"

static unsigned op_sload[] = {
    #include "hex/op_sload.hex"
};
static unsigned op_vload[] = {
    #include "hex/op_vload.hex"
};
static unsigned op_store[] = {
    #include "hex/op_store.hex"
};
static unsigned op_add[] = {
    #include "hex/op_add.hex"
};
static unsigned op_sub[] = {
    #include "hex/op_sub.hex"
};
static unsigned op_min[] = {
    #include "hex/op_min.hex"
};
static unsigned op_max[] = {
    #include "hex/op_max.hex"
};
static unsigned op_and[] = {
    #include "hex/op_and.hex"
};
static unsigned op_or[] = {
    #include "hex/op_or.hex"
};
static unsigned op_xor[] = {
    #include "hex/op_xor.hex"
};
static unsigned op_shl[] = {
    #include "hex/op_shl.hex"
};
static unsigned op_shr[] = {
    #include "hex/op_shr.hex"
};
static unsigned op_asr[] = {
    #include "hex/op_asr.hex"
};
static unsigned op_ror[] = {
    #include "hex/op_ror.hex"
};
static unsigned op_fadd[] = {
    #include "hex/op_fadd.hex"
};
static unsigned op_fsub[] = {
    #include "hex/op_fsub.hex"
};
static unsigned op_fmin[] = {
    #include "hex/op_fmin.hex"
};
static unsigned op_fmax[] = {
    #include "hex/op_fmax.hex"
};
static unsigned op_fminabs[] = {
    #include "hex/op_fminabs.hex"
};
static unsigned op_fmaxabs[] = {
    #include "hex/op_fmaxabs.hex"
};
static unsigned op_not[] = {
    #include "hex/op_not.hex"
};
static unsigned op_clz[] = {
    #include "hex/op_clz.hex"
};
static unsigned op_itof[] = {
    #include "hex/op_itof.hex"
};
static unsigned op_ftoi[] = {
    #include "hex/op_ftoi.hex"
};
static unsigned op_mul24[] = {
    #include "hex/op_mul24.hex"
};
static unsigned op_mul[] = {
    #include "hex/op_mul.hex"
};
static unsigned op_div[] = {
    #include "hex/op_div.hex"
};
static unsigned op_fmul[] = {
    #include "hex/op_mul.hex"
};
static unsigned op_fdiv[] = {
    #include "hex/op_fdiv.hex"
};

static struct {
    int num_args;
    unsigned size;
    unsigned *code;
}
pimd_ops[] = {
    {1, sizeof(op_sload), op_sload},
    {1, sizeof(op_vload), op_vload},
    {1, sizeof(op_store), op_store},
    {1, sizeof(op_add), op_add},
    {1, sizeof(op_sub), op_sub},
    {1, sizeof(op_min), op_min},
    {1, sizeof(op_max), op_max},
    {1, sizeof(op_and), op_and},
    {1, sizeof(op_or), op_or},
    {1, sizeof(op_xor), op_xor},
    {1, sizeof(op_shl), op_shl},
    {1, sizeof(op_shr), op_shr},
    {1, sizeof(op_asr), op_asr},
    {1, sizeof(op_ror), op_ror},
    {1, sizeof(op_fadd), op_fadd},
    {1, sizeof(op_fsub), op_fsub},
    {1, sizeof(op_fmin), op_fmin},
    {1, sizeof(op_fmax), op_fmax},
    {1, sizeof(op_fminabs), op_fminabs},
    {1, sizeof(op_fmaxabs), op_fmaxabs},
    {0, sizeof(op_not), op_not},
    {0, sizeof(op_clz), op_clz},
    {0, sizeof(op_itof), op_itof},
    {0, sizeof(op_ftoi), op_ftoi},
    {1, sizeof(op_mul24), op_mul24},
    {1, sizeof(op_mul), op_mul},
    {1, sizeof(op_div), op_div},
    {1, sizeof(op_fmul), op_fmul},
    {1, sizeof(op_fdiv), op_fdiv},
};

int pimd_op_num_args(PimdOp op) {
    return pimd_ops[op].num_args;
}

unsigned pimd_op_size(PimdOp op) {
    return pimd_ops[op].size;
}

unsigned *pimd_op_code(PimdOp op) {
    return pimd_ops[op].code;
}
