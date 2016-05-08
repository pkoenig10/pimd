#include <stdio.h>
#include <algorithm>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "blurpi.h"
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
   Image * input = readPPMImage("test.ppm");
   Image * output = new Image(input->width, input->height);  
   //Run for each color yo 
   pi_blur(input->width, input->height, input->rd,output->rd); 
   pi_blur(input->width, input->height, input->gd,output->gd); 
   pi_blur(input->width, input->height, input->bd,output->bd); 
   writePPMImage(output,"outtest.ppm");  
   return 0;
}
