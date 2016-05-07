#include "pimd.h"
#include "pimd_op.h"

static unsigned op_load[] = {
    #include "hex/op_load.hex"
};
static unsigned op_store[] = {
    #include "hex/op_store.hex"
};
static unsigned op_open[] = {
    #include "hex/op_open.hex"
};
static unsigned op_save[] = {
    #include "hex/op_save.hex"
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
static unsigned op_abs[] = {
    #include "hex/op_abs.hex"
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
static unsigned op_fmul[] = {
    #include "hex/op_mul.hex"
};
static unsigned op_exp[] = {
    #include "hex/op_exp.hex"
};
static unsigned op_log[] = {
    #include "hex/op_log.hex"
};
static unsigned op_recip[] = {
    #include "hex/op_recip.hex"
};
static unsigned op_recipsqrt[] = {
    #include "hex/op_recipsqrt.hex"
};
static unsigned op_sqrt[] = {
    #include "hex/op_sqrt.hex"
};
static unsigned inst_start[] = {
    #include "hex/inst_start.hex"
};
static unsigned inst_end[] = {
    #include "hex/inst_end.hex"
};
static unsigned inst_unif[] = {
    #include "hex/inst_unif.hex"
};
static unsigned inst_tmu0[] = {
    #include "hex/inst_tmu0.hex"
};
static unsigned inst_tmu1[] = {
    #include "hex/inst_tmu1.hex"
};
static unsigned inst_ldtmu0[] = {
    #include "hex/inst_ldtmu0.hex"
};
static unsigned inst_ldtmu1[] = {
    #include "hex/inst_ldtmu1.hex"
};
static unsigned inst_open0[] = {
    #include "hex/inst_open0.hex"
};
static unsigned inst_open1[] = {
    #include "hex/inst_open1.hex"
};
static unsigned inst_open2[] = {
    #include "hex/inst_open2.hex"
};
static unsigned inst_open3[] = {
    #include "hex/inst_open3.hex"
};
static unsigned inst_save0[] = {
    #include "hex/inst_save0.hex"
};
static unsigned inst_save1[] = {
    #include "hex/inst_save1.hex"
};
static unsigned inst_save2[] = {
    #include "hex/inst_save2.hex"
};
static unsigned inst_save3[] = {
    #include "hex/inst_save3.hex"
};

static struct {
    PimdOpType type;
    unsigned size;
    unsigned *code;
}
pimd_ops[] = {
    {SCALAR,    sizeof(op_load),       op_load},
    {VECTOR,    sizeof(op_load),       op_load},
    {STORE,     sizeof(op_store),      op_store},
    {VARIABLE,  sizeof(op_open),       op_open},
    {SAVE,      sizeof(op_save),       op_save},
    {VARIABLE,  sizeof(op_add),        op_add},
    {VARIABLE,  sizeof(op_sub),        op_sub},
    {VARIABLE,  sizeof(op_min),        op_min},
    {VARIABLE,  sizeof(op_max),        op_max},
    {VARIABLE,  sizeof(op_abs),        op_abs},
    {VARIABLE,  sizeof(op_and),        op_and},
    {VARIABLE,  sizeof(op_or),         op_or},
    {VARIABLE,  sizeof(op_xor),        op_xor},
    {VARIABLE,  sizeof(op_shl),        op_shl},
    {VARIABLE,  sizeof(op_shr),        op_shr},
    {VARIABLE,  sizeof(op_asr),        op_asr},
    {VARIABLE,  sizeof(op_ror),        op_ror},
    {VARIABLE,  sizeof(op_mul24),      op_mul24},
    {VARIABLE,  sizeof(op_mul),        op_mul},
    {VARIABLE,  sizeof(op_fadd),       op_fadd},
    {VARIABLE,  sizeof(op_fsub),       op_fsub},
    {VARIABLE,  sizeof(op_fmin),       op_fmin},
    {VARIABLE,  sizeof(op_fmax),       op_fmax},
    {VARIABLE,  sizeof(op_fminabs),    op_fminabs},
    {VARIABLE,  sizeof(op_fmaxabs),    op_fmaxabs},
    {VARIABLE,  sizeof(op_fmul),       op_fmul},
    {SCALAR,    sizeof(op_add),        op_add},
    {SCALAR,    sizeof(op_sub),        op_sub},
    {SCALAR,    sizeof(op_min),        op_min},
    {SCALAR,    sizeof(op_max),        op_max},
    {SCALAR,    sizeof(op_abs),        op_abs},
    {SCALAR,    sizeof(op_and),        op_and},
    {SCALAR,    sizeof(op_or),         op_or},
    {SCALAR,    sizeof(op_xor),        op_xor},
    {SCALAR,    sizeof(op_shl),        op_shl},
    {SCALAR,    sizeof(op_shr),        op_shr},
    {SCALAR,    sizeof(op_asr),        op_asr},
    {SCALAR,    sizeof(op_ror),        op_ror},
    {SCALAR,    sizeof(op_mul24),      op_mul24},
    {SCALAR,    sizeof(op_mul),        op_mul},
    {SCALAR,    sizeof(op_fadd),       op_fadd},
    {SCALAR,    sizeof(op_fsub),       op_fsub},
    {SCALAR,    sizeof(op_fmin),       op_fmin},
    {SCALAR,    sizeof(op_fmax),       op_fmax},
    {SCALAR,    sizeof(op_fminabs),    op_fminabs},
    {SCALAR,    sizeof(op_fmaxabs),    op_fmaxabs},
    {SCALAR,    sizeof(op_fmul),       op_fmul},
    {VECTOR,    sizeof(op_add),        op_add},
    {VECTOR,    sizeof(op_sub),        op_sub},
    {VECTOR,    sizeof(op_min),        op_min},
    {VECTOR,    sizeof(op_max),        op_max},
    {VECTOR,    sizeof(op_abs),        op_abs},
    {VECTOR,    sizeof(op_and),        op_and},
    {VECTOR,    sizeof(op_or),         op_or},
    {VECTOR,    sizeof(op_xor),        op_xor},
    {VECTOR,    sizeof(op_shl),        op_shl},
    {VECTOR,    sizeof(op_shr),        op_shr},
    {VECTOR,    sizeof(op_asr),        op_asr},
    {VECTOR,    sizeof(op_ror),        op_ror},
    {VECTOR,    sizeof(op_mul24),      op_mul24},
    {VECTOR,    sizeof(op_mul),        op_mul},
    {VECTOR,    sizeof(op_fadd),       op_fadd},
    {VECTOR,    sizeof(op_fsub),       op_fsub},
    {VECTOR,    sizeof(op_fmin),       op_fmin},
    {VECTOR,    sizeof(op_fmax),       op_fmax},
    {VECTOR,    sizeof(op_fminabs),    op_fminabs},
    {VECTOR,    sizeof(op_fmaxabs),    op_fmaxabs},
    {VECTOR,    sizeof(op_fmul),       op_fmul},
    {NONE,      sizeof(op_not),        op_not},
    {NONE,      sizeof(op_clz),        op_clz},
    {NONE,      sizeof(op_itof),       op_itof},
    {NONE,      sizeof(op_ftoi),       op_ftoi},
    {NONE,      sizeof(op_exp),        op_exp},
    {NONE,      sizeof(op_log),        op_log},
    {NONE,      sizeof(op_recip),      op_recip},
    {NONE,      sizeof(op_recipsqrt),  op_recipsqrt},
    {NONE,      sizeof(op_sqrt),       op_sqrt},
    {NONE,      sizeof(inst_start),    inst_start},
    {NONE,      sizeof(inst_end),      inst_end},
    {NONE,      sizeof(inst_unif),     inst_unif},
    {NONE,      sizeof(inst_tmu0),      inst_tmu0},
    {NONE,      sizeof(inst_tmu1),      inst_tmu1},
    {NONE,      sizeof(inst_ldtmu0),    inst_ldtmu0},
    {NONE,      sizeof(inst_ldtmu1),    inst_ldtmu1},
    {NONE,      sizeof(inst_open0),    inst_open0},
    {NONE,      sizeof(inst_open1),    inst_open1},
    {NONE,      sizeof(inst_open2),    inst_open2},
    {NONE,      sizeof(inst_open3),    inst_open3},
    {NONE,      sizeof(inst_save0),    inst_save0},
    {NONE,      sizeof(inst_save1),    inst_save1},
    {NONE,      sizeof(inst_save2),    inst_save2},
    {NONE,      sizeof(inst_save3),    inst_save3},
};


PimdOpType pimd_op_type(PimdOp op) {
    return pimd_ops[op].type;
}

unsigned pimd_op_size(PimdOp op) {
    return pimd_ops[op].size / sizeof(unsigned);
}

unsigned *pimd_op_code(PimdOp op) {
    return pimd_ops[op].code;
}
