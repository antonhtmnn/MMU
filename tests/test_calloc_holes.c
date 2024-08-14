#include <stdio.h>

#include "calloc_helpers.h"

#define MEM_SIZE (96 + 5*sizeof(mem_block))

#define COL_0 "\033[0m"
#define COL_C "\033[1;36m"
#define COL_R "\033[1;31m"
#define COL_G "\033[1;32m"

MEMORY(MEM_SIZE, mem);

#define WHATSTR "Checks whether after allocating n elements, fully exhausting all space on the buffer, and freeing any element i < n, the allocator will properly iterate over the end of the list and find the newfly freed element for use."

static char *d[] = {
        "\x54\x68\x65\x20\x6f\x6e\x65\x20\x77\x68\x6f",
        "\x69\x73\x20\x62\x72\x61\x76\x65\x20\x65\x6e\x6f\x75\x67\x68",
        "\x74\x6f\x20\x73\x65\x61\x72\x63\x68,",
        "\x73\x68\x61\x6c\x6c\x20\x62\x65\x20\x6c\x75\x63\x6b\x79",
        "\x65\x6e\x6f\x75\x67\x68\x20\x74\x6f\x20\x66\x69\x6e\x64..."
};

int main() {
    unsigned i;
    char* ptrl[5];

    my_calloc_init(mem, MEM_SIZE);

    printf("["COL_C"*"COL_0"] Allocating 5 objects ...\n");

    for (i = 0; i < 5; i++) {
        if ((ptrl[i] = my_calloc(strlen(d[i]) + 1, sizeof(char), 0xdb)) == NULL) {
            printf("["COL_R"!"COL_0"] Error allocating object[%u] of size %lu ...\n", i, strlen(d[i]) + 1);

            exit(1);
        } else {
            strcpy(ptrl[i], d[i]);

            printf("["COL_C"*"COL_0"] Object[%u] of size %lu allocated and filled @ %p ...\n", i, strlen(d[i]) + 1, ptrl[i]);
        }
    }

    printf("["COL_C"*"COL_0"] Dumping memory after allocating ...\n");

    mem_print_blocks(mem);

    printf("["COL_C"*"COL_0"] Free-ing object[1] ...\n");

    my_free(ptrl[1]);

    printf("["COL_C"*"COL_0"] Dumping memory after free-ing object[1] ...\n");

    mem_print_blocks(mem);

    printf("["COL_C"*"COL_0"] Trying to re-allocate and insert object[1] ...\n");

    if ((ptrl[1] = my_calloc(strlen(d[1]) + 1, sizeof(char), 0xaa)) == NULL) {
        printf("["COL_R"!"COL_0"] Error re-allocating object[%u] of size %lu ...\n", i, strlen(d[i]) + 1);
        printf("["COL_R"!"COL_0"] You allocator likely doesn't iterate over the end of the list!\n");

        exit(1);
    }

    strcpy(ptrl[1], d[1]);

    printf("["COL_G"ez"COL_0"] Expected behaviour; doing a final memory dump ...\n");

    mem_print_blocks(mem);
}