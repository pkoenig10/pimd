#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "image.h"
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

