#include <algorithm>
#include <cstdio>
#include <ctime>
#include <getopt.h>

#include <arm_neon.h>
#include <pimd.h>

#include "pimd_test.h"


static void usage(const char* progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Program Options:\n");
    printf("  -n  --size <INT>    Number of elements in arrays\n");
    printf("  -r  --repeat <INT>  Number of times to repeat each test\n");
    printf("  -?  --help          This message\n");
}


void saxpySerial(int N, float scale, float X[], float Y[], float result[]) {
    for (int i=0; i<N; i++) {
        result[i] = scale * X[i] + Y[i];
    }
}

void saxpyNeon(int N, float scale, float X[], float Y[], float result[]) {
    float32x4_t scaleVec = vdupq_n_f32(scale);
    float32x4_t Xvec,Yvec,resultVec;
    for (int i=0; i<N; i+=4){
        Xvec = vld1q_f32(X+i);
        Yvec = vld1q_f32((Y+i));
        Xvec = vmulq_f32(Xvec,scaleVec);
        resultVec = vaddq_f32(Xvec,Yvec);  
        vst1q_f32(result+i, resultVec); 
    }
}

int saxpyPimd(int N, float scale, float *X, float *Y, float *result, float *runtime) {
    int mb = pimd_open();

    std::vector<PimdOp> ops; std::vector<PimdArg> args;
    ops.push_back(OP_VLOAD); args.push_back(X);
    ops.push_back(OP_SFMUL); args.push_back(scale);
    ops.push_back(OP_VFADD); args.push_back(Y);
    ops.push_back(OP_STORE); args.push_back(result);

    PimdFunction saxpy = PimdFunction(mb,&ops[0], ops.size());

    int ret = saxpy.call(&args[0], args.size(), N, 10000, runtime);

    saxpy.free();

    pimd_close(mb);

    return ret;
}


int main(int argc, char** argv) {
    int NUM_REPEAT = 3;
    int N = 20 * 1000 * 1000; // 20 M element vectors (~80 MB)

    //
    // Parse command line options
    //
    int opt;
    static struct option long_options[] = {
        {"size", 1, 0, 'n'},
        {"repeat", 2, 0, 'r'},
        {"help", 0, 0, '?'},
        {0 ,0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "?n:", long_options, nullptr)) != EOF) {
        switch (opt) {
        case 'n':
            N = atoi(optarg);
            break;
        case 'r':
            NUM_REPEAT = atoi(optarg);
            break;
        case '?':
        default:
            usage(argv[0]);
            return 1;
        }
    }

    unsigned TOTAL_BYTES = 4 * N * sizeof(float);
    unsigned TOTAL_FLOPS = 2 * N;

    float scale = 2.f;
    float *X = new float[N];
    float *Y = new float[N];
    float *result = new float[N];
    for (int i=0; i<N; i++) {
        X[i] = i;
        Y[i] = i;
        result[i] = 0.f;
    }

    //
    // Run the serial implementation
    //
    float minSerial = 1e30;
    for (int i=0; i<NUM_REPEAT; i++) {
        clock_t startTime = clock();
        saxpySerial(N, scale, X, Y, result);
        clock_t endTime = clock();
        minSerial = std::min(minSerial, ((float)(endTime - startTime))/CLOCKS_PER_SEC);
    }

    printf("[saxpy Serial]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
            minSerial * 1000,
            toBW(TOTAL_BYTES, minSerial),
            toGFLOPS(TOTAL_FLOPS, minSerial));

    //
    // Run the Neon implementation
    //
    float minNeon = 1e30;
    for (int i=0; i<NUM_REPEAT; i++) {
        clock_t startTime = clock();
        saxpyNeon(N, scale, X, Y, result);
        clock_t endTime = clock();
        minNeon = std::min(minNeon, ((float)(endTime - startTime))/CLOCKS_PER_SEC);
    }

    printf("[saxpy NEON]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
            minNeon * 1000,
            toBW(TOTAL_BYTES, minNeon),
            toGFLOPS(TOTAL_FLOPS, minNeon));

    //
    // Run the PiMD implementation
    //
    float minPimd = 1e30;
    for (int i=0; i<NUM_REPEAT; i++) {
        float runtime;
        int ret = saxpyPimd(N, scale, X, Y, result, &runtime);
        if (ret) {
            printf("[saxpy PiMD]:\t\tFAILED with error code %d on iteration %d\n", ret, i);
            return ret;
        }
        minPimd = std::min(minPimd, runtime);
    }

    printf("[saxpy PiMD]:\t\t[%.3f] ms\t[%.3f] GB/s\t[%.3f] GFLOPS\n",
            minPimd * 1000,
            toBW(TOTAL_BYTES, minPimd),
            toGFLOPS(TOTAL_FLOPS, minPimd));

    printf("\t\t\t\t(%.2fx speedup from NEON)\n", minSerial / minNeon);
    printf("\t\t\t\t(%.2fx speedup from PiMD)\n", minSerial / minPimd);
    printf("\t\t\t\t(%.2fx speedup from NEON to PiMD)\n", minNeon / minPimd);

    delete[] X;
    delete[] Y;
    delete[] result;

    return 0;
}
