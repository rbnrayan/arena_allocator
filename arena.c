#include "arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#define ALIGN __alignof__(max_align_t)
#define ALIGN_FORWARD(x) ((x + (ALIGN - 1)) & ~(ALIGN - 1))
#define IS_ALIGNED(x) (!(x & (ALIGN - 1)))

#ifdef ARENA_DEBUG_PRINT
    #include <stdio.h>
#endif

struct Arena {
    size_t   cursor;
    size_t   capacity;
    uint8_t *mem_block;
};

struct Arena *arena_new(size_t size)
{
    uint8_t *mem_block = malloc(size);
    if (mem_block == NULL)
        return NULL;

    struct Arena *arena = malloc(sizeof(struct Arena));
    if (arena == NULL)
        return NULL;
    arena->mem_block = mem_block;
    arena->cursor = 0;
    arena->capacity = size;

    return arena;
}

void *arena_alloc(struct Arena *arena, size_t size)
{
    size_t aligned_size = ALIGN_FORWARD(size);
    if(arena->cursor + aligned_size > arena->capacity)
        return NULL;

#ifdef ARENA_DEBUG_PRINT
    printf("DEBUG:\talign           %zu\n", ALIGN);
    printf("DEBUG:\taligned_size    %zu\n", aligned_size);
    printf("DEBUG:\tmem             %zu\n", arena->cursor);
    printf("DEBUG:\tcursor (before) %zu\n", arena->cursor);
    printf("DEBUG:\tcursor (after)  %zu\n", arena->cursor + aligned_size);
    printf("DEBUG:\tremains         %zu\n", arena->capacity - (arena->cursor + aligned_size));
#endif

    void *mem = arena->mem_block + arena->cursor;
    arena->cursor += aligned_size;
    return mem;
}

void arena_free(struct Arena *arena)
{
    free(arena->mem_block);
    free(arena);
}

