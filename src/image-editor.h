#ifndef __IMAGE_EDITOR__
#define __IMAGE_EDITOR__ 0.11

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define log(formate, ...)                                                      \
  printf("[ "__FILE__                                                          \
         ":%d ](message)\t\t" formate "",                                      \
         __LINE__, ##__VA_ARGS__);

#define logn(formate, ...)                                                      \
  printf("[ "__FILE__                                                          \
         ":%d ](message)\t\t" formate "\n",                                      \
         __LINE__, ##__VA_ARGS__);
#define print(x) printf("%s %hu \n", "UINT", x)

typedef struct {
  union {
    uint8_t r;
    uint8_t red;
  };
  union {
    uint8_t g;
    uint8_t green;
  };
  union {
    uint8_t b;
    uint8_t blue;
  };
  union {
    uint8_t a;
    uint8_t alpha;
  };
} RGBa;

typedef struct {
  union {
    uint8_t r;
    uint8_t red;
  };
  union {
    uint8_t g;
    uint8_t green;
  };
  union {
    uint8_t b;
    uint8_t blue;
  };
} RGB;

typedef struct {
  size_t width, height;
  RGBa *pixels;
} PixelArray;

typedef union{
  struct{
    uint8_t width;
    uint8_t height;
    RGB palette[32];
    uint8_t *pixels;// size width*height
  }one_byte;
  struct{
    uint8_t width;
    uint8_t height;
    RGB *pixels;// size width*height
  }three_byte;

}ThumnailStore;

typedef struct{
  uint16_t lp;
  char identifier[5];
  uint8_t ext_code;
  ThumnailStore thumnailstore;
}EXAPP0;


typedef struct{
  uint16_t marker;
  uint16_t length;
  uint8_t *data;
}COM;

typedef struct{
  uint16_t marker;
  uint16_t length;
  uint8_t precision;
  uint8_t table_count;
  uint8_t *table;
}DQT;

typedef struct{
    uint8_t component_id;
    uint8_t samplingfactor;
    uint8_t dqt_table;
} SOF_component;

typedef struct{
  uint16_t marker;
  uint16_t length;
  uint8_t precision;
  union{
  uint16_t height;
  uint16_t image_height;
  };
  union{
  uint16_t width;
  uint16_t image_width;
  };
  union{
    uint8_t component_count;
    uint8_t total_component;
  };
  SOF_component *sofc;
}SOF;


typedef struct{
  uint16_t marker;
  uint16_t length;
  uint16_t interval;
} DRI;

typedef struct{
  uint16_t marker;
  uint16_t length;
  uint8_t tableinfo;
  uint8_t symbolcount[16];
  uint8_t *symbols;
}DHT;

typedef struct{
  uint16_t marker;
  uint16_t length;
  union{
  uint8_t component_count;
  uint8_t total_component;
  };
  uint8_t *componentdata;
  uint8_t padding[3];
} SOS;

typedef struct{
  uint16_t soi;
  uint16_t marker;
  uint16_t length;
  char identifier[5];
  uint16_t version;
  uint8_t density_unit;
  uint16_t xdensity;
  uint16_t ydensity;
  uint8_t xthumbnail;
  uint8_t ythumbnail;
  RGB *thumnaildata;
  EXAPP0 exapp0;
  COM comment;
  DQT quantizetables;
  union{
  SOF startofframe0;
  SOF sof0;
  };

  DRI defineRestartInterval;
  DHT huffmantable;
  SOS startofscan;
  union{
  uint8_t *compresseddata;
  uint8_t *compressed_data;
  uint8_t *data;
};
  uint16_t eof;

} JPEG;


#endif