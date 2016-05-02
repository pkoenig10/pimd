#include "saxpy.hpp"
#include "arm_neon.h"

void saxpy_NEON(int N,
                       float scale,
                            float X[],
                            float Y[],
                            float result[])
{
    float32x4_t scaleVec = vdupq_n_f32(scale);
    float32x4_t Xvec,Yvec,resultVec;
    for (int i = 0; i < N; i+=8){
        Xvec = vld1q_f32(X+i);
        Yvec = vld1q_f32((Y+i));
        Xvec = vmulq_f32(Xvec,scaleVec);
        resultVec = vaddq_f32(Xvec,Yvec);  
        vld1q_f32(result+i, resultVec); 
    }
}

