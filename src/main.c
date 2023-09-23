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
  log("------Start Of JPEG IMAGE-------\n");
  logn("%s %hu \n", "soi", j.soi);

  log("------JPEG/JFIF Image Segment ( APP-0 )------\n");
  log("%s %hu \n", "marker", j.marker);
  log("%s %hu \n", "length", j.length);
  log("%s %s \n", "identifier", j.identifier);
  log("%s %hu \n", "version", j.version);
  log("%s %hu \n", "density_unit", j.density_unit);
  log("%s %hu \n", "xdensity", j.xdensity);
  log("%s %hu \n", "ydensity", j.ydensity);
  log("%s %hu \n", "xthumbnail", j.xthumbnail);
  logn("%s %hu \n", "ythumbnail", j.ythumbnail);

  log("------Comment Segment ( COM )------\n");
  log("%s %hu \n", "length", j.comment.length);
  log("%s %hu \n", "marker", j.comment.marker);
  logn("%s %s \n", "precession", j.comment.data);

  log("------Define Quantization Table Segment ( DQT )------\n");
  log("%s %hu \n", "length", j.quantizetables.length);
  log("%s %hu \n", "marker", j.quantizetables.marker);
  log("%s %hu \n", "precession", j.quantizetables.precision);
  logn("%s %hu \n", "table count", j.quantizetables.table_count);

  log("------Start of Frame-0 Segment ( SOF-0 ) ------\n");
  log("%s %hu \n", "length", j.sof0.length);
  log("%s %hu \n", "marker", j.sof0.marker);
  log("%s %hu \n", "precession", j.sof0.precision);
  log("%s %hu \n", "width", j.sof0.width);
  log("%s %hu \n", "height", j.sof0.height);
  logn("%s %hu \n", "total component", j.sof0.total_component);

  log("------Define restart interval ( DRI )------\n");
  log("%s %hu \n", "length", j.defineRestartInterval.length);
  log("%s %hu \n", "marker", j.defineRestartInterval.marker);
  logn("%s %hu \n", "interval", j.defineRestartInterval.interval);

  log("------Define Huffman Table Segment ( DHT )------\n");
  log("%s %hu \n", "length", j.huffmantable.length);
  log("%s %hu \n", "marker", j.huffmantable.marker);
  logn("%s %hu \n", "tableinfo", j.huffmantable.tableinfo);

  log("------Start of Scan Segment ( SOS )------\n");
  log("%s %hu \n", "length", j.startofscan.length);
  log("%s %hu \n", "marker", j.startofscan.marker);
  logn("%s %hu \n", "total Component", j.startofscan.total_component);

  log("------End of Image Segment ( EOI )------\n");
  log("%s %hu \n", "marker", j.eof);
}

void print(uint x) { printf("%s %hu \n", "UINT", x); }

void exapp0_cast(FILE *fp, EXAPP0 *exapp0) {
  (void)fp;
  (void)exapp0;
  return;
}

void jpeg_cast(FILE *fp, JPEG *j) {
  int dummy__ = 0, endian_cng = 0;

  // APP0 and headers
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

  // thumnail from app0
  if (j->length > 16) {
    log("Reached unreachable\n");
    dummy__ +=
        fread(j->thumnaildata, sizeof(RGB), j->xthumbnail * j->ythumbnail, fp);
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

JPEG *jpeg_init() {
  JPEG *j = malloc(sizeof(JPEG));

  // APP0
  j->thumnaildata = NULL;
  // EXTRA APP0 jpeg>=1.2(0x0102)
  j->exapp0.thumnailstore.one_byte.pixels = NULL;
  j->exapp0.thumnailstore.three_byte.pixels = NULL;
  // COMMENT SECTION
  j->comment.marker = 0xfffe;
  j->comment.length = 0;
  j->comment.data = malloc(1);
  j->comment.data[0] = '\0';

  // DEFINE QUANTIZE TABLE
  j->quantizetables.length = 0;
  j->quantizetables.marker = 0xffdb;
  j->quantizetables.table_count = 0;
  j->quantizetables.precision = 0;
  j->quantizetables.table = NULL;

  // START OF FRAME0
  j->startofframe0.length = 0;
  j->startofframe0.marker = 0xffc0;
  j->startofframe0.precision = 0;
  j->startofframe0.height = 0;
  j->startofframe0.width = 0;
  j->sof0.total_component = 0;
  j->sof0.sofc = NULL;

  // DEFINE RESTART INTERVAL
  j->defineRestartInterval.length = 0;
  j->defineRestartInterval.marker = 0xffc0;
  j->defineRestartInterval.interval = 0;

  // DEFINE HUFFMAN TABLE
  j->huffmantable.length = 0;
  j->huffmantable.marker = 0xffc0;
  j->huffmantable.tableinfo = 0;
  j->huffmantable.symbols = NULL;

  // START OF SCAN
  j->startofscan.length = 0;
  j->startofscan.marker = 0xffc0;
  j->startofscan.component_count = 0;
  j->startofscan.componentdata = NULL;

  // ACTUAL COMPRESSED DATA
  j->compresseddata = NULL;
  return j;
}

JPEG *jpeg_load_image(const char *file_name) {
  FILE *f = fopen(file_name, "rb");
  JPEG *temp = jpeg_init();
  if (f == NULL) {
    logn("Failed To Open the Image File %s", file_name);
    exit(1);
  }
  jpeg_cast(f, temp);
  return temp;
}

void jpeg_free(JPEG *image) {

  if (image->thumnaildata != NULL) {
    free(image->thumnaildata);
    image->thumnaildata = NULL;
  }
  if (image->exapp0.thumnailstore.one_byte.pixels != NULL) {
    free(image->exapp0.thumnailstore.one_byte.pixels);
    image->exapp0.thumnailstore.one_byte.pixels = NULL;
  }
  if (image->exapp0.thumnailstore.three_byte.pixels != NULL) {
    free(image->exapp0.thumnailstore.three_byte.pixels);
    image->exapp0.thumnailstore.three_byte.pixels = NULL;
  }
  // one byte will be allocated fro comment section
  // to display NULL byte to make sure their is no
  // comment
  free(image->comment.data);
  image->comment.data = NULL;

  if (image->quantizetables.table != NULL) {
    free(image->quantizetables.table);
    image->quantizetables.table = NULL;
  }

  if (image->sof0.sofc != NULL) {
    free(image->sof0.sofc);
    image->sof0.sofc = NULL;
  }

  if (image->huffmantable.symbols != NULL) {
    free(image->huffmantable.symbols);
    image->huffmantable.symbols = NULL;
  }
  if (image->startofscan.componentdata != NULL) {
    free(image->startofscan.componentdata);
    image->startofscan.componentdata = NULL;
  }
  if (image->compresseddata != NULL) {
    free(image->compresseddata);
    image->compresseddata = NULL;
  }

  // finally free the JPEG struct pointer
  free(image);

  image = NULL;
}

int main(void) {

  JPEG *image = jpeg_load_image("./src/test.jpg");
  jpeg_display(*image);
  jpeg_free(image);
  return 0;
}