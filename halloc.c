#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>



struct PAGE_INFORMATION *page_info = NULL;
int initialized = 0x0; // global int for checking meminit 
void* next_available_address = NULL; // Global variable to track next available memory address



// MEM_INIT: Can only be called once, it is the INTERNAL OF HALLOC.
 int MEM_init(int64_t sizeofRegion)  {
   
if (sizeofRegion <= 0x0) { fprintf(stderr, "\nfatal error: SizeOfRegion cannot be less than 0! {fatal_error_less_than_0}\n\n\n");
   return -0x1;
   }

   if (initialized != 0x0) {
      fprintf(stderr, "\nfatal error: mem_init can only be called once! \n\n\n");
      return -1;
   }


    //get the pagesize
	size_t page_size = sysconf(_SC_PAGESIZE); 
   int num_pages = (sizeofRegion + page_size - 0x1) / page_size; // round it up.
   // calculate the required memory region to be allocated via mmap.
  size_t info_size = sizeof(struct PAGE_INFORMATION) + sizeof(char) * num_pages * page_size;
   page_info = mmap(NULL, info_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -0x1, 0x0); // call mmap system call
   
   if (page_info == MAP_FAILED) { perror("mmap"); exit(-0x1); }
   // copy values
   page_info->pages = num_pages;
   page_info->page_size = page_size;
   /* increment initialized, it is at the end of the function because I won't have to make it equal to 0 for error handlings,
      saves time tbh.
   */
	initialized++;
   printf("Initialized with %d pages of size %d\n", page_info->pages, page_info->page_size);
   next_available_address = page_info->page_data; // Set next_available_address to point to the start of the allocated memory region
   printf("Next available address: %p\n", next_available_address); // Debug print
   return 0x0;
}

void* halloc(uint64_t size) {
   if (!initialized) {
      fprintf(stderr, "fatal error: mem_init must be called first!");
      exit(-0x1);
   }

   if (size <= 0) {
      fprintf(stderr, "error: size must be a positive number that is equal to ONE or more.");
      exit(EXIT_FAILURE);    
   }

   // Check if there is enough space available
   size_t available_space = (page_info->pages * page_info->page_size) - ((size_t)next_available_address - (size_t)page_info);

   if (size > available_space) {
      fprintf(stderr, "error: Not enough memory available.");
      exit(EXIT_FAILURE);    
   }

   // Allocate memory from the next available address
   void* allocated_memory = next_available_address;
   next_available_address = (void*)((size_t)next_available_address + size);

   // Ensure next_available_address is aligned properly
   next_available_address = (void*)(((size_t)next_available_address + 7) & ~7);

   printf("Allocated memory at address: %p\n", allocated_memory); // Debug print

   return allocated_memory;
}

void free_internal(struct PAGE_INFORMATION* header) {
	if (header == NULL) {
		return;
	}
	const size_t size = header -> pages * header -> page_size;
	int res = munmap(header, size);
	if (res == FAILURE) {
		perror("fatal error: cannot free memory!");
	}
} 


int hfree(void* ptr) {
    if (ptr != NULL) {
        // Cast the pointer to the header type
        struct PAGE_INFORMATION* header = (struct PAGE_INFORMATION*)((size_t)ptr - sizeof(struct PAGE_INFORMATION));
        // Call the internal free function
        free_internal(header);
        return 0x0;
    }
}
