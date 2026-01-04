#include "allocator.h"
#include <cstring>
#include <iostream>
#include <iomanip>


// The heap buffer - allocated at program start
static char heap_buffer[HEAP_SIZE];
static BlockHeader* free_list = nullptr;

// Helper function to align size up to ALIGN_SIZE boundary
static size_t align_size(size_t size) {
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
}

// Initialize the allocator
// Sets up the entire heap as one large free block
void init_allocator() {
    // Clear the heap buffer
    std::memset(heap_buffer, 0, HEAP_SIZE);
    
    // Create initial free block covering the entire heap
    BlockHeader* initial_block = reinterpret_cast<BlockHeader*>(heap_buffer);
    initial_block->size = HEAP_SIZE;
    initial_block->is_free = true;
    initial_block->next = nullptr;
    
    // Initialize free list
    free_list = initial_block;
}

// Check if a pointer is within the heap bounds
static bool is_valid_ptr(void* ptr) {
    char* char_ptr = reinterpret_cast<char*>(ptr);
    return (char_ptr >= heap_buffer && char_ptr < heap_buffer + HEAP_SIZE);
}

// Get the minimum size needed for a block (header + minimum user data)
static size_t get_min_block_size() {
    return align_size(sizeof(BlockHeader) + 1);
}

// Split a free block if it's large enough
// Returns pointer to the allocated block, or nullptr if block can't be split
static BlockHeader* split_block(BlockHeader* block, size_t requested_size) {
    size_t aligned_size = align_size(requested_size);
    size_t total_size = sizeof(BlockHeader) + aligned_size;
    
    // Check if block is large enough to split
    size_t min_remaining = get_min_block_size();
    if (block->size < total_size + min_remaining) {
        // Not large enough to split, use entire block
        return block;
    }
    
    // Split the block
    char* block_start = reinterpret_cast<char*>(block);
    BlockHeader* new_block = reinterpret_cast<BlockHeader*>(block_start + total_size);
    
    // Setup new free block (the remainder)
    new_block->size = block->size - total_size;
    new_block->is_free = true;
    
    // Update original block size
    block->size = total_size;
    
    // Insert new block into free list (after current block)
    new_block->next = block->next;
    block->next = new_block;
    
    return block;
}

// Remove a block from the free list
static void remove_from_free_list(BlockHeader* block) {
    if (!free_list) {
        return;
    }
    
    // If it's the first block
    if (free_list == block) {
        free_list = block->next;
        return;
    }
    
    // Find the block before it in the free list
    BlockHeader* current = free_list;
    while (current && current->next != block) {
        current = current->next;
    }
    
    if (current) {
        current->next = block->next;
    }
}

// Coalesce a freed block with adjacent free blocks
// Returns the block that should be inserted into the free list
// Returns nullptr if the block was merged into a previous block (already in free list)
static BlockHeader* coalesce_block(BlockHeader* block) {
    char* heap_start = heap_buffer;
    char* heap_end = heap_buffer + HEAP_SIZE;
    char* block_start = reinterpret_cast<char*>(block);
    char* block_end = block_start + block->size;
    
    // Check if next block exists and is free
    if (block_end < heap_end) {
        BlockHeader* next_block = reinterpret_cast<BlockHeader*>(block_end);
        if (next_block->is_free) {
            // Remove next block from free list
            remove_from_free_list(next_block);
            
            // Merge next block into current block
            block->size += next_block->size;
        }
    }
    
    // Check if previous block exists and is free
    // We need to traverse from the start to find previous block
    char* prev_block_end = heap_start;
    BlockHeader* prev_block = nullptr;
    
    // Find the block before this one
    while (prev_block_end < block_start) {
        BlockHeader* current = reinterpret_cast<BlockHeader*>(prev_block_end);
        char* current_end = prev_block_end + current->size;
        
        if (current_end == block_start && current->is_free) {
            prev_block = current;
            break;
        }
        
        if (current_end >= block_start) {
            break;
        }
        
        prev_block_end = current_end;
    }
    
    // If previous block is free, merge current into it
    if (prev_block) {
        // Note: block is not in free list yet (we just freed it)
        // So we just merge it into prev_block
        prev_block->size += block->size;
        // prev_block is already in free list, so return nullptr
        return nullptr;
    }
    
    // Block was not merged into previous block, so it needs to be inserted
    return block;
}

