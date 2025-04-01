#include "arena.h"
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define ARENA_DEFAULT_CAPACITY 256

#define PACKED_ALIGN 0
#define MAX_ALIGN __alignof__(max_align_t)
#define ALIGN_FORWARD(x, align) ((x + (align - 1)) & ~(align - 1))
#define IS_ALIGNED(x, align) (!(x & (align - 1)))

struct ArenaBlock {
    void                *mem_block;
    struct ArenaBlock   *next;
};

struct Arena {
    struct ArenaBlock   *mem_blocks;
    struct ArenaBlock   *current_block;
    size_t               cursor;
    size_t               per_block_capacity;
};

struct Arena *arena_new(size_t initial_capacity)
{
    size_t block_capacity;
    if (!initial_capacity) {
        block_capacity = ARENA_DEFAULT_CAPACITY;
    } else {
        block_capacity = initial_capacity;
    }

    struct Arena *arena = malloc(sizeof(struct Arena));
    if (arena == NULL)
        return NULL;

    struct ArenaBlock *block = malloc(sizeof(struct ArenaBlock));
    if (block == NULL)
        return NULL;
    
    block->next = NULL;
    block->mem_block = malloc(block_capacity);
    if (block->mem_block == NULL)
        return NULL;

    arena->mem_blocks = block;
    arena->current_block = block;
    arena->cursor = 0;
    arena->per_block_capacity = block_capacity;

    return arena;
}

void *arena_alloc_align(struct Arena *arena, size_t size, size_t align)
{
    assert(size > 0);

    size_t aligned_size = size;
    if (!IS_ALIGNED(aligned_size, align))
        aligned_size = ALIGN_FORWARD(aligned_size, align);

    if (arena->cursor + aligned_size > arena->per_block_capacity) {
        /* FIXME: if aligned_size is bigger than arena->per_block_capacity
                  this cannot work. (update PER_BLOCK_CAPACITY?) */
        assert(aligned_size <= arena->per_block_capacity);

        struct ArenaBlock *block = malloc(sizeof(struct ArenaBlock));
        if (block == NULL)
            return NULL;
        
        block->next = NULL;
        block->mem_block = malloc(arena->per_block_capacity);
        if (block->mem_block == NULL)
            return NULL;

        arena->current_block->next = block;
        arena->current_block = arena->current_block->next;
        arena->cursor = 0;
    }

    /* NOTE: does this violate the strict aliasing rule? */
    void *ptr = &((uint8_t *)arena->current_block->mem_block)[arena->cursor];
    arena->cursor += aligned_size;
    return ptr;
}

inline void *arena_alloc(struct Arena *arena, size_t size)
{
    return arena_alloc_align(arena, size, MAX_ALIGN);
}

inline void *arena_alloc_packed(struct Arena *arena, size_t size)
{
    return arena_alloc_align(arena, size, PACKED_ALIGN);
}

static void arena_free_memblocks(struct Arena *arena)
{
    struct ArenaBlock *head = arena->mem_blocks;
    while (head != NULL) {
        struct ArenaBlock *next = head->next;

        free(head->mem_block);
        free(head);

        head = next;
    }
}

void arena_free(struct Arena *arena)
{
    arena_free_memblocks(arena);
    free(arena);
}
