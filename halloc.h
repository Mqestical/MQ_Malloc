#include <stdint.h>

struct PAGE_INFORMATION {
	int pages;
	uint32_t page_size;
	char page_data[]; // flexible array at the end of the struct.
};


int MEM_init(uint64_t sizeofRegion); __THROW;
extern void* halloc(uint64_t size) __THROW __wur;
