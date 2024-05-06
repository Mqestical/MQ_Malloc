#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

/*INPUT_INFORMATION:*/
/*UTF-8 LF C { POSIX : LINUX } */
struct PAGE_INFORMATION *page_info = NULL;
int initialized = 0; // global int for checking meminit 
void* next_available_address = NULL; // Global variable to track next available memory address



// MEM_INIT: Can only be called once, it is the INTERNAL OF HALLOC.
 int MEM_init(uint64_t sizeofRegion)  {
    
   if (initialized != 0) {
      fprintf(stderr, "fatal error: mem_init can only be called once! ");
      exit(FAILURE);
   }

    //get the pagesize
	size_t page_size = sysconf(_SC_PAGESIZE); 
   int num_pages = (sizeofRegion + page_size - 1) / page_size; // round it up.
   // calculate the required memory region to be allocated via mmap.
  size_t info_size = sizeof(struct PAGE_INFORMATION) + sizeof(char) * num_pages * page_size;
   page_info = mmap(NULL, info_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0); // call mmap system call
   
   if (page_info == MAP_FAILED) { perror("mmap"); exit(FAILURE); }
   // copy values
   page_info->pages = num_pages;
   page_info->page_size = page_size;
   /* increment initialized, it is at the end of the function because I won't have to make it equal to 0 for error handlings,
      saves time tbh.
   */
	initialized++;
   return 0;
}

void* halloc(uint64_t size) {


   if (!initialized) 
      { fprintf(stderr, "fatal error: mem_init must be called first!"); exit(FAILURE); }

   if (size <= 0) {
      fprintf(stderr, "error: size must be a positive number that is equal to ONE or more. (unsigned_integer_64bits_halloc_fatal_error)");
      exit(FAILURE);    
   }

   // Check if there is enough space available
   if (next_available_address == NULL || (size_t)next_available_address + size > (size_t)page_info + page_info->pages * page_info->page_size) {
      fprintf(stderr, "error: out of memory!"); // Or handle this condition appropriately
      return NULL;
   }

   // Allocate memory from the next available address
   void* allocated_memory = next_available_address;
   next_available_address = (void*)((size_t)next_available_address + size);

   return allocated_memory;
}


int free(void* ptr) {
	// use its actual datatype later, Implement after halloc's internal&halloc.
	return 0x1;
}
