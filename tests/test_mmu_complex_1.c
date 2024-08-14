#include "../include/mmu.h"

MEMORY(MEM_SIZE,mem);

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c "
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)

void setup_pt()
{
	addr_t *tmp = (addr_t*)mem;
  tmp[5*PT_AMOUNT+0xa] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[5*PT_AMOUNT+0xa] += 0x500;
  tmp[3*PT_AMOUNT+0xb] = (PRESENCE<<12)|(PERM_READ_ONLY);
  tmp[3*PT_AMOUNT+0xb] += 0x400;
  tmp[1*PT_AMOUNT+0x3] = (ACCESSED<<12)|(PERM_FULL_ACCESS);
  tmp[1*PT_AMOUNT+0x3] += 0x300;
  tmp[6*PT_AMOUNT+0x6] = ((PRESENCE | ACCESSED | MODIFIED ) << 12) |(PERM_FULL_ACCESS);
  tmp[6*PT_AMOUNT+0x6] += 0x100;
  tmp[2*PT_AMOUNT+0xa] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[2*PT_AMOUNT+0xa] += 0x300;
  tmp[0x1] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[0x1] += 0x000;
  tmp[PROC_NUM*PT_AMOUNT+0x1] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[PROC_NUM*PT_AMOUNT+0x1] += 0xF00;
  tmp[(PROC_NUM-1)*PT_AMOUNT+0x1] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[(PROC_NUM-1)*PT_AMOUNT+0x1] += 0xF00;
  tmp[0x0] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[0x0] += 0x100;

  //ALL PERMISSIONS
  tmp[8*PT_AMOUNT+0x0] = (PRESENCE<<12)|(PERM_NO_ACCESS);
  tmp[8*PT_AMOUNT+0x0] += 0xf00;
  tmp[8*PT_AMOUNT+0x1] = (PRESENCE<<12)|(PERM_READ_ONLY);
  tmp[8*PT_AMOUNT+0x1] += 0xe00;
  tmp[8*PT_AMOUNT+0x2] = (PRESENCE<<12)|(PERM_READ_WRITE);
  tmp[8*PT_AMOUNT+0x2] += 0xd00;
  tmp[8*PT_AMOUNT+0x3] = (PRESENCE<<12)|(PERM_EXECUTE);
  tmp[8*PT_AMOUNT+0x3] += 0xc00;
  tmp[8*PT_AMOUNT+0x4] = (PRESENCE<<12)|(PERM_FULL_ACCESS);
  tmp[8*PT_AMOUNT+0x4] += 0xb00;
}

