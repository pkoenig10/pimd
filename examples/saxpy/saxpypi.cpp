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
    //CREATE FUNCTION 
    std::vector<PimdOp> ops;
    ops.push_back(OP_VLOAD); 
    ops.push_back(OP_SFMUL);
    ops.push_back(OP_VFADD);
    ops.push_back(OP_STORE);

    //PASS ARGS
    std::vector<PimdArg> args;
    args.push_back(X);
    args.push_back(scale);
    args.push_back(Y);
    args.push_back(result);


    PimdFunction axpy = PimdFunction(mb,&ops[0],ops.size());
	//Dying inside call
    clock_t start = clock();
    int ret = axpy.call(&args[0], args.size(), N, 10000);
    clock_t end = clock(); 
    if(ret < 0){
        fprintf(stderr,"ERROR = %d\n", -ret);
        return 1e30;
    }
    axpy.free();
    pimd_close(mb);
    return (double)end-start;
}
