#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

struct PAGE_INFORMATION *page_info = NULL;
int initialized = 0; // global int for checking meminit 

// CODE COMMENTED WITH X AFTER MEANS ITS TAKEN FROM AN EXTERNAL SOURCE!

// MEM_INIT: Can only be called once, it is the INTERNAL OF HALLOC.
 int MEM_init(uint64_t sizeofRegion)  {
    
   if (initialized != 0) {
      fprintf(stderr, "fatal error: mem_init can only be called once! ");
      return -1;
   }

    //get the pagesize
	size_t page_size = sysconf(_SC_PAGESIZE); 
   int num_pages = (sizeofRegion + page_size - 1) / page_size; // round it up.

  size_t info_size = sizeof(struct PAGE_INFORMATION) + sizeof(char) * num_pages * page_size;
   page_info = mmap(NULL, info_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
   
   if (page_info == MAP_FAILED) { perror("mmap"); exit(-1); }

   page_info->pages = num_pages;
   page_info->page_size = page_size;

	initialized++;
   return 0;
}

void* halloc(uint64_t size) {

   if (!initialized) 
      { fprintf(stderr, "fatal error: mem_init must be called first!"); exit(-1); }

      // brainstorm some allocation techniques here.

}

void free(void* ptr) {
	// use its actual datatype later, Implement after halloc's internal&halloc.
}
