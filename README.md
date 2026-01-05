# Free-list C++ Memory Allocator

A complete, working implementation of a custom malloc/free-style memory allocator in C++. This is a fixed-size heap allocator designed for learning and demonstration purposes. This is NOT intended
for production. I have commented and created this readme in order for a beginner coder to be able to understand the concepts. I have also added Section 7 (Key Concepts Explained) for more clarity.

## Table of Contents

1. [Overview](#overview)
2. [File Structure and Explanations](#file-structure-and-explanations)
3. [How Memory Allocation Works (Conceptual Overview)](#how-memory-allocation-works-conceptual-overview)
4. [Detailed Code Walkthrough](#detailed-code-walkthrough)
5. [Compilation and Running](#compilation-and-running)
6. [Understanding the Output](#understanding-the-output)
7. [Key Concepts Explained](#key-concepts-explained)

---

## Overview

This project implements a free-list memory allocator that manages its own memory pool. Instead of using the system's `malloc()` and `free()`, this allocator:

- Reserves a 1MB block of memory at program start
- Tracks which parts are used and which are free
- Splits large free blocks when smaller allocations are needed
- Merges adjacent free blocks back together when memory is freed
- Provides debugging tools to visualize memory usage

**Key Features:**
- Fixed-size heap (1MB) allocated at program start
- Block-based allocation with metadata headers
- Pointer-based linked list for free block tracking
- Block splitting when allocating
- Block coalescing when freeing
- Double-free and invalid-pointer detection
- Comprehensive debugging utilities

---

## File Structure and Explanations

### `allocator.h` - Header File

**Purpose:** This file contains all the declarations (function signatures, structures, constants) that other files need to use the allocator.

**What's in it:**

1. **Constants:**
   - `ALIGN_SIZE = 8`: All memory allocations are rounded up to multiples of 8 bytes. This improves performance on modern processors.
   - `HEAP_SIZE = 1024 * 1024`: The total size of our memory pool (1 megabyte).

2. **BlockHeader Structure:**
   - This is the most important data structure in the allocator
   - Each block of memory has a header that stores metadata:
     - `size_t size`: The total size of this block (including the header itself)
     - `bool is_free`: Whether this block is currently free (true) or in use (false)
     - `BlockHeader* next`: A pointer to the next free block (only valid if `is_free == true`)
   - Helper functions:
     - `get_data()`: Returns a pointer to the user data (the part after the header)
     - `get_header()`: Given a pointer to user data, returns the pointer to the header

3. **Function Declarations:**
   - `init_allocator()`: Sets up the memory pool
   - `my_malloc(size_t size)`: Allocates memory (like malloc)
   - `my_free(void* ptr)`: Frees memory (like free)
   - Debugging functions: Print heap state, get memory statistics

### `allocator.cpp` - Implementation File

**Purpose:** This file contains the actual implementation of all the allocator functions declared in `allocator.h`.

**What's in it:**

1. **Global Variables:**
   - `heap_buffer[HEAP_SIZE]`: A static array that holds our entire 1MB memory pool
   - `free_list`: A pointer to the first block in a linked list of free blocks

2. **Helper Functions:**
   - `align_size()`: Rounds sizes up to 8-byte boundaries
   - `is_valid_ptr()`: Checks if a pointer points to memory in our heap
   - `get_min_block_size()`: Calculates the smallest block we can create
   - `split_block()`: Divides a large free block into two blocks
   - `remove_from_free_list()`: Removes a block from the linked list of free blocks
   - `coalesce_block()`: Merges adjacent free blocks together

3. **Main Functions:**
   - `init_allocator()`: Initializes the heap as one large free block
   - `my_malloc()`: Finds a free block, splits it if needed, marks it as used, returns pointer
   - `my_free()`: Marks block as free, merges with adjacent free blocks, adds to free list
   - Debugging functions: Traverse the heap and print statistics

### `main.cpp` - Test and Demonstration File

**Purpose:** This file contains test code that demonstrates how the allocator works and verifies it's functioning correctly.

**What's in it:**

1. **Test Functions:**
   - `test_basic_allocation()`: Allocates a few blocks and writes data to them
   - `test_free_and_reuse()`: Frees memory and shows it gets reused
   - `test_block_splitting()`: Shows how large blocks get split into smaller ones
   - `test_coalescing()`: Demonstrates adjacent free blocks merging together
   - `test_edge_cases()`: Tests error handling (double free, invalid pointers, etc.)
   - `test_fragmentation()`: Shows what fragmentation looks like
   - `demo_usage()`: Simulates real-world usage patterns

2. **Main Function:**
   - Calls `init_allocator()` to set up memory
   - Runs all the test functions in sequence
   - Prints heap state after each test

---

## How Memory Allocation Works (Conceptual Overview)

Before diving into code, let's make sure we're on the same page about certain:

### The Memory Layout

Think of the heap as a long, continuous strip of memory:

```
[Header|Data][Header|Data][Header|Data]...
```

Each block has two parts:
1. **Header**: Stores metadata (size, free flag, next pointer) - about 24 bytes
2. **Data**: The actual memory the user requested

### Initial State

When the program starts:
```
[One huge block: Header + 1MB of free space]
```

The entire heap is one big free block.

### When You Call my_malloc(100)

1. **Find a free block**: Search the free list for a block big enough (100 bytes + header)
2. **Split if needed**: If the block is much larger than needed, split it:
   ```
   [Large free block] → [Block for 100 bytes][Remaining free block]
   ```
3. **Mark as used**: Set `is_free = false` on the allocated block
4. **Remove from free list**: Take it out of the linked list of free blocks
5. **Return pointer**: Return a pointer to the data area (after the header)

### When You Call my_free(ptr)

1. **Get the header**: Convert the user pointer back to the block header
2. **Validate**: Check it's a valid pointer and not already free
3. **Mark as free**: Set `is_free = true`
4. **Coalesce**: Check if adjacent blocks are free and merge them:
   ```
   [Free][Free][Free] → [One big free block]
   ```
5. **Add to free list**: Insert the (possibly merged) block into the free list

### The Free List

The free list is a linked list that connects all free blocks:

```
free_list → [Block A] → [Block C] → [Block E] → nullptr
```

Note: Blocks B and D are used, so they're not in the list.

---

## Detailed Code Walkthrough

### Part 1: Header Structure (allocator.h)

#### BlockHeader Structure

```cpp
struct BlockHeader {
    size_t size;           // Total size of block (including header)
    bool is_free;          // Whether this block is free
    BlockHeader* next;     // Next block in free list (only valid if is_free == true)
```

**Why these fields?**

- **size**: We need to know how big each block is to:
  - Find the next block (current address + size = next block)
  - Decide if a block is big enough for an allocation
  - Split blocks correctly

- **is_free**: Essential to distinguish used blocks from free blocks

- **next**: Links free blocks together in a linked list for fast searching

#### Helper Functions in BlockHeader

```cpp
void* get_data() {
    return reinterpret_cast<void*>(this + 1);
}
```

**What this does:**
- `this` is a pointer to the BlockHeader
- `this + 1` moves the pointer forward by one BlockHeader size
- This gives us a pointer to the data area (right after the header)

**Example:**
```
Address 1000: [BlockHeader - 24 bytes]
Address 1024: [User Data starts here] ← get_data() returns this
```

```cpp
static BlockHeader* get_header(void* ptr) {
    return reinterpret_cast<BlockHeader*>(ptr) - 1;
}
```

**What this does:**
- Given a pointer to user data, goes backward by one BlockHeader
- Returns a pointer to the header

**Example:**
```
User gives us pointer to address 1024 (their data)
We subtract 24 bytes → Address 1000 (the header) ← get_header() returns this
```

### Part 2: Initialization (allocator.cpp)

```cpp
static char heap_buffer[HEAP_SIZE];
static BlockHeader* free_list = nullptr;
```

**Global variables:**
- `heap_buffer`: The actual memory pool (1MB array of bytes)
- `free_list`: Pointer to the first free block (initially nullptr)

```cpp
void init_allocator() {
    std::memset(heap_buffer, 0, HEAP_SIZE);
    
    BlockHeader* initial_block = reinterpret_cast<BlockHeader*>(heap_buffer);
    initial_block->size = HEAP_SIZE;
    initial_block->is_free = true;
    initial_block->next = nullptr;
    
    free_list = initial_block;
}
```

**Step by step:**

1. `memset(heap_buffer, 0, HEAP_SIZE)`: Zero out all memory (good practice)

2. `reinterpret_cast<BlockHeader*>(heap_buffer)`:
   - `heap_buffer` is a `char[]` (array of bytes)
   - We treat the start of it as a `BlockHeader*`
   - This is safe because we're just viewing the same memory differently

3. Set up the initial block:
   - `size = HEAP_SIZE`: The entire heap is one block
   - `is_free = true`: It's free
   - `next = nullptr`: No other free blocks yet

4. `free_list = initial_block`: The free list starts with this one block

**Visual representation:**
```
heap_buffer (1MB)
[BlockHeader: size=1048576, is_free=true, next=nullptr][Free space...]
^
free_list points here
```

### Part 3: Alignment Function

```cpp
static size_t align_size(size_t size) {
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
}
```

**Why alignment?**
- Processors work more efficiently when data is aligned to certain boundaries (8 bytes for 64-bit systems)
- Unaligned access can be slower or cause crashes on some architectures

**How it works:**
- `ALIGN_SIZE - 1 = 7` (binary: 00000111)
- `~7 = 11111000` (bits flipped, clears the last 3 bits)
- `(size + 7) & ~7` rounds up to the nearest multiple of 8

**Examples:**
- `align_size(1)` → `(1+7) & ~7 = 8 & 0xF8 = 8`
- `align_size(8)` → `(8+7) & ~7 = 15 & 0xF8 = 8`
- `align_size(9)` → `(9+7) & ~7 = 16 & 0xF8 = 16`
- `align_size(100)` → `(100+7) & ~7 = 107 & 0xF8 = 104`

### Part 4: Allocation (my_malloc)

```cpp
void* my_malloc(size_t size) {
    if (size == 0) {
        return nullptr;
    }
```

**Edge case:** Allocating 0 bytes returns nullptr (like standard malloc)

```cpp
    size_t aligned_size = align_size(size);
    size_t total_size = sizeof(BlockHeader) + aligned_size;
```

**Calculate sizes:**
- `aligned_size`: Round up the user's request to 8-byte boundary
- `total_size`: Need header (24 bytes) + aligned user data

**Example:** User requests 100 bytes
- `aligned_size = 104` (rounded up from 100)
- `total_size = 24 + 104 = 128` bytes total

```cpp
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
    
    return nullptr;  // No suitable block found
}
```

**Algorithm (First-Fit):**

1. **Search the free list**: Start at `free_list`, follow `next` pointers
2. **Check each block**: Is `current->size >= total_size`?
3. **If found:**
   - Call `split_block()`: May split the block if it's too large
   - Remove from free list: Update pointers to skip this block
   - Mark as used: Set `is_free = false`
   - Return data pointer: Use `get_data()` to return pointer after header
4. **If not found**: Return `nullptr` (out of memory)

**Visual example:**
```
Free list: [Block A: 200 bytes] → [Block B: 50 bytes] → [Block C: 500 bytes]

Request: 100 bytes (need 128 total)

Step 1: Check Block A (200 bytes) → 200 >= 128? YES!
Step 2: Split Block A:
  - Block A: 128 bytes (used)
  - New Block D: 72 bytes (free, remains in free list)
Step 3: Remove Block A from free list
Step 4: Mark Block A as used
Step 5: Return pointer to Block A's data area
```

### Part 5: Block Splitting

```cpp
static BlockHeader* split_block(BlockHeader* block, size_t requested_size) {
    size_t aligned_size = align_size(requested_size);
    size_t total_size = sizeof(BlockHeader) + aligned_size;
    
    size_t min_remaining = get_min_block_size();
    if (block->size < total_size + min_remaining) {
        return block;  // Can't split, use whole block
    }
```

**Why split?**
- If we have a 10,000 byte free block and need 100 bytes, we don't want to waste 9,900 bytes!
- Split the large block into: allocated block (128 bytes) + remaining free block (9,872 bytes)

**When NOT to split:**
- If the remainder would be too small to be useful
- `min_remaining` = minimum size for a valid block (header + at least 1 byte)
- If `block->size < total_size + min_remaining`, the remainder is useless, so use the whole block

```cpp
    char* block_start = reinterpret_cast<char*>(block);
    BlockHeader* new_block = reinterpret_cast<BlockHeader*>(block_start + total_size);
    
    new_block->size = block->size - total_size;
    new_block->is_free = true;
    
    block->size = total_size;
    
    new_block->next = block->next;
    block->next = new_block;
    
    return block;
}
```

**Splitting process:**

1. Calculate where the new block starts: `block_start + total_size`
2. Set up new block header:
   - Size = original size - what we're using
   - Mark as free
3. Update original block size: Now it's exactly `total_size`
4. Update free list: Insert new block after original in the list

**Visual example:**
```
Before split:
[Block: 1000 bytes, free, next→BlockB]
              ↓
After split (request 100 bytes = 128 total):
[Block: 128 bytes, used] [NewBlock: 872 bytes, free, next→BlockB]
```

### Part 6: Freeing Memory (my_free)

```cpp
void my_free(void* ptr) {
    if (!ptr) {
        return;  // Freeing nullptr is safe
    }
    
    BlockHeader* block = BlockHeader::get_header(ptr);
```

**Get the header:**
- User gives us pointer to their data
- We use `get_header()` to go backward and find the header

```cpp
    if (!is_valid_ptr(block)) {
        std::cerr << "ERROR: Invalid pointer\n";
        return;
    }
    
    if (block->is_free) {
        std::cerr << "ERROR: Double free detected\n";
        return;
    }
```

**Safety checks:**
- **Invalid pointer**: Is it actually in our heap?
- **Double free**: Is this block already free? (Common bug!)

```cpp
    block->is_free = true;
    block->next = nullptr;
    
    BlockHeader* block_to_insert = coalesce_block(block);
    
    if (block_to_insert) {
        block_to_insert->next = free_list;
        free_list = block_to_insert;
    }
}
```

**Freeing process:**

1. Mark as free: Set `is_free = true`
2. Coalesce: Merge with adjacent free blocks (see below)
3. Add to free list: Insert at the beginning (simple and fast)

### Part 7: Coalescing (Merging Adjacent Free Blocks)

```cpp
static BlockHeader* coalesce_block(BlockHeader* block) {
    char* block_start = reinterpret_cast<char*>(block);
    char* block_end = block_start + block->size;
    
    // Check if next block exists and is free
    if (block_end < heap_end) {
        BlockHeader* next_block = reinterpret_cast<BlockHeader*>(block_end);
        if (next_block->is_free) {
            remove_from_free_list(next_block);
            block->size += next_block->size;
        }
    }
```

**Forward coalescing:**
- Calculate where the next block starts: `block_start + block->size`
- Check if it's free
- If free: Remove next block from free list, merge its size into current block

**Visual example:**
```
Before:
[Block A: 100 bytes, free] [Block B: 200 bytes, free] [Block C: 300 bytes, used]

Free Block A, then coalesce:
[Block A: 300 bytes, free] [Block C: 300 bytes, used]
      ↑ merged A and B
```

```cpp
    // Check if previous block exists and is free
    char* prev_block_end = heap_start;
    BlockHeader* prev_block = nullptr;
    
    while (prev_block_end < block_start) {
        BlockHeader* current = reinterpret_cast<BlockHeader*>(prev_block_end);
        char* current_end = prev_block_end + current->size;
        
        if (current_end == block_start && current->is_free) {
            prev_block = current;
            break;
        }
        
        prev_block_end = current_end;
    }
    
    if (prev_block) {
        prev_block->size += block->size;
        return nullptr;  // Block merged into previous, no need to insert
    }
    
    return block;  // Block needs to be inserted into free list
}
```

**Backward coalescing:**
- Traverse from heap start to find the block before this one
- Check if previous block ends exactly where current block starts
- If previous block is free: Merge current into previous
- Return `nullptr` because previous block is already in free list

**Complete coalescing example:**
```
Before freeing Block B:
[Block A: 100 bytes, FREE] [Block B: 200 bytes, USED] [Block C: 300 bytes, FREE]

Free Block B:
1. Mark Block B as free
2. Forward coalesce: Block C is free → merge: Block B becomes 500 bytes
3. Backward coalesce: Block A is free → merge into Block A
4. Result: [Block A: 600 bytes, FREE] [Block C header overwritten]
```

### Part 8: Debugging Functions

#### print_heap_state()

This function traverses the entire heap and prints information about each block.

```cpp
char* current = heap_buffer;
char* heap_end = heap_buffer + HEAP_SIZE;

while (current < heap_end) {
    BlockHeader* block = reinterpret_cast<BlockHeader*>(current);
    // Print block info
    current += block->size;  // Move to next block
}
```

**How it works:**
1. Start at the beginning of the heap
2. Treat current position as a BlockHeader
3. Print block information (address, size, status)
4. Move forward by `block->size` to get to next block
5. Repeat until we've covered the entire heap

**Why this works:**
- Blocks are stored contiguously (one after another)
- Each block's size tells us where the next block starts
- We can traverse the entire heap by following sizes

#### get_used_memory() and get_free_memory()

**get_used_memory():**
- Traverses the heap (like print_heap_state)
- Sums up `block->size - sizeof(BlockHeader)` for all used blocks
- Returns total bytes available to users (excluding headers)

**get_free_memory():**
- Traverses the free list (linked list of free blocks)
- Sums up `block->size - sizeof(BlockHeader)` for all free blocks
- Returns total free bytes available to users

**Note:** Used + Free should approximately equal Heap Size (accounting for headers)

---

## Compilation and Running

### Using Visual Studio Community (Windows - Recommended)

1. **Double-click `allocator.sln`** to open in Visual Studio
2. Select configuration (Debug/Release) and platform (x64/x86) from the dropdown
3. Press `F7` to build, or `Ctrl+Shift+B`
4. Press `F5` to run (with debugging) or `Ctrl+F5` (without debugging)

See `VISUAL_STUDIO_SETUP.md` for detailed instructions.

### Using g++ (GCC/MinGW)

```bash
g++ -std=c++11 -Wall -Wextra -O2 -o allocator allocator.cpp main.cpp
```

**Flags explained:**
- `-std=c++11`: Use C++11 standard
- `-Wall -Wextra`: Enable all warnings
- `-O2`: Optimize for speed
- `-o allocator`: Output executable name

### Using clang++

```bash
clang++ -std=c++11 -Wall -Wextra -O2 -o allocator allocator.cpp main.cpp
```

### Using MSVC Command Line (Windows)

Open "Developer Command Prompt for VS" and run:
```bash
cl /EHsc /std:c++11 /W4 /O2 allocator.cpp main.cpp /Fe:allocator.exe
```

### Running

```bash
# Linux/Mac
./allocator

# Windows
allocator.exe
```

---

## Understanding the Output

### Test 1: Basic Allocation

**What it does:**
- Allocates 3 blocks of different sizes (100, 200, 50 bytes)
- Writes data to each block
- Prints heap state

**What to look for:**
- Three USED blocks in the layout
- Sizes are rounded up to 8-byte boundaries (104, 200, 56 bytes user data)
- One large FREE block at the end
- Used Memory = sum of user data sizes (not including headers)
- Free Memory ≈ Heap Size - Used Memory - headers

### Test 2: Free and Reuse

**What it does:**
- Allocates 3 blocks
- Frees the middle block
- Allocates a new block (should reuse the freed space)

**What to look for:**
- After freeing: Fragmentation = 2 (two separate free blocks)
- New allocation gets the same address as the freed block (reuse!)
- After reallocating: Fragmentation = 1 (free blocks merged or one large block remains)

### Test 3: Block Splitting

**What it does:**
- Allocates a large block (5000 bytes)
- Frees it
- Allocates 3 smaller blocks (100, 200, 300 bytes)

**What to look for:**
- Large block gets split into 3 smaller blocks
- All 3 allocations come from the same area (the large block was split)
- Remaining free space is one block

### Test 4: Coalescing

**What it does:**
- Allocates 4 blocks
- Frees them in a pattern to test coalescing
- Frees block 2, then block 4, then block 1, then block 3

**What to look for:**
- After freeing block 2: Fragmentation = 2 (block 2 is free, block 4 is used)
- After freeing block 1: Block 1 and 2 merge! Fragmentation decreases
- After freeing block 3: All blocks merge into one! Fragmentation = 1

### Test 5: Edge Cases

**What it does:**
- Tests `malloc(0)` → should return nullptr
- Tests `free(nullptr)` → should be safe (do nothing)
- Tests double free → should detect and report error
- Tests invalid pointer → should detect and report error

**What to look for:**
- Error messages for double free and invalid pointer
- No crashes

### Test 6: Fragmentation

**What it does:**
- Allocates 10 blocks
- Frees every other block (1, 3, 5, 7, 9)
- Shows what fragmentation looks like

**What to look for:**
- Fragmentation = 5 (5 separate free blocks)
- Free list shows 5 entries
- After freeing all: Fragmentation = 1 (everything coalesced)

---

## Key Concepts Explained

### 1. What is a Heap?

In this project:
- Our heap is a 1MB static array (`heap_buffer`)
- We manage this memory ourselves
- This project creates it's own version of the OS heap.

### 2. Why Use Block Headers?

When a user calls `my_free(ptr)`, they just give us a pointer to their data. How do we know how big the block is, whether it's free or used,
or where the next block is? We can solve this by storing metadata in a header before each block of data. Headers use extra memory (24 bytes per block), but they're essential for tracking.

### 3. Why Alignment?

Processors read memory in chunks (8 bytes on 64-bit systems). If data isn't aligned, the processor may need multiple operations to read it.

As an example:
Unaligned (slow):
Address 1001: [Data starts here, spans 1001-1100]
Processor must: Read bytes 1000-1007, then 1008-1015, then combine

Aligned (fast):
Address 1008: [Data starts here, spans 1008-1108]
Processor can: Read bytes 1008-1015 in one operation

Therefore, we should round all allocations up to 8-byte boundaries.

### 4. Why Split Blocks?

If we have a 10,000 byte free block and need 100 bytes, should we give the user the whole 10,000 bytes? Definitely not. That wastes 9,900 bytes.
Instead, we can split large blocks, that way we use what we need (128 bytes for a 100-byte request) and keep the remainder as a free block (9,872 bytes).
More blocks = more headers = more overhead, but is much less wasted space.

### 5. Why Coalesce Blocks?

After many allocations and frees, we might have:
```
[Used][Free 100 bytes][Used][Free 200 bytes][Used][Free 300 bytes]
```
If someone requests 500 bytes, we can't satisfy it even though we actually have 600 free bytes total...

To solve this, when freeing,  we can merge adjacent free blocks:
```
[Used][Free 600 bytes][Used]
```
And then that will satisfy the 500-byte request. Coalescing takes time, but reduces this fragmentation problem.

### 6. What is Fragmentation?
Fragmentation is when free memory exists, but it's broken into small pieces that can't satisfy large requests.
```
Heap: [Used 1000][Free 100][Used 1000][Free 200][Used 1000][Free 300]
Request: 500 bytes → FAILS! (even though 600 bytes are free)
```
Types:
- External fragmentation: Free memory exists but is scattered (our problem)
- Internal fragmentation: Allocated block is larger than requested (due to alignment/splitting)

### 7. First-Fit Algorithm
Searches the free list from the beginning, takes the first block that's big enough.

Advantages:
- Simple to implement
- Fast (often finds block near the beginning)
- Good for small allocations

Disadvantages:
- Can cause fragmentation
- Not optimal (might skip a better-fitting block)

Alternatives:
- Best-Fit: Find the smallest block that fits (reduces waste, but slower)
- Worst-Fit: Find the largest block (reduces fragmentation, but wastes space)
- Buddy System: More complex, but very efficient

### 8. Linked List for Free Blocks
Why a linked list?
- Free blocks are scattered throughout the heap (not contiguous)
- We need to quickly find free blocks
- Linked list allows O(1) insertion/deletion

Structure:
```
free_list → [Block A] → [Block C] → [Block E] → nullptr
            (at addr 1000)  (at addr 5000)  (at addr 8000)
```
Note: Used blocks (B, D) are not in the list.

### 9. Pointer Arithmetic
In C++, adding to a pointer moves it by the size of the pointed-to type.
Example:
```
int* ptr = address 1000;
ptr + 1 = address 1004  // int is 4 bytes

char* ptr = address 1000;
ptr + 1 = address 1001  // char is 1 byte

BlockHeader* ptr = address 1000;
ptr + 1 = address 1024  // BlockHeader is 24 bytes
```

In our code:
- `block_start + total_size`: Move forward by `total_size` bytes (using `char*` for byte-wise arithmetic)
- `this + 1`: Move forward by one BlockHeader (using `BlockHeader*`)

## Limitations

This allocator is designed just for educational purposes and has several limitations that may make scaling infeasible:

- **Single-threaded only**: Not thread-safe (would need locks for multi-threading)
- **Fixed heap size**: 1MB, cannot grow (real allocators request more from OS)
- **Simple algorithm**: First-fit, not optimized for performance
- **No advanced features**: No realloc, calloc, alignment options, etc.
- **No memory protection**: No guard pages or bounds checking for user data
- **No virtual memory**: All memory is physical (real allocators use virtual memory)

---

## Learning Points

This implementation demonstrates:

- How to allocate and free memory manually
- Calculating addresses and offsets
- Implementing and manipulating linked lists
- Why and how to align data 
- Storing information with data
- Understanding and managing fragmentation
- Merging adjacent free blocks
- Validating inputs and detecting errors
- Inspecting memory layouts and states 

---

## What should you learn about next?

- Other memory allocators: Research buddy system allocators, slab allocators, jemalloc, and tcmalloc
- Heap management
- Understand how operating systems manage memory with virtual memory 
- Memory pools
- Learn how garbage collection works in C++ and other languages

---
