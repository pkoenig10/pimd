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

    void clear(float r, float g, float b, float a) {
    
            int numPixels = width * height;
            float* ptr = data;
            for (int i=0; i<numPixels; i++) {
                        rd[i] = r;
                        gd[i] = g;
                        bd[i] = b;
                    }
        }

    int width;
    int height;
    float* rd;
    float* gd;
    float* bd;
};


#endif
