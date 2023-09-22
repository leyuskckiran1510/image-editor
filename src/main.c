#include "image-editor.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/*
  [Wiki ] https://en.wikipedia.org/wiki/JPEG

*/
void binary(unsigned short int x, char *final) {
  int p = 0;
  do {
    final[p++] = x & 1 ? '1' : '0';
  } while (x >>= 1);
  final[p] = '\0';
}

void jpeg_display(JPEG j) {
  printf("%s %hu \n", "soi", j.soi);
  printf("%s %hu \n", "marker", j.marker);
  printf("%s %hu \n", "length", j.length);
  printf("%s %s \n", "identifier", j.identifier);
  printf("%s %hu \n", "version", j.version);
  printf("%s %hu \n", "density_unit", j.density_unit);
  printf("%s %hu \n", "xdensity", j.xdensity);
  printf("%s %hu \n", "ydensity", j.ydensity);
  printf("%s %hu \n", "xthumbnail", j.xthumbnail);
  printf("%s %hu \n", "ythumbnail", j.ythumbnail);

  printf("------QUANTIZATION TABLE------\n");
  printf("%s %hu \n", "length", j.quantizetables.length);
  printf("%s %hu \n", "marker", j.quantizetables.marker);
  printf("%s %hu \n", "precession", j.quantizetables.precision);
  printf("%s %hu \n", "table count", j.quantizetables.table_count);

  printf("------Start of Frame-0 Segment (SOF-0)------\n");
  printf("%s %hu \n", "length", j.sof0.length);
  printf("%s %hu \n", "marker", j.sof0.marker);
  printf("%s %hu \n", "precession", j.sof0.precision);
  printf("%s %hu \n", "width", j.sof0.width);
  printf("%s %hu \n", "height", j.sof0.height);
  printf("%s %hu \n", "total component", j.sof0.total_component);
}

void print(uint x) { printf("%s %hu \n", "UINT", x); }

void exapp0_cast(FILE *fp, EXAPP0 *exapp0) {
  (void)fp;
  (void)exapp0;
  return;
}

