#include "../include/calloc.h"
// Für memset
#include <string.h>

static void * MEM;
static size_t MEM_SIZE;

/* Die aktuelle Position für das Next Fit Verfahren */
static mem_block * last_allocation = NULL;

void my_calloc_init(void * mem, size_t mem_size){
	MEM = mem;
	MEM_SIZE = mem_size;

	/* Initial existiert genau ein mem_block direkt am Anfang des Speichers */
	mem_block * beginning = MEM;

	beginning->next = NULL;
	beginning->prev = NULL;

	/* Der verwaltete Speicher des initialen mem_blocks ist die des
	 * gesamten Speichers minus die Größe seiner eigenen Verwaltungsstruktur
	 * Da sowohl MEM_SIZE ein Vielfaches von 8 ist und sizeof(mem_block) auch
	 * mindestens ein vielfaches von 8 oder mehr ist, ist das LSB
	 * auch direkt 0 -> free.
	 */
	beginning->size = MEM_SIZE - sizeof(mem_block);

	/* Unser Zeiger für das Next Fit Verfahren */
	last_allocation = beginning;
}

/* +------------------------------------+ *
 * | Hier ist Platz für Hilfsfunktionen | *
 * +------------------------------------+ */

/* find smallest greater multiple of 8 for a given number x */
int round_up(int x) {

	return ((x + 7) & (-8));
}

/* if mem_block is used return 1, else return 0 */
int check_mem_block_status(mem_block *block) {

	// LSB of size equals 1 -> used
	if ((block->size & 1) == 1) {
		return 1;
	}
	// LSB of size equals 0 -> free
	return 0;
}

/* get mem_block size (minus the 3 LSB's) */
int get_mem_block_size(mem_block *block) {

	int remainder = block->size & 7;
	return (block->size - remainder);
}

/* set mem_block status to 1 (used) */
void set_mem_block_status_to_used(mem_block *block) {

	block->size++;
}

/* set mem_block status to 0 (free) */
void set_mem_block_status_to_free(mem_block *block) {

	block->size--;
}

/* if mem_block is fit case (a) return 1, else return 0 */
int check_fit_a(mem_block *block, int size) {

	if (check_mem_block_status(block) == 0 && get_mem_block_size(block) == size) {
		return 1;
	}
	return 0;
}

/* if mem_block is fit case (b) return 1, else return 0 */
int check_fit_b(mem_block *block, int size) {

	if (check_mem_block_status(block) == 0 && get_mem_block_size(block) > size) {
		return 1;
	}
	return 0;
}

/* if mem_block is fit case (c) return 1, else return 0 */
int check_fit_c(mem_block *block, int size) {

	if (check_mem_block_status(block) == 0 && get_mem_block_size(block) > size) {
		if ((get_mem_block_size(block) - ((int) sizeof(mem_block)) - 8) < size) {
			return 1;
		}
	}
	return 0;
}

/* initialize memory with specified value */
void *init_memory(mem_block *block, int size, int c) {

	return memset(((void *) block) + sizeof(mem_block), c, size);
}

/* merges two mem_blocks and returns a pointer to the first mem_block */
mem_block *merge(mem_block *first, mem_block *second) {

	first->size += (get_mem_block_size(second) + sizeof(mem_block));
	first->next = second->next;
	if (second->next != NULL) {
		second->next->prev = first;
	}
	if (check_mem_block_status(first) == 1) {
		set_mem_block_status_to_free(first);
	}
	return first;
}

/* returns first mem_block (starts iterating backwards from current) */
mem_block *get_first_mem_block(mem_block *current) {

	while (current->prev != NULL) {
		current = current->prev;
	}
	return current;
}

/* -------------------------------------- */

void *my_calloc(size_t nmemb, size_t size, int c) {

	if ((nmemb * size) == 0) {
		return NULL;
	}

	int new_size = round_up(nmemb * size); // calculate size to allocate
	mem_block *tmp = last_allocation;
	int skip_first = 1; // if equal to 1 while loop also executes

	// iterate over list (from last_allocation to last_allocation-1)
	while (tmp != last_allocation || skip_first == 1) {
		skip_first = 0; // from now on check if (tmp != last_allocation)
		if (tmp == NULL) {
			break;
		}

		if (check_fit_a(tmp, new_size) == 1) { // fit case (a)
			set_mem_block_status_to_used(tmp);
			last_allocation = tmp;
			return init_memory(tmp, new_size, c);
		}
		if (check_fit_c(tmp, new_size) == 1) { // fit case (c)
			set_mem_block_status_to_used(tmp);
			last_allocation = tmp;
			return init_memory(tmp, new_size, c);
		}
		if (check_fit_b(tmp, new_size) == 1) { // fit case (b)
			int remaining_size = get_mem_block_size(tmp) - new_size;
			// create new mem_block:
			mem_block *new = ((void *) tmp) + sizeof(mem_block) + new_size;
			new->prev = tmp;
			new->next = tmp->next;
			new->size = remaining_size - sizeof(mem_block);
			if (new->next != NULL) {
				new->next->prev = new;
			}
			// update old mem_block:
			tmp->next = new;
			tmp->size = new_size;
			set_mem_block_status_to_used(tmp);

			last_allocation = tmp;
			return init_memory(tmp, new_size, c);
		}

		mem_block *last = tmp;
		tmp = tmp->next;
		if (tmp == NULL) { // end of list reached -> start at first block in list
			tmp = get_first_mem_block(last);
		}
	}
	return NULL;
}

void my_free(void *ptr) {

	if (ptr == NULL) {
		return;
	}

	mem_block *block = ptr - sizeof(mem_block);
	mem_block *prev = block->prev;
	mem_block *next = block->next;

	// edge case (list contains only one block)
	if (prev == NULL && next == NULL) {
		set_mem_block_status_to_free(block);
		return;
	}

	// edge case (block is first)
	if (prev == NULL) {
		if (check_mem_block_status(next) == 1) { // next is used
			set_mem_block_status_to_free(block);
			return;
		} else { // next is free
			merge(block, next);
			if (last_allocation == next) {
				last_allocation = block;
			}
			return;
		}
	}

	// edge case (block is last)
	if (next == NULL) {
		if (check_mem_block_status(prev) == 1) { // prev is used
			set_mem_block_status_to_free(block);
			return;
		} else { // prev is free
			merge(prev, block);
			if (last_allocation == block) {
				last_allocation = prev;
			}
			return;
		}
	}

	// case 1 (prev is used, next is used)
	if (check_mem_block_status(prev) == 1 && check_mem_block_status(next) == 1) {
		set_mem_block_status_to_free(block);
		return;
	}

	// case 2.1 (prev is used, next is free)
	if (check_mem_block_status(prev) == 1 && check_mem_block_status(next) == 0) {
		merge(block, next);
		if (last_allocation == next) {
			last_allocation = block;
		}
		return;
	}

	// case 2.2 (prev is free, next is used)
	if (check_mem_block_status(prev) == 0 && check_mem_block_status(next) == 1) {
		merge(prev, block);
		if (last_allocation == block) {
			last_allocation = prev;
		}
		return;
	}

	// case 3 (prev is free, next is free)
	if (check_mem_block_status(prev) == 0 && check_mem_block_status(next) == 0) {
		merge(prev, block);
		if (last_allocation == block) {
			last_allocation = prev;
		}
		merge(prev, next);
		if (last_allocation == next) {
			last_allocation = prev;
		}
		return;
	}
}
