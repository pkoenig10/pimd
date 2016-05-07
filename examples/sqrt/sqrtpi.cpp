
#include <vector>
#include <time.h>
#include <stdio.h>

#include "pimd.h"


void sqrtPi (int N,
                float initialGuess,
                float values[],
                float output[])
{
    static const float kThreshold = 0.00001f;
    std::vector<PimdOp> ops; std::vector<PimdArg> args;
    
    ops.push_back(OP_VLOAD); args.push_back(values);
    ops.push_back(OP_SAVE1);
    ops.push_back(OP_VLOAD); args.push_back(initialGuess);
    ops.push_back(OP_SAVE0); //Store guess
    ops.push_back(OP_VMUL); args.push_back(VAR_1); //Mul X
    ops.push_back(OP_VMUL); args.push_back(VAR_1); //Mul X
    ops.push_back(OP_SFSUB); args.push_back(1.f); // -1.f
    ops.push_back(OP_FABS); //Abs of above
    ops.push_back(OP_SWHILE_GT); args.push_back(kThreshold); 
    // While Error > thresh
    
    //START GUESS CALC
    ops.push_back(OP_OPEN0); 
    ops.push_back(OP_SFMUL); args.push_back(3.f);
    ops.push_back(OP_SAVE2); 
    ops.push_back(OP_LOAD); args.push_back(VAR_1);
    ops.push_back(OP_MUL); args.push_back(VAR_0);
    ops.push_back(OP_MUL); args.push_back(VAR_0);
    ops.push_back(OP_MUL); args.push_back(VAR_0);
    ops.push_back(OP_SAVE3);
    
    ops.push_back(OP_OPEN2);
    ops.push_back(OP_FSUB); args.push_back(VAR_3);
    ops.push_back(OP_SAVE0);
    //END CALC GUESS 
    ops.push_back(OP_FMUL); args.push_back(VAR_0);
    ops.push_back(OP_FMUL); args.push_back(VAR_1);
    ops.push_back(OP_SSUB); args.push_back(1.f); 
    ops.push_back(OP_FABS); 

    ops.push_back(OP_END);

    ops.push_back(OP_OPEN0);
    ops.push_back(OP_FMUL); args.push_back(VAR_1);
    ops.push_back(OP_STORE); args.push_back(output);

    int mb = pimd_open(); 
    PimdFunction sqrt = PimdFunction(mb,&ops[0],ops.size()); 
    int ret = sqrt.call(&args[0], args.size(), N, 10000);
    if(ret<0){
        fprintf(stderr, "ERROR = %d\n", -ret);
        sqrt.free();
        pimd_close(mb);    
        return;
    }
    
    sqrt.free();
    pimd_close(mb);    
    return;
}
