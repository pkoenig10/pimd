#include <stdio.h>
#include <algorithm>
#include <time.h>

#include "bandpi.h"
#include  "arm_neon.h"
// return GB/s
static float
toBW(int bytes, float sec) {
    return static_cast<float>(bytes) / (1024. * 1024. * 1024.) / sec;
}

static float
toGFLOPS(int ops, float sec) {
    return static_cast<float>(ops) / 1e9 / sec;
}



int main() {
    for(int i = 1; i <5; i++){  
        const unsigned int N = i * 40 * 1000 * 1000; // 20 M element vectors (~80 MB)
        const unsigned int TOTAL_BYTES = 2 * N * sizeof(float);
        const unsigned int TOTAL_FLOPS = 2 * N;
        float scale = 2.f;
        float* arrayX = new float[N];
        float* result = new float[N];


        // initialize array values
        for (unsigned int i=0; i<N; i++)
        {
            arrayX[i] = i;
            result[i] = 0.f;
        }

        //
        // Run the serial implementation. Repeat three times for robust
        // timing.
        //
        double minSerial = 1e30;
        for (int i = 0; i < 3; ++i) {
            clock_t startTime = clock();
            for (int i = 0; i<N; i++){
                result[i] = arrayX[i]; 
            }
            clock_t endTime = clock();
            minSerial = std::min(minSerial,(double)(endTime-startTime)/CLOCKS_PER_SEC);
        }

        printf("[serial]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
               minSerial * 1000,
               toBW(TOTAL_BYTES, minSerial),
               toGFLOPS(TOTAL_FLOPS, minSerial));

        // Clear out the buffer
        for (unsigned int i = 0; i < N; ++i)
            result[i] = 0.f;

        //
        // Run the ISPC (single core) implementation
        //
        double minNEON = 1e30;
        for (int i = 0; i < 3; ++i) {
            clock_t startTime = clock();
            for (int i = 0; i<N; i +=4){
                float32x4_t x = vld1q_f32(arrayX+i); 
                vst1q_f32(result+i,x);
            }
            clock_t endTime = clock();
            minNEON = std::min(minNEON,(double)(endTime-startTime)/CLOCKS_PER_SEC);
        }

        printf("[NEON]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
               minNEON* 1000,
               toBW(TOTAL_BYTES, minNEON),
               toGFLOPS(TOTAL_FLOPS, minNEON));

        // Clear out the buffer
        for (unsigned int i = 0; i < N; ++i)
            result[i] = 0.f;

        double minPI = 1e30;
        for (int i = 0; i < 3; ++i) {
            double time = band_pi(N, arrayX, result);
            minPI = std::min(minPI,(double)(time)/CLOCKS_PER_SEC);
        }
        printf("[PI]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
               minPI* 1000,
               toBW(TOTAL_BYTES, minPI),
               toGFLOPS(TOTAL_FLOPS, minPI));


        printf("\t\t\t\tNUMBER OF ELEMENTS = %d\n", N);
        printf("\t\t\t\t(%.2fx speedup from NEON)\n", minSerial/minNEON);
        printf("\t\t\t\t(%.2fx speedup from PIGPU)\n", minSerial/minPI);
        printf("\t\t\t\t(%.2fx speedup from NEON to PIGPU)\n", minNEON/minPI);
        printf("\n\n\n");
        delete[] arrayX;
        delete[] result;
    }

    return 0;
}
