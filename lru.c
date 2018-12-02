#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *last_time_ref; // The array keeps track of the last time the frame is referenced

int time; // The global variable is the latest time

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

	int min_idx = 0;
	int i;

	// Find the least recently used page frame number
	for(i = 0; i < memsize; i++){
		if(last_time_ref[i] < last_time_ref[min_idx]){
			min_idx = i;
		}
	}
	

	return min_idx;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	
	time++;

	//Increase the count for the page frame everytime it is referenced
	int frame = p->frame >> PAGE_SHIFT;
	last_time_ref[frame] = time;

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {

	time = 0;

	last_time_ref =  malloc(memsize * sizeof(int));
	
	int i;
	for(i = 0; i < memsize; i++){
		last_time_ref[i] = 0;
	}
}
