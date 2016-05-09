#ifndef  __IMAGE_H__
#define  __IMAGE_H__


struct Image {

    Image(int w, int h) {
            width = w;
            height = h;
            rd = new float[width * height];
            gd = new float[width * height];
            bd = new float[width * height];
        }


    int width;
    int height;
    float* rd;
    float* gd;
    float* bd;
};


#endif
