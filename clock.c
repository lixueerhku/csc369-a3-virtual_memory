#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *ref_array; // Keep track of the reference bit

int curr_clock; // Keep track of the current position of the clock hand

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {

	while(ref_array[curr_clock] != 0){
		// If the current reference is 1, 
		// set it to be 0 and move to next position
		ref_array[curr_clock] = 0;
		curr_clock = (curr_clock + 1) % memsize;
	}

	// Find the page to evict, 
	// set the reference bit to be 1 after replacement.
	ref_array[curr_clock] = 1;
	
	return curr_clock;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {

	// Set the reference bit to be 1 on each access to the page.
	int frame = p->frame >> PAGE_SHIFT;
	ref_array[frame] = 1;

	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {

	curr_clock = 0;

	// Set every reference bit to be 0.
	ref_array = malloc(memsize * sizeof(int));
	int i;
	for(i = 0; i < memsize; i++){
		ref_array[i] = 0;
	}

}
