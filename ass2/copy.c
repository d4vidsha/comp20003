/* =============================================================================
   Project: Assignment 2 (altered from Assignment 1)
   copy.c :
            = related all to copying between structs/strings
============================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "copy.h"
#include "data.h"

/*  Copies specified argument to `filename` string. This includes a malloc.
    Ideally, every time this function is run, an associated `free(filename)`
    is to be included.
*/
char *filename_strcpy(char *arg) {
    int filename_len = strlen(arg) + NULLBYTE_LEN;
    char *filename = (char *)malloc(sizeof(char) * filename_len);
    assert(filename);
    strcpy(filename, arg);
    return filename;
}

/*  Copies a given `footpath_segment` struct to a new struct.
*/
footpath_segment_t *footpath_segment_cpy(footpath_segment_t *fp) {
    assert(fp);
    footpath_segment_t *new;
    new = (footpath_segment_t *)malloc(sizeof(*new));
    assert(new);
    memcpy(new, fp, sizeof(*new));
    return new;
}

/*  Copies a given point struct to a new point struct.
*/
point2D_t *point_cpy(point2D_t *p) {
    assert(p);
    point2D_t *new;
    new = (point2D_t *)malloc(sizeof(*new));
    assert(new);
    memcpy(new, p, sizeof(*new));
    return new;
}

/*  Copies a given rectangle struct to a new rectangle struct.
*/
rectangle2D_t *rectangle_cpy(rectangle2D_t *r) {
    assert(r);
    rectangle2D_t *new;
    new = (rectangle2D_t *)malloc(sizeof(*new));
    assert(new);
    new->bl = point_cpy(r->bl);
    new->tr = point_cpy(r->tr);
    return new;
}

/*  Copies a given datapoint struct to a new datapoint struct.
*/
datapoint_t *datapoint_cpy(datapoint_t *dp) {
    assert(dp);
    datapoint_t *new;
    new = (datapoint_t *)malloc(sizeof(*new));
    assert(new);
    new->fp = footpath_segment_cpy(dp->fp);
    new->p = point_cpy(dp->p);
    return new;
}

/*  Copies a given datapoint node struct to a new datapoint node struct.
*/
dpnode_t *dpnode_cpy(dpnode_t *node) {
    assert(node);
    dpnode_t *new;
    new = (dpnode_t *)malloc(sizeof(*new));
    assert(new);
    new->dp = datapoint_cpy(node->dp);
    if (node->next != NULL) {
        new->next = dpnode_cpy(node->next);
    } else {
        new->next = NULL;
    }
    return new;
}

/*  Copies a given datapoint list struct to a new datapoint list struct.
*/
dpll_t *dpll_cpy(dpll_t *list) {
    assert(list);
    dpll_t *new;
    new = (dpll_t *)malloc(sizeof(*new));
    assert(new);
    new->head = dpnode_cpy(list->head);
    dpnode_t *curr = new->head;
    while (curr) {
        curr = curr->next;
    }
    new->foot = curr;
    return new;
}

/* =============================================================================
   Written by David Sha.
============================================================================= */