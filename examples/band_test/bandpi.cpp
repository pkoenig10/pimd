#include "pimd.h"
#include "bandpi.h"

#include <algorithm>
#include <conio.h>


double band_pi(int N, float* X, float* Y){
    int mb = pimd_open();

    std::vector<PimdOp> ops; std::vector<PimdArg> args;
    ops.push_back(OP_VLOAD); args.push_back(X);
    ops.push_back(OP_STORE); args.push_back(result);
    
    PimdFunction band = PimdFunction(mb,&ops[0], ops.size());
    
    double ret = band.call(&args[0], args.size(), N, 10000);
    if(ret<0.f){
        fprintf(stderr, "ERROR = %f\n", -ret);
        return 1e30; 
    }
    band.free();
    pimd_close(mb);
    return ret;
}
