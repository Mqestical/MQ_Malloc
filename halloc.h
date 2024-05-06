#include <stdint.h>

#define FAILURE -0x1

/*INPUT_INFORMATION:*/
/*UTF-8 LF C { POSIX : LINUX } */

struct PAGE_INFORMATION {
	int pages;
	uint32_t page_size;
	char page_data[]; // flexible array at the end of the struct.
};


extern int MEM_init(uint64_t size);
extern void* halloc(uint64_t size) __THROW __wur;
extern int hfree(void* ptr); __THROW;
