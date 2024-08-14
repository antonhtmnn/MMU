#include <stdio.h>

#include "calloc_helpers.h"

// Simple test that does a single allocation
#define MEM_SIZE (162760)
MEMORY(MEM_SIZE, mem);

int main() {

    //****************************************************************//
    // Zero nmemb test
	my_calloc_init(mem, MEM_SIZE);

    size_t total_size_start = mem_size_total(mem);

	// Uncomment to print memory layout
	// mem_print_blocks(mem);

	char * ptr = my_calloc(0, 100, 5);
	char * expected = NULL;
	printf("Expected: %p\nFound   : %p\n", expected, ptr);

	// Uncomment to print memory layout
	// mem_print_blocks(mem);

	if( expected != ptr ) {
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}

    if( expected != ptr ) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    size_t total_size_end = mem_size_total(mem);

    printf("Total memory at start: %zu\n", total_size_start);
    printf("Total memory at end  : %zu\n\n", total_size_end);

    if(total_size_start != total_size_end) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    //****************************************************************//
    // Zero size Test
    my_calloc_init(mem, MEM_SIZE);

    total_size_start = mem_size_total(mem);

    // Uncomment to print memory layout
    // mem_print_blocks(mem);

    ptr = my_calloc(100, 0, 5);
    expected = NULL;
    printf("Expected: %p\nFound   : %p\n", expected, ptr);

    // Uncomment to print memory layout
    // mem_print_blocks(mem);

    if( expected != ptr ) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    if( expected != ptr ) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    total_size_end = mem_size_total(mem);

    printf("Total memory at start: %zu\n", total_size_start);
    printf("Total memory at end  : %zu\n\n", total_size_end);

    if(total_size_start != total_size_end) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    //****************************************************************//
    // Both zero test
    my_calloc_init(mem, MEM_SIZE);

    total_size_start = mem_size_total(mem);

    // Uncomment to print memory layout
    // mem_print_blocks(mem);

    ptr = my_calloc(0, 0, 5);
    expected = NULL;
    printf("Expected: %p\nFound   : %p\n", expected, ptr);

    // Uncomment to print memory layout
    // mem_print_blocks(mem);

    if( expected != ptr ) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

    total_size_end = mem_size_total(mem);

    printf("Total memory at start: %zu\n", total_size_start);
    printf("Total memory at end  : %zu\n\n", total_size_end);

    if(total_size_start != total_size_end) {
        fprintf(stderr, "Did not match!\n");
        exit(1);
    }

}
