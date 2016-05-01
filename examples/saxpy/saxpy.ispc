
export void saxpy_NEON(int N,
                       float scale,
                            float X[],
                            float Y[],
                            float result[])
{
    float32x4_t scaleVec = vdup_n_f32(scale);
    float32x4_t Xvec,Yvec,resultVec;
    for (int i = 0; i < N; i+=4){
        Xvec = vld1q_lane_f32((X+i)); 
        Yvec = vld1_f32((Y+i));
                 
        
    }
}

