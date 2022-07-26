/* =============================================================================
   Project: Assignment 2 (altered from Assignment 1)
   stage.c :
            = the stages of the project which processes queries on the fly
============================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stage.h"
#include "print.h"
#include "linkedlist.h"
#include "readcsv.h"
#include "data.h"
#include "quicksort.h"
#include "search.h"
#include "array.h"
#include "convert.h"

/*  Stage 1 of project. Finds the footpath segments by address and
    prints them to the provided file `out`.
*/
void stage1(FILE *out, list_t *list) {

    // process queries on the fly from `stdin`
    char line[MAX_STR_LEN + NEWLINE_LEN + NULLBYTE_LEN];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;      // removes "\n" from line

        // find addresses
        list_t *result_list = find_addresses(line, list);

        // print to `out` file
        fprintf(out, "%s\n", line);
        print_list(out, result_list);
        
        // print to `stdout`
        int list_length = list_len(result_list);
        if (list_length) {
            printf("%s --> %d\n", line, list_length);
        } else {
            printf("%s --> %s\n", line, NOTFOUND);
        }

        free_list(DEEP, result_list);
    }
    free_list(DEEP, list);
}

/*  Stage 2 of project. Finds the closeest footpath segments by `grade1in` 
    and prints them to the provided file `out`.
*/
void stage2(FILE *out, list_t *list) {

    // sort linked list from lowest to highest by the given column index
    // note that there is a better function to sort data
    // see `quicksort_array()` for more details.
    quicksort(list, COLUMN_INDEX_GRADE1IN);

    // convert linked list to array and remove linked list
    array_t *array = convert_to_array(DEEP, list);
    free_list(DEEP, list);

    // process queries on the fly from `stdin`
    char line[MAX_STR_LEN + NEWLINE_LEN + NULLBYTE_LEN];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;      // removes "\n" from line

        // parse string to double
        char *ptr;
        double value = strtod(line, &ptr);

        // search through linked list for closest match
        array_t *results = binarysearch(value, array);
        
        // print to `out` file
        fprintf(out, "%s\n", line);
        print_array(out, results);

        // print to `stdout`
        printf("%s --> %.1lf\n", line, results->A[0]->grade1in);
        
        free_array(DEEP, results);
    }
    free_array(DEEP, array);
}

/*  Stage 3 of project. Given coordinate queries from `stdin` of format
    `x y`, find the footpath segments that are in the quadtree node
    specified by the coordinate and prints them to the provided file `out`.
*/
void stage3(FILE *out, qtnode_t *tree) {

    // process queries on the fly from `stdin`
    char line[MAX_STR_LEN + NEWLINE_LEN + NULLBYTE_LEN];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;      // removes "\n" from line

        // parse string to double from `x y` to the point (x, y)
        double x, y;
        char *ptr = line;
        x = strtod(ptr, &ptr);
        y = strtod(ptr, &ptr);
        point2D_t *point = create_point(x, y);

        // search for the point returning a pointer to the results
        // directly on the quadtree
        printf("%s -->", line);
        dpll_t *results = search_quadtree(tree, point);

        // print the query to `out` file
        fprintf(out, "%s\n", line);

        // results could be `NULL` if no footpath segments are found, 
        // in which case we don't do anything
        if (results) {
            // sort results
            array_t *resarr = convert_dpll_to_array(SHALLOW, results);
            quicksort_array(COLUMN_INDEX_FPID, resarr, 0, resarr->n - 1);
            check_array_sorted(COLUMN_INDEX_FPID, resarr);
            
            // print results
            print_array(out, resarr);
            free_array(SHALLOW, resarr);
        }
        free_point(point);
    }
}

/*  Stage 4 of project. Given coordinate queries from `stdin` of format
    `x1 y1 x2 y2`, find the footpath segments that are within the range
    (x1, y1), (x2, y2) and prints them to the provided file `out`.
*/
void stage4(FILE *out, qtnode_t *tree) {

    // process queries on the fly from `stdin`
    char line[MAX_STR_LEN + NEWLINE_LEN + NULLBYTE_LEN];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;      // removes "\n" from line

        // parse string to double from format `x1 y1 x2 y2` to the
        // rectangle range ((x1, y1), (x2, y2))
        rectangle2D_t *range = get_rectangle(line);

        // range search for all datapoints within range with `results`
        // being a `SHALLOW` `dpll` i.e. list only has pointers to data from
        // the quadtree
        printf("%s -->", line);
        dpll_t *results = range_search_quadtree(tree, range);
        free_rectangle(range);
        printf("\n");

        // initiate printing the results to `out` file
        fprintf(out, "%s\n", line);

        // convert to an array and then sort the array
        array_t *resarr = convert_dpll_to_array(SHALLOW, results);
        free_dpll(SHALLOW, results);
        quicksort_array(COLUMN_INDEX_FPID, resarr, 0, resarr->n - 1);
        check_array_sorted(COLUMN_INDEX_FPID, resarr);
        
        // convert array to a linked list so that we can remove duplicates
        list_t *reslist = convert_array_to_list(SHALLOW, resarr);
        free_array(SHALLOW, resarr);
        reslist = remove_duplicates(COLUMN_INDEX_FPID, reslist);

        // finally print results to `out` file
        print_list(out, reslist);
        free_list(SHALLOW, reslist);
    }
}

/* =============================================================================
   Written by David Sha.
   - Read line-by-line from stdin: https://stackoverflow.com/a/9206332/15444163
   - Remove newline from fgets: https://stackoverflow.com/a/28462221/15444163
============================================================================= */