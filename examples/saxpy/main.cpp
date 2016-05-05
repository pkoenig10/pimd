#include <stdio.h>
#include <algorithm>
#include <time.h>

#include "saxpy.hpp"
#include "saxpySerial.hpp"
#include "saxpypi.hpp"


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

    const unsigned int N = 20 * 1000; //* 1000; // 20 M element vectors (~80 MB)
    const unsigned int TOTAL_BYTES = 4 * N * sizeof(float);
    const unsigned int TOTAL_FLOPS = 2 * N;
    float scale = 2.f;

    float* arrayX = new float[N];
    float* arrayY = new float[N];
    float* result = new float[N];

    // initialize array values
    for (unsigned int i=0; i<N; i++)
    {
        arrayX[i] = i;
        arrayY[i] = i;
        result[i] = 0.f;
    }

    //
    // Run the serial implementation. Repeat three times for robust
    // timing.
    //
    double minSerial = 1e30;
    for (int i = 0; i < 3; ++i) {
        clock_t startTime = clock();
        saxpySerial(N, scale, arrayX, arrayY, result);
        clock_t endTime = clock();
        minSerial = std::min(minSerial,(double)(endTime-startTime)/CLOCKS_PER_SEC);
    }

    printf("[saxpy serial]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
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
        saxpy_NEON(N, scale, arrayX, arrayY, result);
        clock_t endTime = clock();
        minNEON = std::min(minNEON,(double)(endTime-startTime)/CLOCKS_PER_SEC);
    }

    printf("[saxpy NEON]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
           minNEON* 1000,
           toBW(TOTAL_BYTES, minNEON),
           toGFLOPS(TOTAL_FLOPS, minNEON));

    // Clear out the buffer
    for (unsigned int i = 0; i < N; ++i)
        result[i] = 0.f;

    double minPI = 1e30;
    for (int i = 0; i < 3; ++i) {
        clock_t startTime = clock();
        saxpy_pi(N, scale, arrayX, arrayY, result);
        clock_t endTime = clock();
        minNEON = std::min(minPI,(double)(endTime-startTime)/CLOCKS_PER_SEC);
    }
    printf("[saxpy PI]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
           minPI* 1000,
           toBW(TOTAL_BYTES, minPI),
           toGFLOPS(TOTAL_FLOPS, minPI));



    printf("\t\t\t\t(%.2fx speedup from NEON)\n", minSerial/minNEON);
    printf("\t\t\t\t(%.2fx speedup from PIGPU)\n", minSerial/minPI);
    printf("\t\t\t\t(%.2fx speedup from NEON to PIGPU)\n", minNEON/minPI);

    delete[] arrayX;
    delete[] arrayY;
    delete[] result;

    return 0;
}
