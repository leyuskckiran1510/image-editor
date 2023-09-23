#include "image-editor.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "./jpeg/jpeg.h"
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





void exapp0_cast(FILE *fp, EXAPP0 *exapp0) {
  (void)fp;
  (void)exapp0;
  return;
}

void jpeg_cast1(FILE *fp, JPEG *j) {
  int dummy__ = 0, endian_cng = 0;

  // APP0 and headers
  dummy__ += fread(&j->soi,2, 1, fp);
  dummy__ += fread(&j->marker, 2, 1, fp);
  dummy__ += fread(&j->length, 2, 1, fp);
  dummy__ += fread(&j->identifier, 5, 1, fp);
  dummy__ += fread(&j->version, 2, 1, fp);
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

  // thumnail from app0
  if (j->length > 16) {
    logn("Reached unreachable");
    logn("Not IMplemented the thumnail part...");
    exit(0);
    // dummy__ +=
    //     fread(j->thumnaildata, sizeof(RGB), j->xthumbnail * j->ythumbnail, fp);
  }
  if (j->version >= 0x0102) {
    exapp0_cast(fp, &j->exapp0);
  }

  // COMMENT Section Parsing
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

  // Quantize Table Parsing
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

  // Start OF Frame 0 Parsing
  switch (next_flag) {
  case 0xffc0:
  case 0xc0ff: {
    dummy__ += fread(&j->startofframe0.length, 2, 1, fp);
    dummy__ += fread(&j->startofframe0.precision, 1, 1, fp);
    dummy__ += fread(&j->startofframe0.height, 2, 1, fp);
    dummy__ += fread(&j->startofframe0.width, 2, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->startofframe0.length = __builtin_bswap16(j->startofframe0.length);
      j->startofframe0.height = __builtin_bswap16(j->startofframe0.height);
      j->startofframe0.width = __builtin_bswap16(j->startofframe0.width);
    }
    j->sof0.marker = next_flag;
    dummy__ += fread(&j->sof0.total_component, 1, 1, fp);
    j->sof0.sofc = malloc(sizeof(*j->sof0.sofc) * (j->sof0.total_component));
    for (int i = 0; i < j->sof0.total_component; i++) {
      dummy__ += fread(&j->sof0.sofc[i], 3, 1, fp);
    }

    // next flag
    dummy__ += fread(&next_flag, 2, 1, fp);
    break;
  }
  }

  // Define Restart Interval Parsing
  switch (next_flag) {
  case 0xddff:
  case 0xffdd: {
    dummy__ += fread(&j->defineRestartInterval.length, 2, 1, fp);
    dummy__ += fread(&j->defineRestartInterval.interval, 2, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->defineRestartInterval.length =
          __builtin_bswap16(j->defineRestartInterval.length);
      j->defineRestartInterval.interval =
          __builtin_bswap16(j->defineRestartInterval.interval);
    }
    j->defineRestartInterval.marker = next_flag;

    // next flag
    dummy__ += fread(&next_flag, 2, 1, fp);
    break;
  }
  }

  // Huffman Table Parsing
  switch (next_flag) {
  case 0xffc4:
  case 0xc4ff: {
    dummy__ += fread(&j->huffmantable.length, 2, 1, fp);
    dummy__ += fread(&j->huffmantable.tableinfo, 1, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->huffmantable.length = __builtin_bswap16(j->huffmantable.length);
    }
    j->huffmantable.marker = next_flag;
    dummy__ += fread(j->huffmantable.symbolcount, 16, 1, fp);
    int symbols_size =
        j->huffmantable.length -
        (sizeof(j->huffmantable.symbolcount) + sizeof(j->huffmantable.length) +
         sizeof(j->huffmantable.tableinfo));
    j->huffmantable.symbols = malloc(symbols_size);
    dummy__ += fread(j->huffmantable.symbols, symbols_size, 1, fp);

    // next flag
    dummy__ += fread(&next_flag, 2, 1, fp);
    break;
  }
  }

  // Start of Scan Segment (SOS)
  switch (next_flag) {
  case 0xdaff:
  case 0xffda: {
    dummy__ += fread(&j->startofscan.length, 2, 1, fp);
    dummy__ += fread(&j->startofscan.component_count, 1, 1, fp);
    if (endian_cng) {
      next_flag = __builtin_bswap16(next_flag);
      j->startofscan.length = __builtin_bswap16(j->startofscan.length);
    }
    j->startofscan.marker = next_flag;
    j->startofscan.componentdata = malloc(2 * j->startofscan.component_count);
    dummy__ += fread(j->startofscan.componentdata,
                     2 * j->startofscan.component_count, 1, fp);
    dummy__ += fread(j->startofscan.padding, 3, 1, fp);
    break;
  }
  }

  long pointer_backup = ftell(fp);
  fseek(fp, 0, SEEK_END);
  long remaning_bytes = ftell(fp) - pointer_backup;
  fseek(fp, pointer_backup, SEEK_SET);
  j->compresseddata = malloc(remaning_bytes - 2);
  dummy__ += fread(j->compresseddata, remaning_bytes - 2, 1, fp);
  dummy__ += fread(&next_flag, 2, 1, fp);

  if (endian_cng) {
    next_flag = __builtin_bswap16(next_flag);
  }
  j->eof = next_flag;
}




int main(void) {

  JPEG *image = jpeg_load_image("./Images/sample.jpg");
  jpeg_display(*image);
  jpeg_free(image);
  return 0;
}