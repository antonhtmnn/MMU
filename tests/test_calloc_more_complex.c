#include <stdio.h>

#include "calloc_helpers.h"

// Just trying out a bit of everything, vorgabe erweitert

#define MEM_SIZE ((size_t)(1024 * 1024)) // 1MB
static char mem[MEM_SIZE] __attribute__((aligned(8)));

int main() {
    char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int error = 0;

	my_calloc_init(mem, MEM_SIZE);

	size_t initial_free_memory = mem_size_free(mem);


	printf("Allocating\n");

	void * ptr1 = my_calloc(100, 1, 'A');
	void * ptr2 = my_calloc(1024, 512, 'B');
	void * ptr3 = my_calloc(1, 1, 'C');
    void * ptr4 = my_calloc(1, 1, 'D');
    void * ptr5 = my_calloc(5, 1, 'E');
    void * ptr6 = my_calloc(1, MEM_SIZE, 'E'); //should be NULL
    char * ptr7 = my_calloc(27, 1, 0);
    strncpy(ptr7, alphabet, 26);
    if(ptr6){
        printf("Pointer should be NULL");
        error++;
    }
	printf("Freeing\n");
    my_free(ptr6);
	my_free(ptr2);
    my_free(ptr1);
    my_free(ptr3);
    my_free(ptr5);
    my_free(ptr4);

    char* output = mem_concat_data(mem);
    printf("Expected %s\n ",alphabet);
    printf("Found    %s\n\n ",output);
    if(strcmp(output, alphabet) != 0){
        error++;
    }
    my_free(ptr7);

    void* ptr = my_calloc(1,1,'a');
    if(ptr != (((char*)mem) + sizeof(mem_block))){
        printf("Should be back at beginning");
        error++;
    }

    my_free(ptr);


    //filling up everything
    void *arr[40000] = {NULL};
    printf("Checking if Next Fit works. Allocating while freeing some space behind\n\n");
    for(size_t i = 0; i < 32768; i++){
        arr[i] = my_calloc(1,1,'c');
        if(i % 10 == 0 && i > 9){
            my_free(arr[i-1]);
        }
        if(arr[i] == NULL){
            printf("Pointer should not be NULL\n");
            error++;
        }
    }
    size_t free_blocks = mem_size_free(mem);
    printf("Expected %d\n", 3276* 8);
    printf("Found:   %zu\n\n", free_blocks);
    for(size_t i = 0; i < 32768; i++){
        if(i % 10 == 0 && i > 9){
            arr[i-1] = my_calloc(1,1,'a');
        }
    }
    free_blocks = mem_size_free(mem);
    printf("Expected %d\n", 0);
    printf("Found:   %zu\n\n", free_blocks);
    printf("This might take a few seconds depending on how efficient your calloc is\n\n");
    for(size_t i = 32768; i < 35000; i++){
        arr[i] = my_calloc(1, 1, 'c');
        if(arr[i] != NULL){
            printf("Pointer should be NULL\n");
            error++;
        }
    }

    free_blocks = mem_size_free(mem);
    printf("Expected 0\n");
    printf("Found:   %zu\n\n", free_blocks);
    if(free_blocks > 0){
        error++;
    }

    for(size_t i = 0; i < 40000; i++){
        my_free(arr[i]);
    }

    free_blocks = mem_size_free(mem);
    printf("Expected %zu\n", initial_free_memory);
    printf("Found:   %zu\n\n", free_blocks);
    if(free_blocks != initial_free_memory){
        error++;
    }


    printf("Checking correct melting behaviour\n\n ");
    void *array[10] = {NULL};
    unsigned int block_count = mem_block_count(mem);
    for (int i = 0; i < 10 ; i++) {
        array[i] = my_calloc(1,1,'c');
    }
    for (int i = 0; i < 10 ; i+=2) {
        my_free(array[i]);
    }
    block_count = mem_block_count(mem);

    printf("Expected 11\n");
    printf("Found:   %u\n\n", block_count);
    if(block_count != 11){
        error++;
    }
    unsigned int exp = 11;
    for (int i = 1; i < 10 ; i+=2) {
        my_free(array[i]);
        block_count = mem_block_count(mem);
        if(exp-2 != block_count){
            error++;
        }
        printf("Expected %u\n", exp-2);
        printf("Found:   %u\n\n", block_count);
        exp = block_count;
    }





	unsigned int final_block_count = mem_block_count(mem);
	printf("Expected block count: %u\n", 1);
	printf("Final block count   : %u\n", final_block_count);
	if(final_block_count != 1) {
		fprintf(stderr, "Final block count did not match expected count\n");
		error++;
	}

    printf("ERROR COUNT: %d\n", error);
    free(output);
	if(error) {
		exit(1);
	}

}
