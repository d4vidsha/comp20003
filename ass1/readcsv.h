/* =============================================================================
   Project: Assignment 1
   readcsv.h :
            = related all to reading from .csv file
============================================================================= */
#ifndef _READCSV_H_
#define _READCSV_H_

/* #includes ================================================================ */
#include "copy.h"
#include "data.h"
#include "linkedlist.h"

/* function prototypes ====================================================== */
int is_in_cell_length(int curr, int max);
void get_str(FILE *f, char *string);
int get_int(FILE *f);
double get_double(FILE *f);
void skip_header_line(FILE *f);
footpath_segment_t *footpath_read_line(FILE *f);

#endif
/* =============================================================================
   Written by David Sha.
============================================================================= */
