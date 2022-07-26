/* Skeleton for W4.7, comp20003 workshop Week 4 */

/*-------------------------------------------------------------- 
..Project: postfix
  stack.[c,h] :  module stack of the project
----------------------------------------------------------------*/

#include "stack.h"

stackADT_t *stackCreate() {
	// FILL IN just one simple line here
    return listCreate();
} 

void push(stackADT_t *s, int x) {
	// FILL IN just one simple line here
    return listPrepend(s, x);
}

int pop(stackADT_t *s) {
	// FILL IN just one simple line here
    return listDeleteHead(s);
} 

void stackFree(stackADT_t *s) {
	// FILL IN just one simple line here
    listFree(s);
}

int stackCount(stackADT_t *s) {
	// FILL IN just one simple line here
    return listCount(s);
}



/* =====================================================================
   This skeleton/program is compiled by the comp20003 teaching team,
   the university of Melbourne.
   Last Updated: 15/07/2022
   ================================================================== */

