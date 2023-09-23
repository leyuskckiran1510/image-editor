#ifndef __JPEG__
#define __JPEG__ 1.2

#include "./../image-editor.h"

typedef void (*jpeg_func)(FILE*,JPEG*);

// Use this macro to add new function defination;
// easy to add and expand

JPEG * jpeg_init();
void jpeg_free(JPEG *);
JPEG * jpeg_load_image(const char *);
void jpeg_display(JPEG);


#define list U(app0)\
             U(ex_app0)\
             U(commment)\
             U(quantize)\
             U(start_frameN)\
             U(restart_interval)\
             U(huffman_table)\
             U(start_scan)\
             U(restart_markern)\
             U(number_lines)\
             U(hierarchival_pg)\
             U(expand_ref_comp)\
             U(app_segmtN)\
             U(jpeg_extN)\
             U(compressed)\
             U(cast)\

// Look at expnd (1);below
#define U(x) void jpeg_##x(FILE*,JPEG*);
        list
#undef U

// Look at expnd (2);below
// will use this array latter if I can
// make it more modular
static const jpeg_func jpeg_function[]={
  #define U(x) &jpeg_##x,
    list
  #undef U
};

/*

 EXPND (1) 
Expands To
void jpeg_app0(JPEG*,FILE*);
void jpeg_ex_app0(JPEG*,FILE*);
void jpeg_commment(JPEG*,FILE*);
void jpeg_quantize(JPEG*,FILE*);
void jpeg_start_frameN(JPEG*,FILE*);
void jpeg_restart_interval(JPEG*,FILE*);
void jpeg_huffman_table(JPEG*,FILE*);
void jpeg_start_scan(JPEG*,FILE*);
void jpeg_restart_markern(JPEG*,FILE*);
void jpeg_number_lines(JPEG*,FILE*);
void jpeg_hierarchival_pg(JPEG*,FILE*);
void jpeg_expand_ref_comp(JPEG*,FILE*);
void jpeg_app_segmtN(JPEG*,FILE*);
void jpeg_jpeg_extN(JPEG*,FILE*);

 EXPND (2) 
 static const jpeg_func jpeg_function[]={
    &jpeg_app0,
    &jpeg_ex_app0,
    &jpeg_commment,
    &jpeg_quantize,
    &jpeg_start_frameN,
    &jpeg_restart_interval,
    &jpeg_huffman_table,
    &jpeg_start_scan,
    &jpeg_restart_markern,
    &jpeg_number_lines,
    &jpeg_hierarchival_pg,
    &jpeg_expand_ref_comp,
    &jpeg_app_segmtN,
    &jpeg_jpeg_extN,
};

*/


#endif