#ifndef __UTIL_H_
#define __UTIL_H_

inline float * addZeroes(int m, int n, float* input){
    float* newArr = new float[(m*n+m*2+ (n+2)*2)]; 
    std::memset(newArr, 0, sizeof(float)*(n+2));
    for (int i = 1 ; i< m; i++){
        float* curr_ptr = newArr+(i*n+2);
        curr_ptr[0] = 0;
        std::memcpy(curr_ptr + 1,(input+(i-1)*n), sizeof(float)*n); 
        curr_ptr[n+1] = 0; 
    }  
    std::memset((newArr + (m+1)*(n+2)), 0, n+2);
    return newArr;
}

inline float * remZeroes(int m, int n, float* input){
    float* newArr = new float[(m-2)*(n-2)];

    for (int i = 0; i < (m-2); i++){
        float* ptr = (input + n*(i+1) + 1);
        std::memcpy(newArr, ptr, sizeof(float)*(n-2));
    }
    delete[] input;
    return newArr;
}


#endif
