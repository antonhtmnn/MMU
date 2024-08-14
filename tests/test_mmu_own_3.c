#include "../include/mmu.h"

/*************************************
 * test testet nur fälle, in denen der 
 * zugriff verweigert werden soll
 * ***********************************/

MEMORY(MEM_SIZE,mem);

void setup_pt_1()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[5*PT_AMOUNT+0xa] = PERM_FULL_ACCESS;
	tmp[5*PT_AMOUNT+0xa] += 0x500;
}

void setup_pt_2()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[(PROC_NUM-1)*PT_AMOUNT+0xF] = (PRESENCE<<12) | (PERM_EXECUTE);
	tmp[(PROC_NUM-1)*PT_AMOUNT+0xF] += 0x100; 
}

void setup_pt_3()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[0x1] = (PRESENCE<<12) | (PERM_READ_ONLY);
}

void setup_pt_4()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[PT_AMOUNT+0x9] = (PRESENCE<<12) | (PERM_EXECUTE);
	tmp[PT_AMOUNT+0x9] += 0x900;
}

void setup_pt_5()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[2*PT_AMOUNT] = (PRESENCE<<12);
	tmp[2*PT_AMOUNT] += 0x700;
}

void setup_pt_8()
{
	addr_t *tmp = (addr_t*)mem;
	tmp[3*PT_AMOUNT+0xF] = (PRESENCE<<12) | (PERM_READ_WRITE);
	tmp[3*PT_AMOUNT+0xF] += 0xB00;
}

int test(request r, addr_t expected) 
{
	addr_t ptr = mmu_check_request(r);
	
	if (expected != ptr){
		fprintf(stderr, "Did not match!\n");
		return 1;
	}
	return 0;
}

int main()
{
	mmu_init(mem);
	int check_sum = 0;
	addr_t my_null = (addr_t) MY_NULL

	/* presence bit is not set */

	setup_pt_1();
	request r1 = {0xace,5,READ};
	check_sum += test(r1, my_null);
	
	/* process wants to write, but has just permission to execute */

	setup_pt_2();
	request r2 = {0xF01,PROC_NUM-1,WRITE};
	check_sum += test(r2, my_null);

	/* process wants to write, but read only */

	setup_pt_3();
	request r3 = {0x1FF,0,WRITE};
	check_sum = test(r3, my_null);

	/* process wants to read, but just permission to execute */

	setup_pt_4();
	request r4 = {0x910,1,READ};
	check_sum += test(r4, my_null);

	/* process wants to read, but no permission at all */

	setup_pt_5();
	request r5 = {0x001,2,READ};
	check_sum += test(r5, my_null);

	/* process wants to execute, but no permission at all */

	request r6 = {0x001,2,EXECUTE};
	check_sum += test(r6, my_null);

	/* process wants to execute, but just permission to read */

	request r7 = {0x1FF,0,EXECUTE};
	check_sum += test(r7, my_null);

	/* process wants to execute, but just permission to read and write */

	setup_pt_8();
	request r8 = {0xBAA,3,EXECUTE};
	check_sum += test(r8, my_null);
	

	if(!check_sum) printf("OK\n");
	else {
		fprintf(stderr, "FAILED\n");
		exit(1);
	}

	return check_sum;
}
