#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

// Alignment requirement (8 bytes)
const size_t ALIGN_SIZE = 8;

// Heap size (1MB)
const size_t HEAP_SIZE = 1024 * 1024;

// Block header structure
// This header is stored before each memory block in the heap
struct BlockHeader {
    size_t size;           // Total size of block (including header)
    bool is_free;          // Whether this block is free
    BlockHeader* next;     // Next block in free list (only valid if is_free == true)
    
    // Get pointer to user data (after header)
    void* get_data() {
        return reinterpret_cast<void*>(this + 1);
    }
    
    // Get pointer to header from user data pointer
    static BlockHeader* get_header(void* ptr) {
        return reinterpret_cast<BlockHeader*>(ptr) - 1;
    }
};

// Allocator functions
void init_allocator();
void* my_malloc(size_t size);
void my_free(void* ptr);

// Debugging utilities
void print_heap_state();
size_t get_used_memory();
size_t get_free_memory();
size_t get_fragmentation_count();

#endif // ALLOCATOR_H


