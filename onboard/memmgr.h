//----------------------------------------------------------------
// Statically-allocated memory manager
//
// by Eli Bendersky (eliben@gmail.com)
//
// This code is in the public domain.
//----------------------------------------------------------------
#ifndef MEMMGR_H
#define MEMMGR_H
//
// Memory manager: dynamically allocates memory from
// a fixed pool that is allocated statically at link-time.
//
// Usage: after calling memmgr_init() in your
// initialization routine, just use memmgr_alloc() instead
// of malloc() and memmgr_free() instead of free().
// Naturally, you can use the preprocessor to define
// malloc() and free() as aliases to memmgr_alloc() and
// memmgr_free(). This way the manager will be a drop-in
// replacement for the standard C library allocators, and can
// be useful for debugging memory allocation problems and
// leaks.
//
// Preprocessor flags you can define to customize the
// memory manager:
//
// DEBUG_MEMMGR_FATAL
//    Allow printing out a message when allocations fail
//
// DEBUG_MEMMGR_SUPPORT_STATS
//    Allow printing out of stats in function
//    memmgr_print_stats When this is disabled,
//    memmgr_print_stats does nothing.
//
// Note that in production code on an embedded system
// you'll probably want to keep those undefined, because
// they cause printf to be called.
//
// POOL_SIZE
//    Size of the pool for new allocations. This is
//    effectively the heap size of the application, and can
//    be changed in accordance with the available memory
//    resources.
//
// MIN_POOL_ALLOC_QUANTAS
//    Internally, the memory manager allocates memory in
//    quantas roughly the size of two ulong objects. To
//    minimize pool fragmentation in case of multiple allocations
//    and deallocations, it is advisable to not allocate
//    blocks that are too small.
//    This flag sets the minimal ammount of quantas for
//    an allocation. If the size of a ulong is 4 and you
//    set this flag to 16, the minimal size of an allocation
//    will be 4 * 2 * 16 = 128 bytes
//    If you have a lot of small allocations, keep this value
//    low to conserve memory. If you have mostly large
//    allocations, it is best to make it higher, to avoid
//    fragmentation.
//

const unsigned int POOL_SIZE = 512;
const unsigned int MIN_POOL_ALLOC_QUANTAS = 7;

class MemoryManager {
public:
    static MemoryManager& instance() {
	static MemoryManager inst;
	return inst;
    }

    void*		alloc(unsigned int nbytes);
    void		free(void* ap);
private:
    MemoryManager();


    union mem_header_union {
        struct {
            // Pointer to the next block in the free list
            union mem_header_union* next;
            // Size of the block (in quantas of sizeof(mem_header_t))
            unsigned int size;
        } s;
        // Used to align headers in memory to a boundary
        unsigned int align_dummy;
    };

    typedef union mem_header_union mem_header_t;

    // Initial empty list
    mem_header_t base;

    // Start of free list
    mem_header_t* freep;

    // Static pool for new allocations
    char pool[POOL_SIZE];
    unsigned int pool_free_pos;

    void		init();
    mem_header_t*	get_mem_from_pool(unsigned int);
};


void* operator new(unsigned int size);
void operator delete(void * addr);

#endif // MEMMGR_H


