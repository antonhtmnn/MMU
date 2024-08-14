#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

void failed()
{
	fprintf(stderr, "FAILED\n");
	exit(1);
}

void ok()
{
	printf("OK\n");
}

int main()
{
	mmu_init(mem);
	if(PROC_NUM > 0 && switch_process(PROC_NUM-1)) failed();
	else if(switch_process(0)) failed();
	else if(!switch_process(-1)) failed();
	else if(!switch_process(PROC_NUM)) failed();
	else ok();

	return 0;
}
