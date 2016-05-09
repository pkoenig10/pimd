#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <time.h>
#include "image.h"
#include "pimd.h"
#include "pimd_test.h"
#include "util.h"

#define CLAMP(x,minimum,maximum) std::max(minimum,std::min(x,maximum))    
void
writePPMImage(const Image* image, const char *filename)
{
    FILE *fp = fopen(filename, "wb");

    if (!fp) {
            fprintf(stderr, "Error: could not open %s for write\n", filename);
            exit(1);
        }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", image->width, image->height);
    fprintf(fp, "255\n");

    for (int j=image->height-1; j>=0; j--) {
            for (int i=0; i<image->width; i++) {
            
                        const float* r = &image->rd[(j*image->width + i)];
                        const float* g = &image->gd[(j*image->width + i)];
                        const float* b = &image->bd[(j*image->width + i)];
            
                        char val[3];
                        val[0] = static_cast<char>(255.f * CLAMP(*r, 0.f, 1.f));
                        val[1] = static_cast<char>(255.f * CLAMP(*g, 0.f, 1.f));
                        val[2] = static_cast<char>(255.f * CLAMP(*b, 0.f, 1.f));
            
                        fputc(val[0], fp);
                        fputc(val[1], fp);
                        fputc(val[2], fp);
                    }
        }

    fclose(fp);
    printf("Wrote image file %s\n", filename);
}
Image * readPPMImage(const char*filename){
    FILE *fp = fopen(filename, "r");
    if (!fp) {
            fprintf(stderr, "Error: could not open %s for read\n", filename);
            exit(1);
        }
    int w,h; 
    fscanf(fp, "P6\n");
    fscanf(fp, "%d %d\n", &w,&h);
    fscanf(fp, "255\n");
    Image* img = new Image(w,h);
    for(int i = 0; i< w*h; i++){
         img->rd[i] = static_cast<float>(fgetc(fp))/255.f;
         img->gd[i] = static_cast<float>(fgetc(fp))/255.f;
         img->bd[i] = static_cast<float>(fgetc(fp))/255.f;
    }
    return img;
}

void pi_blur(int m, int n, float * input, float* output){
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
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(b); 
    ops.push_back(OP_SFMUL); args.push_back(scales[1]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(c); 
    ops.push_back(OP_SFMUL); args.push_back(scales[2]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(d); 
    ops.push_back(OP_SFMUL); args.push_back(scales[3]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(e); 
    ops.push_back(OP_SFMUL); args.push_back(scales[4]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(f); 
    ops.push_back(OP_SFMUL); args.push_back(scales[5]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(g); 
    ops.push_back(OP_SFMUL); args.push_back(scales[6]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(h); 
    ops.push_back(OP_SFMUL); args.push_back(scales[7]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_SAVE); args.push_back(VAR_0); 
    ops.push_back(OP_VLOAD); args.push_back(i); 
    ops.push_back(OP_SFMUL); args.push_back(scales[8]); 
    ops.push_back(OP_VFADD); args.push_back(VAR_0);
    ops.push_back(OP_STORE); args.push_back(result);

    PimdFunction blur = PimdFunction(mb,&ops[0],ops.size()); 
    int ret = blur.call(&args[0], args.size(), m*n - 2*n - 2, 10000);
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

float mask_val(float* pixels,int r,int c,int width,int height,float *mask)
{
    int mskwth = 3;
    int row = r;
    int col = c;
    float ret = 0.f;
    int rem = (mskwth-1)/2;
    for(int r=0; r <mskwth;r++)
    {
        for(int c =0;c<mskwth;c++)
        {
            if(row-rem + r >0 &&row-rem + r <height&& c-rem +col>0 && c-rem +col<width)  
                {
                    float p = pixels[(row -rem) *width + col + c - rem];
                    ret+=p*mask[r*mskwth + c];
                }
        }
    }
    return ret;
}



int main() {
    Image * input = readPPMImage("test.ppm");
    Image * output = new Image(input->width, input->height);  
    int height = input-> height;
    int width= input-> width;
    float scales[] = {.0586f,.0952f,.0586f,
                      .0952f, .1501f,.0952f,
                      .0586f,.0952f,.0586f};
    double sstart = clock();
    for(int r=0;r<height;r++)
    {
        for(int c=0;c<width;c++)
        {
            output->rd[r*width + c]=mask_val(input->rd,r,c,width,height,scales);
        }
    }
    for(int r=0;r<height;r++)
    {
        for(int c=0;c<width;c++)
        {
            output->gd[r*width+c]=mask_val(input->gd,r,c,width,height,scales);
        }
    }
    for(int r=0;r<height;r++)
    {
        for(int c=0;c<width;c++)
        {
            output->bd[r*width+c]=mask_val(input->bd,r,c,width,height,scales);
        }
    }

    double send = clock();
    double minSeq = (float)((send-sstart)/CLOCKS_PER_SEC);
    printf("[Image Seq]:\t\t[%.3f] ms\t\n",
            minSeq* 1000);
    writePPMImage(output,"outtest.ppm");  
    input = readPPMImage("test.ppm");
    //Run for each color yo 
    double start = clock();
    float* R = addZeroes(input->height,input->width,input->rd);
    float* G = addZeroes(input->height,input->width,input->gd);
    float* B = addZeroes(input->height,input->width,input->bd);
    
    output = new Image(input->width, input->height);  
        
    pi_blur(input->width+2, input->height+2, R,output->rd); 
    pi_blur(input->width+2, input->height+2, G,output->gd); 
    pi_blur(input->width+2, input->height+2, B,output->bd); 

    output->rd = remZeroes(input->width+2,input->height+2,output->rd);
    output->gd = remZeroes(input->width+2,input->height+2,output->gd);
    output->bd = remZeroes(input->width+2,input->height+2,output->bd);
    double end = clock();
    double minPi = (float)((end-start)/CLOCKS_PER_SEC);
    printf("[saxpy PI]:\t\t[%.3f] ms\t\n",
            minPi * 1000);

    output->height = input->height; 
    output->width = input->width; 
    writePPMImage(output,"outtest.ppm");  
   return 0;
}
