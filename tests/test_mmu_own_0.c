#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

void setup_pt()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[0] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
	tmp[0] += 0xF00;
}

int main()
{
	mmu_init(mem);
	setup_pt();

	request r = {0x0Ce,0,EXECUTE};
	addr_t ptr = mmu_check_request(r);

	addr_t expected = 0xFCe;
	if (expected != ptr){
		fprintf(stderr, "Did not match!\n");
		exit(1);
	}
	printf("OK\n");

	return 0;
}
