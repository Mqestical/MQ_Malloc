#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>



atomic_flag allocator_spinlock = ATOMIC_FLAG_INIT;
struct PAGE_INFORMATION *page_info = NULL;
void* next_available_address = NULL; // Global variable to track next available memory address

void acquire_spinlock() {
    while(atomic_flag_test_and_set(&allocator_spinlock)) {
        // spin, and spin, and spin until the lock is acquired.
    }
}

void release_spinlock() {
    atomic_flag_clear(&allocator_spinlock);
}

// MEM_INIT: Can only be called once, it is the INTERNAL OF HALLOC.
int MEM_init(int64_t sizeofRegion) {
    if (sizeofRegion <= 0x0) {
        fprintf(stderr, "\nfatal error: SizeOfRegion cannot be less than or equal to 0!\n\n");
        exit(FAILURE);
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    int num_pages = (sizeofRegion + page_size - 0x1) / page_size;
    size_t info_size = sizeof(struct PAGE_INFORMATION) + sizeof(char) * num_pages * page_size;
    page_info = mmap(NULL, info_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -0x1, 0x0);

    if (page_info == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }

    page_info->pages = num_pages;
    page_info->page_size = page_size;
    next_available_address = page_info->page_data;
    printf("Initialized with %d pages of size %d\n", page_info->pages, page_info->page_size);
    next_available_address = page_info->page_data; // Set next_available_address to point to the start of the allocated memory region
    printf("Next available address: %p\n", next_available_address); // Debug print
    return 0x0;
}

void* halloc(int64_t size) {
    acquire_spinlock();

    if (size <= 0x0) {
        fprintf(stderr, "error: size must be a positive number.\n");
        exit(FAILURE);
    }

    size_t available_space = (page_info->pages * page_info->page_size) - ((size_t)next_available_address - (size_t)page_info);

    if (size + 2 * TAG_SIZE > available_space) { // Include space for tags
        fprintf(stderr, "error: Not enough memory available.\n");
        exit(FAILURE);
    }

    void* allocated_memory = next_available_address;

    
    struct BlockTag* start_tag = (struct BlockTag*)allocated_memory;
    start_tag->tag_start = 'A';

    
    struct BlockTag* end_tag = (struct BlockTag*)((char*)allocated_memory + size + TAG_SIZE);
    end_tag->tag_end = 'B';

    
    next_available_address = (void*)((char*)next_available_address + size + 2 * TAG_SIZE);

    
    next_available_address = (void*)(((size_t)next_available_address + 0x7) & ~0x7);

    release_spinlock();
    return (char*)allocated_memory + TAG_SIZE; 
}

void free_internal(struct PAGE_INFORMATION* header) {
    if (header == NULL) {
        return;
    }
    const size_t size = header->pages * header->page_size;
    int res = munmap(header, size);
    if (res == -1) {
        perror("fatal error: cannot free memory!\n");
    }
}

int hfree(void* ptr) {
    acquire_spinlock();

    if (ptr != NULL) {
        
        struct BlockTag* start_tag = (struct BlockTag*)((char*)ptr - TAG_SIZE);

        
        if (start_tag->tag_start != 'A') {
            fprintf(stderr, "error: Invalid start tag detected!\n");
            exit(FAILURE);
        }

        
        struct BlockTag* end_tag = (struct BlockTag*)((char*)ptr + sizeof(ptr));

        
        if (end_tag->tag_end != 'B') {
            fprintf(stderr, "error: Invalid end tag detected!\n");
            exit(FAILURE);
        }

        
        struct PAGE_INFORMATION* header = (struct PAGE_INFORMATION*)((size_t)ptr - sizeof(struct PAGE_INFORMATION));
        free_internal(header);
    } else {
        fprintf(stderr, "fatal error: pointer is null!\n");
        exit(FAILURE);
    }

    release_spinlock();
    return 0x0;
}
