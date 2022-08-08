/* =============================================================================
   Project: dict1
   readin.h :
            = related all to reading from input
============================================================================= */

#ifndef _READIN_H_
#define _READIN_H_

#define NULLBYTE 1      // size of nullbyte

/*  Copies specified argument to `filename` string. This includes a malloc.
    Ideally, every time this function is run, an associated `free(filename)`
    is to be included.
*/
char *filenamecpy(char *arg);

#endif

/* =============================================================================
   Written by David Sha.
============================================================================= */
