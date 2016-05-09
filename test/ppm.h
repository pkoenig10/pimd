#ifndef __PPM_H__
#define __PPM_H__

struct Image;

void writePPMImage(const Image* image, const char *filename);
Image * readPPMImage(const char *filename);

#endif

