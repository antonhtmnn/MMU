#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);
addr_t *tmp = (addr_t*)mem;

void setup_pt()
{
	tmp[5*PT_AMOUNT+0xa] = (PRESENCE<<12)|(PERM_READ_WRITE);
	tmp[5*PT_AMOUNT+0xa] += 0x500;

	tmp[5*PT_AMOUNT+0xb] = PERM_FULL_ACCESS;
	tmp[5*PT_AMOUNT+0xb] += 0x600;
}

void test(int test, char* description, int expected, int actual) {
	if (expected != actual) {
		fprintf(stderr, "Test %d: %s - expected 0x%X but got 0x%X\n", test, description, expected, actual);
		exit(1);
	}
}

int main()
{
	mmu_init(mem);
	setup_pt();

	request r = {0xace,5,READ};
	test(1, "Info bit should be set to presence", 0x8, tmp[5*PT_AMOUNT+0xa] >> 12);
	addr_t ptr = mmu_check_request(r);
	test(1, "Info bit should be set to accessed", 0xC, tmp[5*PT_AMOUNT+0xa] >> 12);

	addr_t expected = 0x5ce;
	test(1, "Address should be correct", expected, ptr);

	request r2 = {0xace,5,WRITE};
	expected = 0x5ce;
	test(2, "Info bit should be set to accessed", 0xC, tmp[5*PT_AMOUNT+0xa] >> 12);
	ptr = mmu_check_request(r2);
	test(2, "Address should be correct", expected, ptr);
	test(2, "Info bit should be set to modified", 0xE, tmp[5*PT_AMOUNT+0xa] >> 12);

	request r3 = {0xace,5,EXECUTE};
	expected = MY_NULL;
	ptr = mmu_check_request(r3);
	test(3, "Address should be correct", expected, ptr);
	

	request r4 = {0xbce,5,READ};
	expected = MY_NULL;
	ptr = mmu_check_request(r4);
	test(4, "Address should be correct", expected, ptr);

	return 0;
}