void jpeg_cast(FILE *fp, JPEG *j) {
  int dummy__ = 0, endian_cng = 0;
  dummy__ += fread(&j->soi, 1, 2, fp);
  dummy__ += fread(&j->marker, 1, 2, fp);
  dummy__ += fread(&j->length, 1, 2, fp);
  dummy__ += fread(&j->identifier, 5, 1, fp);
  dummy__ += fread(&j->version, 1, 2, fp);
  dummy__ += fread(&j->density_unit, 1, 1, fp);
  dummy__ += fread(&j->xdensity, 2, 1, fp);
  dummy__ += fread(&j->ydensity, 2, 1, fp);
  dummy__ += fread(&j->xthumbnail, 1, 1, fp);
  dummy__ += fread(&j->ythumbnail, 1, 1, fp);
  if (j->soi != 0xffd8) {
    /*
      endian converison
      expected 0xffd8 0xffe0
      but  got 0xd8ff 0xe0ff
      so converting it
    */
    endian_cng = 1;
    j->soi = __builtin_bswap16(j->soi);
    j->marker = __builtin_bswap16(j->marker);
    j->length = __builtin_bswap16(j->length);
    j->version = __builtin_bswap16(j->version);
    j->xdensity = __builtin_bswap16(j->xdensity);
    j->ydensity = __builtin_bswap16(j->ydensity);
  }
  if (j->length > 16) {
    log("Reached unreachable");
    dummy__ +=
        fread(j->thumnaildata, sizeof(RGB), j->xthumbnail * j->ythumbnail, fp);
  }
  if (j->version >= 0x0102) {
    exapp0_cast(fp, &j->exapp0);
  }

  uint16_t next_flag;

  dummy__ += fread(&next_flag, 2, 1, fp);
  switch (next_flag) {
  case 0xfffe:
  case 0xfeff: {
    dummy__ += fread(&j->comment.length, 2, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->comment.length = __builtin_bswap16(j->comment.length);
    }
    j->comment.data = malloc(sizeof(uint8_t) * j->comment.length);
    dummy__ += fread(j->comment.data, sizeof(uint8_t), j->comment.length, fp);
    j->comment.marker = next_flag;
    dummy__ += fread(&next_flag, 2, 1, fp); // reading next flag
    break;
  }
  }

  // print(next_flag);
  switch (next_flag) {
  case 0xffdb:
  case 0xdbff: {
    // dummy__ = 0;
    dummy__ += fread(&j->quantizetables.length, 2, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->quantizetables.length = __builtin_bswap16(j->quantizetables.length);
    }
    j->quantizetables.marker = next_flag;
    j->quantizetables.table_count = 1;
    j->quantizetables.table = malloc(64 * sizeof(uint8_t));
    int total_read = 0;
    for (int _table_c = 0; total_read < j->quantizetables.length - 2;
         _table_c++) {
      j->quantizetables.table =
          realloc(j->quantizetables.table, (_table_c + 1) * 64);
      total_read += fread(&j->quantizetables.precision, 1, 1, fp);
      total_read += fread(&j->quantizetables.table[_table_c * 64], 1, 64, fp);
      j->quantizetables.table_count += 1;
    }
    dummy__ += total_read;
    dummy__ += fread(&next_flag, 2, 1, fp);
    break;
  }
  }

  switch (next_flag) {
  case 0xffc0:
  case 0xc0ff: {
    dummy__ += fread(&j->start_of_frame0.length, 2, 1, fp);
    dummy__ += fread(&j->start_of_frame0.precision, 1, 1, fp);
    dummy__ += fread(&j->start_of_frame0.height, 2, 1, fp);
    dummy__ += fread(&j->start_of_frame0.width, 2, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->start_of_frame0.length = __builtin_bswap16(j->start_of_frame0.length);
      j->start_of_frame0.height = __builtin_bswap16(j->start_of_frame0.height);
      j->start_of_frame0.width = __builtin_bswap16(j->start_of_frame0.width);
    }
    j->sof0.marker = next_flag;
    dummy__ += fread(&j->sof0.total_component, 1, 1, fp);
    j->sof0.sofc = malloc(sizeof(*j->sof0.sofc) * (j->sof0.total_component));
    for (int i = 0; i < j->sof0.total_component; i++) {
      dummy__ += fread(&j->sof0.sofc[i], 3, 1, fp);
    }


    //next flag
    dummy__ += fread(&next_flag, 2, 1, fp);
    break;
  }
  }
  
  log("NEXT FLAGGG");
  log("Current DUMP location is %d", dummy__);
  print(next_flag);

  switch (next_flag) {
    case 0xddff:
    case 0xffdd:{
      dummy__ += fread(&j->defineRestartInterval.length, 2, 1, fp);
      dummy__ += fread(&j->defineRestartInterval.interval, 2, 1, fp);
      if (endian_cng) {
        next_flag = __builtin_bswap16(next_flag);
        j->defineRestartInterval.length = __builtin_bswap16(j->defineRestartInterval.length);
        j->defineRestartInterval.interval = __builtin_bswap16(j->defineRestartInterval.interval);
      }
      j->defineRestartInterval.marker = next_flag;
      //next flag

      dummy__ += fread(&next_flag, 2, 1, fp);
      // dummy__ += fread(&next_flag, 2, 1, fp);
      break;
      break;

    }
  
  }
  
   // 0xddff
      /*
      0xddff
      0x400
      0x6400
      0xff64
      */

  log("Starting...");
  log("Current DUMP location is %d", dummy__);
  print(next_flag);
  // for (int p=0;p<10;p++) {
  //   dummy__ += fread(&next_flag, 2, 1, fp);
  //   print(next_flag);
    switch (next_flag) {
    case 0xffc4:
    case 0xc4ff: {
      break;
    }
    case 65497:
    case 0x99fd:{
      break;
    }
    // default:
      // continue;
    }
    // break;
  // }
  return;
  /*0xffd9*/
  /*
    0x0102 -> version >= 01.02
  */

  (void)dummy__;
}

void load_image(const char *file_name) {
  FILE *f = fopen(file_name, "rb");
  if (f == NULL) {
    log("Failed To Open the Image File %s", file_name);
    exit(1);
  }
  JPEG *temp = malloc(sizeof(JPEG));
  jpeg_cast(f, temp);

  jpeg_display(*temp);
  free(temp);
}

int main(void) {

  load_image("./src/test.jpg");
  return 0;
}