int main()
{
  int error = 0;
  int test_id=0;
	mmu_init(mem);
	setup_pt();

  request r;
  addr_t ptr, expected;

  test_id++;
  printf("%3d: Translating a virtual address to physical address\n", test_id);
	r.addr = 0xace;
  r.p_num = 5;
  r.type = READ;
	ptr = mmu_check_request(r);
	expected = 0x5ce;
	if (expected != ptr){
		fprintf(stderr, "%3d: Failed: Adress translation wrong!\n", test_id);
    error++;
	}

  test_id++;
  printf("%3d: Trying to write access a read-only address\n", test_id);
  r.addr = 0xbfa;
  r.p_num = 3;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: Requested write on a read-only page: access should've been prohibited\n", test_id);
  }

  test_id++;
  printf("%3d: Trying to access the same address with correct req READ\n",test_id);
  r.addr = 0xbfa;
  r.p_num = 3;
  r.type = READ;
  ptr = mmu_check_request(r);
  expected = 0x4fa;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: Permitted READ access returned wrong address.\n",test_id);
  }

  test_id++;
  printf("%3d: Trying to access an address without presence set\n", test_id);
  r.addr = 0x355;
  r.p_num = 1;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: Presence wasn't set: access should've been prohibited\n", test_id);
  }

  test_id++;
  printf("%3d: Trying to access an address that has all pt_information set\n", test_id);
  r.addr = 0x642;
  r.p_num = 6;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = 0x142;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: Presence is set but not the correct address returned\n", test_id);
  }

  test_id++;
  printf("%3d: Trying to get lowest process id\n", test_id);
  r.addr = 0x142;
  r.p_num = 0;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = 0x42;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: lowest process id returned wrong address\n",test_id);
  }

  test_id++;
  printf("%3d: Trying to get highest process id\n", test_id);
  r.addr = 0x142;
  r.p_num = PROC_NUM-1;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = 0xF42;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: highest process id returned wrong address\n",test_id);
  }

  test_id++;
  printf("%3d: Trying to get a process out of range\n", test_id);
  r.addr = 0x142;
  r.p_num = PROC_NUM;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
    fprintf(stderr, "%3d: Failed: process id out of range shouldn't return an address\n", test_id);
  }

  test_id++;
  printf("%3d: Check if ACCESSED bit will be set correctly\n", test_id);
  r.addr = 0xa11;
  r.p_num = 2;
  r.type = READ;
  ptr = mmu_check_request(r);
  expected = 0x311;
  if (expected != ptr){
    fprintf(stderr, "%3d: Failed: Adress translation wrong!\n", test_id);
    error++;
  }
  addr_t *tmp = (addr_t*)mem;
  if (!(tmp[2*PT_AMOUNT+0xa] & (ACCESSED<<12))) {
    fprintf(stderr, "%3d: Failed: ACCESSED bit was not set\n", test_id);
  }
  if ((tmp[2*PT_AMOUNT+0xa] & (MODIFIED<<12))) {
    fprintf(stderr, "%3d: Failed: MODIFIED bit shouldn't be set\n", test_id);
  }

  test_id++;
  tmp[2*PT_AMOUNT+0xa] = tmp[2*PT_AMOUNT+0xa] & ~(ACCESSED<<12);
  printf("%3d: Check if MODIFIED bit will be set correctly\n", test_id);
  r.addr = 0xa11;
  r.p_num = 2;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = 0x311;
  if (expected != ptr){
    fprintf(stderr, "%3d: Failed: Adress translation wrong!\n", test_id);
    error++;
  }
  if (!(tmp[2*PT_AMOUNT+0xa] & (ACCESSED<<12))) {
    fprintf(stderr, "%3d: Failed: ACCESSED bit was not set\n", test_id);
  }
  if (!(tmp[2*PT_AMOUNT+0xa] & (MODIFIED<<12))) {
    fprintf(stderr, "%3d: Failed: MODIFIED bit was not set\n", test_id);
  }


  //ADDITIONS

  test_id++;
  printf("%3d: Trying to get lowest process id, page 0\n", test_id);
  r.addr = 0xa4;
  r.p_num = 0;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = 0x1a4;
  if (expected != ptr) {
      fprintf(stderr, "%3d: Failed: lowest process id, page 0, returned wrong address\n",test_id);
  }


  // CHECKING ALL ACCESS POSSIBILITIES

  // ACCESSING NO PERMISSION

  test_id++;
  printf("%3d: Trying to READ access a no permission address\n", test_id);
  r.addr = 0x45;
  r.p_num = 8;
  r.type = READ;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
      fprintf(stderr, "%3d: Failed: Requested READ on a no permission page: access should've been prohibited\n", test_id);
  }

  test_id++;
  printf("%3d: Trying to WRITE access a no permission address\n", test_id);
  r.addr = 0x45;
  r.p_num = 8;
  r.type = WRITE;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
      fprintf(stderr, "%3d: Failed: Requested WRITE on a no permission page: access should've been prohibited\n", test_id);
  }

  test_id++;
  printf("%3d: Trying to EXECUTE access a no permission address\n", test_id);
  r.addr = 0x45;
  r.p_num = 8;
  r.type = EXECUTE;
  ptr = mmu_check_request(r);
  expected = MY_NULL;
  if (expected != ptr) {
      fprintf(stderr, "%3d: Failed: Requested EXECUTE on a no permission page: access should've been prohibited\n", test_id);
  }


  // ACCESSING READ ONLY

    test_id++;
    printf("%3d: Trying to WRITE access a read-only address\n", test_id);
    r.addr = 0x145;
    r.p_num = 8;
    r.type = WRITE;
    ptr = mmu_check_request(r);
    expected = MY_NULL;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Requested WRITE on a read-only page: access should've been prohibited\n", test_id);
    }

    test_id++;
    printf("%3d: Trying to EXECUTE access a read-only address\n", test_id);
    r.addr = 0x145;
    r.p_num = 8;
    r.type = EXECUTE;
    ptr = mmu_check_request(r);
    expected = MY_NULL;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Requested EXECUTE on a read-only page: access should've been prohibited\n", test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req READ\n",test_id);
    r.addr = 0x145;
    r.p_num = 8;
    r.type = READ;
    ptr = mmu_check_request(r);
    expected = 0xe45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted READ access returned wrong address\n",test_id);
    }

    // ACCESSING READ-WRITE

    test_id++;
    printf("%3d: Trying to EXECUTE access a read-write address\n", test_id);
    r.addr = 0x245;
    r.p_num = 8;
    r.type = EXECUTE;
    ptr = mmu_check_request(r);
    expected = MY_NULL;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Requested EXECUTE on a read-write page: access should've been prohibited\n", test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req READ\n",test_id);
    r.addr = 0x245;
    r.p_num = 8;
    r.type = READ;
    ptr = mmu_check_request(r);
    expected = 0xd45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted READ access returned wrong address\n",test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req WRITE\n",test_id);
    r.addr = 0x245;
    r.p_num = 8;
    r.type = WRITE;
    ptr = mmu_check_request(r);
    expected = 0xd45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted WRITE access returned wrong address\n",test_id);
    }


    // ACCESSING EXECUTE ONLY

    test_id++;
    printf("%3d: Trying to WRITE access a execute-only address\n", test_id);
    r.addr = 0x345;
    r.p_num = 8;
    r.type = WRITE;
    ptr = mmu_check_request(r);
    expected = MY_NULL;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Requested WRITE on a execute-only page: access should've been prohibited\n", test_id);
    }

    test_id++;
    printf("%3d: Trying to READ access a execute-only address\n", test_id);
    r.addr = 0x345;
    r.p_num = 8;
    r.type = READ;
    ptr = mmu_check_request(r);
    expected = MY_NULL;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Requested READ on a execute-only page: access should've been prohibited\n", test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req EXECUTE\n",test_id);
    r.addr = 0x345;
    r.p_num = 8;
    r.type = EXECUTE;
    ptr = mmu_check_request(r);
    expected = 0xc45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted EXECUTE access returned wrong address\n",test_id);
    }


    // ACCESSING PERM-FULL

    test_id++;
    printf("%3d: Trying to access a PERM-FULL address with correct req READ\n",test_id);
    r.addr = 0x445;
    r.p_num = 8;
    r.type = READ;
    ptr = mmu_check_request(r);
    expected = 0xb45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted READ access returned wrong address\n",test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req WRITE\n",test_id);
    r.addr = 0x445;
    r.p_num = 8;
    r.type = WRITE;
    ptr = mmu_check_request(r);
    expected = 0xb45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted WRITE access returned wrong address\n",test_id);
    }

    test_id++;
    printf("%3d: Trying to access the same address with correct req EXECUTE\n",test_id);
    r.addr = 0x445;
    r.p_num = 8;
    r.type = EXECUTE;
    ptr = mmu_check_request(r);
    expected = 0xb45;
    if (expected != ptr) {
        fprintf(stderr, "%3d: Failed: Permitted EXECUTE access returned wrong address\n",test_id);
    }




    if (error) {
    exit(1);
  }
	return 0;
}
