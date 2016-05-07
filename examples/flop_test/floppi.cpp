#include "floppi.h"

#include "pimd.h"

#include <algorithm>
#include <vector>
#include <stdio.h>

double flop_pi(int N,int NUM_OPS,float * X,float *result){
    int mb = pimd_open();
    std::vector<PimdOp> ops; std::vector<PimdArg> args;
    ops.push_back(OP_VLOAD); args.push_back(X);   
    for(int i = 0; i<NUM_OPS; i++){
        ops.push_back(OP_SFADD); args.push_back(1.f);
    }
    ops.push_back(OP_STORE); args.push_back(result);
    PimdFunction flop = PimdFunction(mb,&ops[0], ops.size());
    double ret = flop.call(&args[0], args.size(), N, 10000);
    if(ret < 0.f){
        fprintf(stderr, "ERROR = %f\n", -ret);
        return 1e30;
    }
    flop.free();
    pimd_close(mb);
    return ret;
}
