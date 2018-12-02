#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h> 
#include "pagetable.h"
#define MAXLINE 256
#define TRUE 1
#define FALSE 0


extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char* tracefile;

int trace_file_size = 0;

unsigned long *ref_array; // Stores the reference sequence from the tracefile

unsigned long *mem_array; // Stores the page frames

int curr_ref_index = 0; // Keep track of the index of the current page frame in the reference sequence

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int i, j;
	int farthest = 0;
	int evit_frame;

	// Starting from the beginning of the memory array(wihich is full now),
	// try to find the frame that will be used in the farthest future
	for(i = 0; i < memsize; i++){
		bool exist_in_future = FALSE;
		for(j = curr_ref_index; j < trace_file_size; j++){
			if(mem_array[i] == ref_array[j]){
				exist_in_future = TRUE;
				if(j > farthest){
					farthest = j;
					evit_frame = i;
				}
				break;
			}
		}
		// If the frame will not be used in the future, evict it.
		if(!exist_in_future){
			return i;
		}
	}
	
	return evit_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {

	// Get the input frame
	int frame = p->frame;
	frame = frame >> PAGE_SHIFT;
	
	mem_array[frame] = ref_array[curr_ref_index];

	// Update the current reference index on each access to a page 
	curr_ref_index++;

	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	mem_array = malloc(memsize * sizeof(unsigned long));

	// Load the tracefile
	FILE *stdin;
	if((stdin = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}

	// Get the size of the tracefile
	char line[MAXLINE]; 
	char type;
	while(fgets(line, MAXLINE, stdin) != NULL){
		trace_file_size++; 
	}

	ref_array = malloc(trace_file_size * sizeof(unsigned long));

	// Open the tracefile again to load every address to the array
	if((stdin = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}

	addr_t vaddr = 0;
	int i = 0;
	while(fgets(line, MAXLINE, stdin) != NULL){
		if(line[0] != '=') {
			sscanf(line, "%c %lx", &type, &vaddr);
			ref_array[i] =  vaddr; 
			i++; 
		} 
	}

}