// Allocate memory
void* my_malloc(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    
    // Calculate required size (header + aligned user data)
    size_t aligned_size = align_size(size);
    size_t total_size = sizeof(BlockHeader) + aligned_size;
    
    // Search free list for a block large enough
    BlockHeader* current = free_list;
    BlockHeader* prev = nullptr;
    
    while (current) {
        if (current->size >= total_size) {
            // Found a block large enough
            BlockHeader* block_to_use = split_block(current, size);
            
            // Remove from free list
            if (block_to_use == free_list) {
                free_list = block_to_use->next;
            } else {
                if (prev) {
                    prev->next = block_to_use->next;
                }
            }
            
            // Mark as allocated
            block_to_use->is_free = false;
            block_to_use->next = nullptr;
            
            // Return pointer to user data
            return block_to_use->get_data();
        }
        
        prev = current;
        current = current->next;
    }
    
    // No suitable block found
    return nullptr;
}

// Free memory
void my_free(void* ptr) {
    if (!ptr) {
        return;  // Freeing nullptr is safe (like standard free)
    }
    
    // Get block header
    BlockHeader* block = BlockHeader::get_header(ptr);
    
    // Validate pointer
    if (!is_valid_ptr(block)) {
        std::cerr << "ERROR: Invalid pointer passed to my_free (not in heap)\n";
        return;
    }
    
    // Check for double free
    if (block->is_free) {
        std::cerr << "ERROR: Double free detected\n";
        return;
    }
    
    // Mark as free
    block->is_free = true;
    block->next = nullptr;
    
    // Coalesce with adjacent free blocks
    // Returns the block to insert, or nullptr if merged into previous block (already in free list)
    BlockHeader* block_to_insert = coalesce_block(block);
    
    // Insert into free list if needed (at the beginning for simplicity)
    if (block_to_insert) {
        block_to_insert->next = free_list;
        free_list = block_to_insert;
    }
}

// Get used memory in bytes
size_t get_used_memory() {
    size_t used = 0;
    char* current = heap_buffer;
    char* heap_end = heap_buffer + HEAP_SIZE;
    
    while (current < heap_end) {
        BlockHeader* block = reinterpret_cast<BlockHeader*>(current);
        if (!block->is_free) {
            used += (block->size - sizeof(BlockHeader));
        }
        current += block->size;
    }
    
    return used;
}

// Get free memory in bytes
size_t get_free_memory() {
    size_t free = 0;
    BlockHeader* current = free_list;
    
    while (current) {
        free += (current->size - sizeof(BlockHeader));
        current = current->next;
    }
    
    return free;
}

// Get fragmentation count (number of free blocks)
size_t get_fragmentation_count() {
    size_t count = 0;
    BlockHeader* current = free_list;
    
    while (current) {
        count++;
        current = current->next;
    }
    
    return count;
}

// Print heap state for debugging
void print_heap_state() {
    std::cout << "\n=== Heap State ===\n";
    std::cout << "Heap Size: " << HEAP_SIZE << " bytes (" << (HEAP_SIZE / 1024.0) << " KB)\n";
    std::cout << "Used Memory: " << get_used_memory() << " bytes\n";
    std::cout << "Free Memory: " << get_free_memory() << " bytes\n";
    std::cout << "Fragmentation: " << get_fragmentation_count() << " free blocks\n";
    std::cout << "\nBlock Layout:\n";
    std::cout << std::left << std::setw(12) << "Address" 
              << std::setw(12) << "Size" 
              << std::setw(12) << "User Size"
              << std::setw(10) << "Status"
              << "\n";
    std::cout << std::string(50, '-') << "\n";
    
    char* current = heap_buffer;
    char* heap_end = heap_buffer + HEAP_SIZE;
    size_t block_num = 0;
    
    while (current < heap_end) {
        BlockHeader* block = reinterpret_cast<BlockHeader*>(current);
        size_t user_size = block->size - sizeof(BlockHeader);
        
        std::cout << std::hex << std::setw(12) << reinterpret_cast<void*>(current)
                  << std::dec << std::setw(12) << block->size
                  << std::setw(12) << user_size
                  << std::setw(10) << (block->is_free ? "FREE" : "USED")
                  << "\n";
        
        current += block->size;
        block_num++;
        
        // Safety check to prevent infinite loop
        if (block->size == 0 || block_num > 10000) {
            std::cerr << "ERROR: Corrupted heap or infinite loop detected\n";
            break;
        }
    }
    
    std::cout << "\nFree List:\n";
    BlockHeader* free_block = free_list;
    size_t free_num = 0;
    while (free_block) {
        std::cout << "  [" << free_num << "] " 
                  << std::hex << reinterpret_cast<void*>(free_block)
                  << std::dec << " -> size: " << free_block->size << " bytes\n";
        free_block = free_block->next;
        free_num++;
    }
    
    if (free_num == 0) {
        std::cout << "  (empty)\n";
    }
    
    std::cout << "\n";
}

