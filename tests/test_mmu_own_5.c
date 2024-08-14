#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

void setup_pt_1()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[PT_AMOUNT+0xa] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
	tmp[PT_AMOUNT+0xa] += 0x500;
}

void setup_pt_2()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[2*PT_AMOUNT+0x0] = (PRESENCE<<12) | (PERM_READ_WRITE);
	tmp[2*PT_AMOUNT+0x0] += 0xA00;
}

void setup_pt_3()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[3*PT_AMOUNT+0xF] = (ACCESSED<<12) | (PERM_FULL_ACCESS);
	tmp[3*PT_AMOUNT+0xF] += 0x400;
}

void setup_pt_4()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[4*PT_AMOUNT+0x3] = (PERM_FULL_ACCESS);
	tmp[4*PT_AMOUNT+0x3] += 0x200;
}

void setup_pt_5()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[5*PT_AMOUNT+0x4] = ((PRESENCE | MODIFIED)<<12) | (PERM_READ_WRITE);
	tmp[5*PT_AMOUNT+0x4] += 0xa00;
}

void setup_pt_6()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[6*PT_AMOUNT+0xb] = ((PRESENCE | ACCESSED)<<12) | (PERM_EXECUTE);
	tmp[6*PT_AMOUNT+0xb] += 0x100;
}

void setup_pt_7()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[7*PT_AMOUNT+0xa] = (((PRESENCE | ACCESSED) | MODIFIED)<<12) | (PERM_EXECUTE);
	tmp[7*PT_AMOUNT+0xa] += 0xf00;
}

void setup_pt_8()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[8*PT_AMOUNT+0x6] = (PRESENCE<<12) | (PERM_READ_ONLY);
	tmp[8*PT_AMOUNT+0x6] += 0x500;
}

void setup_pt_9()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[9*PT_AMOUNT+0x2] = (PRESENCE<<12) | (PERM_EXECUTE);
	tmp[9*PT_AMOUNT+0x2] += 0xb00;
}

int test_ph_adr(request r, addr_t expected)
{
	addr_t ptr = mmu_check_request(r);

	if (expected != ptr){
		printf("expected: \t%x\n", expected);
		printf("actual: \t%x\n", ptr);
		fprintf(stderr, "Did not match!\n");
		return 1;
	}
	return 0;
}

int test_table(int indx, addr_t expected)
{
	addr_t *tmp = (addr_t*)mem;
	if(tmp[indx] != expected){
		printf("expected: \t%x\n", expected);
		printf("actual: \t%x\n", tmp[indx]);
		fprintf(stderr, "Entry not as expected\n");
		return 1;
	}
	return 0;
}

int main()
{
	mmu_init(mem);
	int check_sum = 0;
	addr_t my_null = (addr_t) MY_NULL

	setup_pt_1();
	request r1 = {0xace,1,READ};
	check_sum += test_ph_adr(r1, 0x5ce);
	check_sum += test_table(PT_AMOUNT+0xa, 0xc507);

	setup_pt_2();
	request r2 = {0x001,2,WRITE};
	check_sum += test_ph_adr(r2, 0xa01);
	check_sum += test_table(2*PT_AMOUNT, 0xea03);

	setup_pt_3();
	request r3 = {0x42b,3,WRITE};
	check_sum += test_ph_adr(r3, my_null);
	check_sum += test_table(3*PT_AMOUNT+0xF, 0x4407);

	setup_pt_4();	// accessed bit geht nicht weg, wenn kein zugriff, wegen kein presence bit
	request r4 = {0x2c0,4,READ};
	check_sum += test_ph_adr(r4, my_null);
	check_sum += test_table(4*PT_AMOUNT+0x3, 0x0207);

	setup_pt_5();	// modified bit geht nicht weg, wenn read zugriff
	request r5 = {0x473,5,READ};
	check_sum += test_ph_adr(r5, 0xa73);
	check_sum += test_table(5*PT_AMOUNT+0x4, 0xea03);

	setup_pt_6();	// accessed bit geht nicht weg, wenn kein zugriff, wegen kein read bit
	request r6 = {0xb65,6,WRITE};
	check_sum += test_ph_adr(r6, my_null);
	check_sum += test_table(6*PT_AMOUNT+0xb, 0xc104);

	setup_pt_7();
	request r7 = {0xaf0,7,EXECUTE};
	check_sum += test_ph_adr(r7, 0xff0);
	check_sum += test_table(7*PT_AMOUNT+0xa, 0xef04);

	setup_pt_8();
	request r8 = {0x688,8,READ};
	check_sum += test_ph_adr(r8, 0x588);
	check_sum += test_table(8*PT_AMOUNT+0x6, 0xc501);

	setup_pt_9();	// should fail, if modified bit is set, when just execute
	request r9 = {0x21b,9,EXECUTE};
	check_sum += test_ph_adr(r9, 0xb1b);
	check_sum += test_table(9*PT_AMOUNT+0x2, 0xcb04);
	
	if(!check_sum) printf("OK\n");
	else {
		fprintf(stderr, "FAILED\n");
		exit(1);
	}

	return check_sum;
}
