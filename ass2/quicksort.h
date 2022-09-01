/* =============================================================================
   Project: Assignment 2 (altered from Assignment 1)
   quicksort.h :
            = all related to quicksort
============================================================================= */
#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

/* #includes ================================================================ */
#include "data.h"
#include "linkedlist.h"

/* function prototypes ====================================================== */
void quicksort(list_t *list, int col);
node_t *partition(node_t *low, node_t *high, int col);
void swap(node_t *n1, node_t *n2);
node_t *get_prev_node(node_t *start, node_t *node);
double cmp_column(int col, footpath_segment_t *n, footpath_segment_t *m);
double cmp_grade1in(double n, double m);
int cmp_address(char *n, char *m);
int cmp_footpath_id(int n, int m);

array_t *convert_dpll_to_array(dpll_t *list);
void quicksort_array(array_t *A);
int partition_array(array_t *A, int lo, int hi);
void swap_elem(arrat_t *A, int i, int j);

#endif
/* =============================================================================
   Written by David Sha.
============================================================================= */