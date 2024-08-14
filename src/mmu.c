#include "../include/mmu.h"

/* Memory start address: Startadresse unseres 12 bit Adressraumes */
static void *mem_start_addr;

/* page table base register = Seitentabellenbasisregister */
static addr_t *ptbr;

/* +------------------------------------+ *
 * | Hier ist Platz für Hilfsfunktionen | *
 * +------------------------------------+ */



/* -------------------------------------- */

void mmu_init(void* mem)
{
	mem_start_addr = mem;
	ptbr = mem;
}

int switch_process(int proc_id)
{
	if (proc_id >= 0 && proc_id < PROC_NUM) {
		ptbr = ((addr_t *) mem_start_addr) + (proc_id * PT_AMOUNT);
		return 0;
	}
	return 1;
}

addr_t mmu_translate(addr_t va, req_type req)
{
	int page_num = va & 0b0000111100000000;
	page_num = page_num >> 8;
	int offset = va & 0b0000000011111111;

	addr_t *pte = ptbr + page_num;
	int info = *pte & 0b1111000000000000;
	info = info >> 12;
	int page_frame = *pte & 0b0000111100000000;
	page_frame = page_frame >> 8;
	int permission = *pte & 0b111;

	if (((req & permission) > 0) && info >= PRESENCE) {
		*pte = (*pte | (ACCESSED << 12)); // set ACCESSED bit (in info)
		if (req == WRITE) {
			*pte = (*pte | (MODIFIED << 12)); // set MODIFIED bit (in info)
		}
		return ((page_frame << 8) | offset);
	}
	return MY_NULL;
}

addr_t mmu_check_request(request r)
{
	if (switch_process(r.p_num) == 0) {
		return mmu_translate(r.addr, r.type);
	}
	return MY_NULL;
}
