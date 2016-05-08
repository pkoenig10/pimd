#include <vector>
#include <stdio.h>
#include "blurpi.h"
#include "pimd.h"
//m x n matrix represented in a single array
double pi_blur(int m, int n, float * input, float* output){
    float* a = input;
    float* b =input + 1;
    float* c = input + 2;
    float* d = input + n;
    float* e = (input + n + 1);
    float* f = (input + n + 2);
    float* g = (input + n*2);
    float* h = (input + n*2 + 1);
    float* i = (input + n*2 + 2); 
    float* result = output + n + 1; 
   
    float scales[] = {.0586f,.0952f,.0586f,
                      .0952f, .1501f,.0952f,
                      .0586f,.0952f,.0586f};
    int mb = pimd_open();    

    std::vector<PimdOp> ops; std::vector <PimdArg> args;
    ops.push_back(OP_VLOAD); args.push_back(a); 
    ops.push_back(OP_SFMUL); args.push_back(scales[0]); 
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(b); 
    ops.push_back(OP_SFMUL); args.push_back(scales[1]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(c); 
    ops.push_back(OP_SFMUL); args.push_back(scales[2]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(d); 
    ops.push_back(OP_SFMUL); args.push_back(scales[3]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(e); 
    ops.push_back(OP_SFMUL); args.push_back(scales[4]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(f); 
    ops.push_back(OP_SFMUL); args.push_back(scales[5]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(g); 
    ops.push_back(OP_SFMUL); args.push_back(scales[6]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(h); 
    ops.push_back(OP_SFMUL); args.push_back(scales[7]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE0); 
    ops.push_back(OP_VLOAD); args.push_back(i); 
    ops.push_back(OP_SFMUL); args.push_back(scales[8]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_STORE); args.push_back(result);

    PimdFunction sqrt = PimdFunction(mb,&ops[0],ops.size()); 
    double ret = blur.call(&args[0], args.size(), N - 2*m - 2, 10000);
    if(ret<0){
        fprintf(stderr, "ERROR = %d\n", -ret);
        blur.free();
        pimd_close(mb);    
        return;
    }
    
    blur.free();
    pimd_close(mb);    
    return;


}
