#include "allocator.h"
#include <iostream>
#include <cstring>
#include <cassert>

// Test function declarations
void test_basic_allocation();
void test_free_and_reuse();
void test_block_splitting();
void test_coalescing();
void test_edge_cases();
void test_fragmentation();
void demo_usage();

int main() {
    std::cout << "========================================\n";
    std::cout << "Custom Memory Allocator Demonstration\n";
    std::cout << "========================================\n\n";
    
    // Initialize the allocator
    init_allocator();
    std::cout << "Allocator initialized with " << (HEAP_SIZE / 1024) << " KB heap\n";
    print_heap_state();
    
    // Run tests
    test_basic_allocation();
    test_free_and_reuse();
    test_block_splitting();
    test_coalescing();
    test_edge_cases();
    test_fragmentation();
    demo_usage();
    
    std::cout << "\n========================================\n";
    std::cout << "All tests completed!\n";
    std::cout << "========================================\n";
    
    return 0;
}

// Test 1: Basic allocation
void test_basic_allocation() {
    std::cout << "\n>>> Test 1: Basic Allocation\n";
    std::cout << "Allocating blocks of different sizes...\n";
    
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(200);
    void* ptr3 = my_malloc(50);
    
    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    assert(ptr3 != nullptr);
    
    std::cout << "Allocated 3 blocks: " << ptr1 << ", " << ptr2 << ", " << ptr3 << "\n";
    
    // Write to allocated memory to verify it works
    std::strcpy(static_cast<char*>(ptr1), "Hello");
    std::strcpy(static_cast<char*>(ptr2), "World");
    std::strcpy(static_cast<char*>(ptr3), "Test");
    
    std::cout << "Written data: \"" << static_cast<char*>(ptr1) << "\", \""
              << static_cast<char*>(ptr2) << "\", \"" << static_cast<char*>(ptr3) << "\"\n";
    
    print_heap_state();
}

// Test 2: Free and reuse
void test_free_and_reuse() {
    std::cout << "\n>>> Test 2: Free and Reuse\n";
    std::cout << "Freeing the middle block and reallocating...\n";
    
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(200);
    void* ptr3 = my_malloc(50);
    
    std::cout << "Before free: ptr2 = " << ptr2 << "\n";
    my_free(ptr2);
    std::cout << "Freed ptr2\n";
    print_heap_state();
    
    // Allocate again - should reuse freed memory
    void* ptr4 = my_malloc(150);
    std::cout << "Allocated new block: ptr4 = " << ptr4 << "\n";
    
    // ptr4 might be the same as ptr2 (reused), or might be different depending on allocation strategy
    assert(ptr4 != nullptr);
    
    std::strcpy(static_cast<char*>(ptr4), "Reused");
    std::cout << "Written to reused block: \"" << static_cast<char*>(ptr4) << "\"\n";
    
    my_free(ptr1);
    my_free(ptr3);
    my_free(ptr4);
    
    print_heap_state();
}

// Test 3: Block splitting
void test_block_splitting() {
    std::cout << "\n>>> Test 3: Block Splitting\n";
    std::cout << "Allocating a large block, then freeing and allocating smaller blocks...\n";
    
    // Allocate a large block
    void* large = my_malloc(5000);
    assert(large != nullptr);
    std::cout << "Allocated large block: " << large << "\n";
    print_heap_state();
    
    // Free it
    my_free(large);
    std::cout << "Freed large block\n";
    print_heap_state();
    
    // Allocate smaller blocks - should split the large free block
    void* small1 = my_malloc(100);
    void* small2 = my_malloc(200);
    void* small3 = my_malloc(300);
    
    assert(small1 != nullptr);
    assert(small2 != nullptr);
    assert(small3 != nullptr);
    
    std::cout << "Allocated 3 smaller blocks from split:\n";
    std::cout << "  small1 = " << small1 << "\n";
    std::cout << "  small2 = " << small2 << "\n";
    std::cout << "  small3 = " << small3 << "\n";
    
    print_heap_state();
    
    my_free(small1);
    my_free(small2);
    my_free(small3);
}

