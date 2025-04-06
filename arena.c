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
    uint8_t             *mem_block;
    size_t               capacity;
    struct ArenaBlock   *prev;
};

struct Arena {
    struct ArenaBlock   *last_block;
    size_t               cursor;
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
    if (arena == NULL) {
        return NULL;
    }

    struct ArenaBlock *block = malloc(sizeof(struct ArenaBlock));
    if (block == NULL) {
        free(arena);
        return NULL;
    }
    
    block->prev = NULL;
    block->capacity = block_capacity;
    block->mem_block = malloc(block_capacity);
    if (block->mem_block == NULL) {
        free(arena);
        free(block);
        return NULL;
    }

    arena->last_block = block;
    arena->cursor = 0;

    return arena;
}

void *arena_alloc_align(struct Arena *arena, size_t size, size_t align)
{
    assert(size > 0);

    size_t aligned_size = size;
    if (!IS_ALIGNED(aligned_size, align)) {
        aligned_size = ALIGN_FORWARD(aligned_size, align);
    }

    if (arena->cursor + aligned_size > arena->last_block->capacity) {
        struct ArenaBlock *block = malloc(sizeof(struct ArenaBlock));
        if (block == NULL) {
            return NULL;
        }

        size_t block_capacity = arena->last_block->capacity * 2;
        while(block_capacity < aligned_size) {
            block_capacity *= 2;
        }

        block->capacity = block_capacity;
        block->mem_block = malloc(block->capacity);
        if (block->mem_block == NULL) {
            free(block);
            return NULL;
        }
        block->prev = arena->last_block;

        arena->last_block = block;
        arena->cursor = 0;
    }

    /* NOTE: does this violate the strict aliasing rule? */
    void *ptr = arena->last_block->mem_block + arena->cursor;
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
    struct ArenaBlock *head = arena->last_block;
    while (head != NULL) {
        struct ArenaBlock *prev = head->prev;

        free(head->mem_block);
        free(head);

        head = prev;
    }
}

void arena_free(struct Arena *arena)
{
    arena_free_memblocks(arena);
    free(arena);
}
