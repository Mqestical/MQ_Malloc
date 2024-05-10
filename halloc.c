
/*  |===========================================================
    |                                                          |
    |                                                          |
    | Copyright (c) Muhammad Alhijab (Kingdom of Saudi Arabia) |
    | Linear Block Allocator                                                         |
    |                                                          |
    |==========================================================|
*/




#include "halloc.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>

/*ANYTHING THAT IS ASSOSCIATED WITH ROUNDING UP / FINDING ADDRESSES / CALCULATIONS IS FROM AN EXTERNAL SOURCE.*/




atomic_flag allocator_spinlock = ATOMIC_FLAG_INIT;
struct PAGE_INFORMATION *page_info = NULL;
void* next_available_address = NULL;





struct MemoryBlock allocation_table[MAX_BLOCKS];
int num_allocated_blocks = 0;

void acquire_spinlock() {
    while(atomic_flag_test_and_set(&allocator_spinlock)) {
        // spin, and spin, and spin until the lock is acquired.
    }
}

void release_spinlock() {
    atomic_flag_clear(&allocator_spinlock);
}


// from an external source
void add_to_allocation_table(void* address, size_t size) {
    if (num_allocated_blocks < MAX_BLOCKS) {
        allocation_table[num_allocated_blocks].address = address;
        allocation_table[num_allocated_blocks].size = size;
        num_allocated_blocks++;
    } else {
        fprintf(stderr, "fatal error: Maximum number of allocated blocks reached!\n");
        exit(EXIT_FAILURE);
    }
}

// from an external source
void remove_from_allocation_table(void* address) {
    for (int i = 0; i < num_allocated_blocks; ++i) {
        if (allocation_table[i].address == address) {
            
            for (int j = i; j < num_allocated_blocks - 1; ++j) {
                allocation_table[j] = allocation_table[j + 1];
            }
            num_allocated_blocks--;
            return;
        }
    }
}

int MEM_init(int64_t sizeofRegion) {
    if (sizeofRegion <= 0) {
        fprintf(stderr, "\nfatal error: SizeOfRegion cannot be less than or equal to 0!\n\n");
        exit(EXIT_FAILURE);
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    int num_pages = (sizeofRegion + page_size - 1) / page_size;
    size_t info_size = sizeof(struct PAGE_INFORMATION) + sizeof(char) * num_pages * page_size;
    page_info = mmap(NULL, info_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (page_info == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    page_info->pages = num_pages;
    page_info->page_size = page_size;
    next_available_address = page_info->page_data;
    printf("Initialized with %d pages of size %zu\n", page_info->pages, page_info->page_size);
    next_available_address = page_info->page_data;
    printf("Next available address: %p\n", next_available_address);
    return 0;
}

void* halloc(int64_t size) {
    acquire_spinlock();

    if (size <= 0) {
        fprintf(stderr, "error: size must be a positive number.\n");
        exit(EXIT_FAILURE);
    }

    size_t available_space = (page_info->pages * page_info->page_size) - ((size_t)next_available_address - (size_t)page_info);

    if (size + 2 * TAG_SIZE > available_space) { 
        fprintf(stderr, "fatal error: not enough memory available.\n");
        exit(EXIT_FAILURE);
    }

    void* allocated_memory = next_available_address;

    size_t* size_ptr = (size_t*)allocated_memory;
    *size_ptr = size;

    struct BlockTag* start_tag = (struct BlockTag*)next_available_address;
    start_tag->tag_start = 'A';

    next_available_address = (void*)((char*)next_available_address + sizeof(size_t) + TAG_SIZE);

    struct BlockTag* end_tag = (struct BlockTag*)((char*)allocated_memory + sizeof(size_t) + size + TAG_SIZE);
    end_tag->tag_end = 'B';

    next_available_address = (void*)((char*)next_available_address + size + TAG_SIZE);

    /*align the next available address to 0x7&~0x7 (8-byte aligned addr)*/
    next_available_address = (void*)(((size_t)next_available_address + 7) & ~7);

    
    add_to_allocation_table(allocated_memory, size);

    release_spinlock();
    return (char*)allocated_memory + sizeof(size_t);
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
        
        uintptr_t page_start = ((uintptr_t)ptr & ~(page_info->page_size - 1));
        struct PAGE_INFORMATION* header = (struct PAGE_INFORMATION*)((uintptr_t)ptr - sizeof(struct PAGE_INFORMATION));


    
        uintptr_t block_start = (uintptr_t)header->page_data;
        uintptr_t block_end = block_start + header->pages * header->page_size;
        while (block_start < block_end) {
            
            if ((uintptr_t)ptr >= block_start && (uintptr_t)ptr < block_start + header->page_size) {
                
                struct BlockTag* start_tag = (struct BlockTag*)(block_start + sizeof(size_t));

                
                if (start_tag->tag_start != 'A') {
                    fprintf(stderr, "\n\nfatal error: Heap Corrupted! (FATAL_ERROR_START_TAG_NOT_AVAILABLE)\n\n");
                    exit(EXIT_FAILURE);
                }

                
                struct BlockTag* end_tag = (struct BlockTag*)(block_start + sizeof(size_t) + *(size_t*)block_start + TAG_SIZE);

                
                if (end_tag->tag_end != 'B') {
                    fprintf(stderr, "fatal error: Heap Corrupted! (FATAL_ERROR_END_TAG_NOT_AVAILABLE)");
                    exit(EXIT_FAILURE);
                }

                
                remove_from_allocation_table((void*)block_start);

                
                free_internal(header);

                
                break;
            }

            
            block_start += header->page_size;
        }
    } else {
        fprintf(stderr, "fatal error: pointer is null!\n");
        exit(EXIT_FAILURE);
    }

    release_spinlock();
    return 0;
}