// Test 4: Coalescing
void test_coalescing() {
    std::cout << "\n>>> Test 4: Block Coalescing\n";
    std::cout << "Allocating multiple blocks, freeing them in order to test coalescing...\n";
    
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(200);
    void* ptr3 = my_malloc(150);
    void* ptr4 = my_malloc(250);
    
    std::cout << "Allocated 4 blocks\n";
    print_heap_state();
    
    // Free blocks 2 and 4 (non-adjacent)
    std::cout << "Freeing ptr2 (middle block)...\n";
    my_free(ptr2);
    print_heap_state();
    
    std::cout << "Freeing ptr4 (last block)...\n";
    my_free(ptr4);
    print_heap_state();
    
    // Free block 1 - should coalesce with block 2
    std::cout << "Freeing ptr1 (first block) - should coalesce with ptr2...\n";
    my_free(ptr1);
    print_heap_state();
    
    // Free block 3 - should coalesce with blocks 1+2 and 4
    std::cout << "Freeing ptr3 - should coalesce all free blocks...\n";
    my_free(ptr3);
    print_heap_state();
    
    std::cout << "Expected: One large free block (heap should be mostly empty)\n";
}

// Test 5: Edge cases
void test_edge_cases() {
    std::cout << "\n>>> Test 5: Edge Cases\n";
    
    // Test malloc(0)
    std::cout << "Testing malloc(0)...\n";
    void* zero_ptr = my_malloc(0);
    assert(zero_ptr == nullptr);
    std::cout << "malloc(0) correctly returned nullptr\n";
    
    // Test free(nullptr)
    std::cout << "Testing free(nullptr)...\n";
    my_free(nullptr);
    std::cout << "free(nullptr) handled safely\n";
    
    // Test double free
    std::cout << "Testing double free detection...\n";
    void* test_ptr = my_malloc(100);
    my_free(test_ptr);
    std::cout << "Freed once, attempting double free...\n";
    my_free(test_ptr);  // Should print error message
    
    // Test invalid pointer
    std::cout << "Testing invalid pointer detection...\n";
    int stack_var = 42;
    my_free(&stack_var);  // Should print error message
    
    print_heap_state();
}

// Test 6: Fragmentation
void test_fragmentation() {
    std::cout << "\n>>> Test 6: Fragmentation Analysis\n";
    std::cout << "Creating fragmentation by allocating and freeing blocks in a pattern...\n";
    
    void* blocks[10];
    
    // Allocate 10 blocks
    for (int i = 0; i < 10; i++) {
        blocks[i] = my_malloc(100 + i * 50);
        assert(blocks[i] != nullptr);
    }
    
    std::cout << "Allocated 10 blocks\n";
    print_heap_state();
    
    // Free every other block
    std::cout << "Freeing blocks 1, 3, 5, 7, 9...\n";
    for (int i = 1; i < 10; i += 2) {
        my_free(blocks[i]);
    }
    
    print_heap_state();
    std::cout << "Fragmentation count: " << get_fragmentation_count() << "\n";
    
    // Free remaining blocks
    for (int i = 0; i < 10; i += 2) {
        my_free(blocks[i]);
    }
    
    print_heap_state();
}

// Demonstration: Real-world usage pattern
void demo_usage() {
    std::cout << "\n>>> Demonstration: Real-World Usage Pattern\n";
    std::cout << "Simulating typical allocation patterns...\n";
    
    // Simulate allocating strings
    char* str1 = static_cast<char*>(my_malloc(50));
    char* str2 = static_cast<char*>(my_malloc(100));
    char* str3 = static_cast<char*>(my_malloc(75));
    
    std::strcpy(str1, "First string");
    std::strcpy(str2, "Second string that is longer");
    std::strcpy(str3, "Third string");
    
    std::cout << "Strings allocated:\n";
    std::cout << "  1: \"" << str1 << "\"\n";
    std::cout << "  2: \"" << str2 << "\"\n";
    std::cout << "  3: \"" << str3 << "\"\n";
    
    print_heap_state();
    
    // Free middle one, allocate new
    my_free(str2);
    char* str4 = static_cast<char*>(my_malloc(80));
    std::strcpy(str4, "New string in reused memory");
    std::cout << "\nFreed str2, allocated str4: \"" << str4 << "\"\n";
    
    print_heap_state();
    
    // Clean up
    my_free(str1);
    my_free(str3);
    my_free(str4);
    
    print_heap_state();
    std::cout << "All blocks freed - heap should be one large free block\n";
}


