#include <stdio.h>

#include "calloc_helpers.h"

// Tests 3 basic functions:
// 1. calloc setting NULL with size==0 (as answered in forum)
// 2. the return pointer being correct
// 3. the storage being able to be filled completely

#define MEM_SIZE ((size_t)(1024 * 1024)) // 1MB
static char mem[MEM_SIZE] __attribute__((aligned(8)));

int main() {

	int error = 0;

	my_calloc_init(mem, MEM_SIZE);

	size_t initial_free_memory = mem_size_free(mem);

	mem_print_blocks(mem);


	printf("Allocating size 0\n");

	void * ptr1 = my_calloc(1, 0, 'A');
	void * ptr2 = my_calloc(0, 1, 'B');
	void * ptr3 = my_calloc(0, 0, 'C');
	
	if(ptr1!=NULL || ptr2!=NULL || ptr3!=NULL){
		printf("did not return NULL\n");
		error++;
		mem_print_blocks(mem);
	}
	

	printf("Checking the return pointer\n");

	char * str1 = my_calloc(10, 1, 'A');
	if (str1 - sizeof(mem_block) != mem){
		printf("%p mem vs %p return pointer vs %lu size of memblock \n",mem, str1, sizeof(mem_block));
		error++;
	}
	my_free(str1);

	printf("These should be empty (not testcase but necessary for later test) \n");
	mem_print_blocks(mem);

	
	printf("Checking for filling all the space at once \n");
	printf("If this gives you segfault or more than 1 memblock you do something wrong (no testcase)\n");
	my_calloc(8, initial_free_memory/8, 'A');
	mem_print_blocks(mem);
	

	if(error) {
		exit(1);
	}
}
