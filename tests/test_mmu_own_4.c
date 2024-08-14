#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

/**************************************
 * testet, ob accessed, modified bits
 * korrekt gesetzt werden
 * ************************************/

int main()
{
	mmu_init(mem);
	
	addr_t *tmp = (addr_t*)mem;
	tmp[9*PT_AMOUNT] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
	tmp[9*PT_AMOUNT] += 0x500;

	request r = {0x02A,9,READ};
	addr_t ptr = mmu_check_request(r);

	addr_t expected = 0x52A;
	if (expected != ptr){
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}

	if(!(tmp[9*PT_AMOUNT] == 0xC507)){
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}

	r.type = WRITE;
	ptr = mmu_check_request(r);
	if(!(tmp[9*PT_AMOUNT] == 0xE507)){
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}

	printf("OK\n");

	return 0;
}
