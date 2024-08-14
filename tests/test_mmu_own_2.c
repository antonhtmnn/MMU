#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

void setup_pt()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[4*PT_AMOUNT+0x2] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
	tmp[4*PT_AMOUNT+0x2] += 0x500;
}

int main()
{
	mmu_init(mem);
	setup_pt();

	request r = {0x2ce,4,READ};
	addr_t ptr = mmu_check_request(r);

	addr_t expected = 0x5ce;
	if (expected != ptr){
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}
	printf("OK\n");

	return 0;
}
