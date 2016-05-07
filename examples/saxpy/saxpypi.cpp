#include "saxpypi.hpp"
#include <stdio.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include "pimd.h"

double saxpy_pi(int N,
                float scale,
                float *X,
                float *Y,
                float *result){
    
    int mb = pimd_open();
    //CREATE FUNCTION         //PASS ARGS
    std::vector<PimdOp> ops; std::vector<PimdArg> args;
    ops.push_back(OP_VLOAD); args.push_back(X);
    ops.push_back(OP_SFMUL); args.push_back(scale);
    ops.push_back(OP_VFADD); args.push_back(Y);
    ops.push_back(OP_STORE); args.push_back(result);



    PimdFunction axpy = PimdFunction(mb,&ops[0],ops.size());

    double ret = axpy.call(&args[0], args.size(), N, 10000);
    
    if(ret < 0.f){
        fprintf(stderr,"ERROR = %f\n", -ret);
        return 1e30;
    }
    axpy.free();
    pimd_close(mb);
    return ret;
}
