#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>


// CODE COMMENTED WITH X AFTER MEANS ITS TAKEN FROM AN EXTERNAL SOURCE!


 int MEM_init(uint64_t size)  {
    


    //get the pagesize
	size_t page_size = sysconf(_SC_PAGESIZE); 

	

   void *vptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 );

   if (vptr == MAP_FAILED) { perror("mmap"); exit(-1); }

	
}

void* halloc(uint64_t size) {

// TODO: Implement Halloc after its internal function.
}

void free(void* ptr) {
	// use its actual datatype later, Implement after halloc's internal&halloc.
}
