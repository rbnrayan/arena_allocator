#include "arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef ARENA_DEBUG_PRINT
    #include <stdio.h>
#endif

#define DEFAULT_ALIGN __alignof__(max_align_t)
#define ALIGN_FORWARD(x, align) ((x + (align - 1)) & ~(align - 1))
#define IS_ALIGNED(x, align) (!(x & (align - 1)))

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
    if (arena == NULL) {
        free(mem_block);
        return NULL;
    }

    arena->mem_block = mem_block;
    arena->cursor = 0;
    arena->capacity = size;
    return arena;
}

void *arena_alloc_align(struct Arena *arena, size_t size, size_t align)
{
    assert((align & 1) == 0 && "aligment must be a power of two");

    uintptr_t curr_addr = (uintptr_t)arena->mem_block + arena->cursor;
    size_t padding = 0;

    if (align > 0 && !IS_ALIGNED(curr_addr, align))
        padding = align - (curr_addr & (align - 1));

    if(arena->cursor + padding + size > arena->capacity)
        return NULL;

#ifdef ARENA_DEBUG_PRINT
    printf("# ARENA_ALLOC_ALIGN(size = %zu, align = %zu)\n", size, align);
    printf("ARENA_DEBUG:\tmem_block       %p\n", arena->mem_block);
    printf("ARENA_DEBUG:\tcurr_addr       %p\n", (void *)curr_addr);
    printf("ARENA_DEBUG:\tret_addr        %p\n", (void *)curr_addr + padding);
    printf("ARENA_DEBUG:\tnext_addr       %p\n", (void *)curr_addr + padding + size);
    printf("ARENA_DEBUG:\tcapacity        %zuB\n", arena->capacity);
    printf("ARENA_DEBUG:\tcursor (before) %zu\n", arena->cursor);
    printf("ARENA_DEBUG:\tcursor (after)  %zu\n", arena->cursor + padding + size);
    printf("ARENA_DEBUG:\tremains         %zuB\n", arena->capacity - (arena->cursor + padding + size));
#endif

    arena->cursor = padding + size;
    return (void *)curr_addr + padding;
}

inline void *arena_alloc(struct Arena *arena, size_t size)
{
    return arena_alloc_align(arena, size, DEFAULT_ALIGN);
}

inline void *arena_alloc_packed(struct Arena *arena, size_t size)
{
    return arena_alloc_align(arena, size, 0);
}

void arena_free(struct Arena *arena)
{
    free(arena->mem_block);
    free(arena);
}

