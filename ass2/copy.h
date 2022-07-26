/* =============================================================================
   Project: Assignment 2 (altered from Assignment 1)
   copy.h :
            = related all to copying between structs/strings
============================================================================= */
#ifndef _COPY_H_
#define _COPY_H_

/* #includes ================================================================ */
#include "data.h"
#include "quadtree.h"

/* function prototypes ====================================================== */
char *filename_strcpy(char *arg);
footpath_segment_t *footpath_segment_cpy(footpath_segment_t *fp);
point2D_t *point_cpy(point2D_t *p);
rectangle2D_t *rectangle_cpy(rectangle2D_t *r);
datapoint_t *datapoint_cpy(datapoint_t *dp);
dpnode_t *dpnode_cpy(dpnode_t *node);
dpll_t *dpll_cpy(dpll_t *list);

#endif
/* =============================================================================
   Written by David Sha.
============================================================================= */
