#ifndef __IMAGE_EDITOR__
#define __IMAGE_EDITOR__ 0.11

#include <stdio.h>
#include <stdlib.h>

#define log(formate, ...)                                                      \
  printf("[ "__FILE__                                                          \
         ":%d ](message)\t" formate "\n",                                      \
         __LINE__, ##__VA_ARGS__);

typedef struct {
  union {
    unsigned int r;
    unsigned int red;
  };
  union {
    unsigned int g;
    unsigned int green;
  };
  union {
    unsigned int b;
    unsigned int blue;
  };
  union {
    unsigned int a;
    unsigned int alpha;
  };
} RGBa;

typedef struct {
  size_t width, height;
  RGBa *pixels;
} PixelArray;

#define U(x,name) unsigned char name[x]

typedef unsigned short int u2;


typedef struct{
  short soi;
  short marker;
  short length;
  U(5,identifier);
  short version;
  unsigned char density_unit;
  short xdensity;
  short ydensity;
  unsigned char xthumbnail;
  unsigned char ythumbnail;
} JPEG;


#endif