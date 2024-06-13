#include <stdio.h>
#include <stdlib.h>
#include "halloc.h"
#include <time.h>

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Number of iterations
    int iterations = 1000000;

    // Allocate memory with random sizes
    for (int i = 0; i < iterations; i++) {
        // Generate a random size between 1 byte and 1024 bytes (1 KiB)
        size_t size = (size_t)(rand() % 4096) + 1;
        
        // Allocate memory
        MEM_init(size*2);
        void* ptr = halloc(size);
        
        // Check if memory allocation was successful
        if (ptr == NULL) {
            fprintf(stderr, "\nMemory allocation failed\n");
            return EXIT_FAILURE;
        }
        
        // Print the size of allocated memory
        printf("\nAllocated %zu bytes of memory\n", size);
        printf("\naddress: %p\n", &ptr);
        
        // Free the allocated memory
        hfree(ptr);
    }

    return EXIT_SUCCESS;
}
