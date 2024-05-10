#include <stdint.h>

#define FAILURE -0x1
#define SUCCESS 0x0
#define TAG_SIZE sizeof(struct BlockTag)
#define MAX_BLOCKS 1000

/*INPUT_INFORMATION:*/
/*UTF-8 LF C { POSIX : LINUX } */

struct PAGE_INFORMATION {
	int pages;
	uint32_t page_size;
	char page_data[]; // flexible array at the end of the struct.
};
	
struct BlockTag {
    char tag_start; // tag at the start of the block
    char tag_end;   // tag at the end of the block
};

struct MemoryBlock {
    void* address;
    size_t size;
};

extern int MEM_init(int64_t sizeofRegion);
extern void* halloc(int64_t size) __THROW __wur;
extern void free_internal(struct PAGE_INFORMATION* header);

extern int hfree(void* ptr);