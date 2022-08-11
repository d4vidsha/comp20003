/* =============================================================================
   Project: Assignment 1
   quicksort.h :
            = all related to quicksort
============================================================================= */
#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

/* #includes ================================================================ */
#include "data.h"
#include "linkedlist.h"

/* function prototypes ====================================================== */
void quicksort(node_t *low, node_t *high);
node_t *partition(node_t *low, node_t *high);
void swap(node_t *n1, node_t *n2);
node_t *get_prev_node(node_t *start, node_t *node);

#endif
/* =============================================================================
   Written by David Sha.
============================================================================= */