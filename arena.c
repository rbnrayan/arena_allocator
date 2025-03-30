#include "arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef ARENA_DEBUG_PRINT
    #include <stdio.h>
#endif

#define DEFAULT_ALIGN __alignof__(max_align_t)
#define ALIGN_FORWARD(x, align) ((x + (align - 1)) & ~(align - 1))
#define IS_ALIGNED(x, align) (!(x & (align - 1)))

struct Arena {
    size_t   capacity;
    void    *cursor;
    void    *mem_block;
};

struct Arena *arena_new(size_t size)
{
    void *mem_block = malloc(size);
    if (mem_block == NULL)
        return NULL;

    struct Arena *arena = malloc(sizeof(struct Arena));
    if (arena == NULL) {
        free(mem_block);
        return NULL;
    }

    arena->mem_block = mem_block;
    arena->cursor = mem_block;
    arena->capacity = size;
    return arena;
}

/*
 * MEM_BLOCK is already guaranteed to be aligned because it is allocated using malloc.
 * So we only need to handle alignment from inside the arena using an OFFSET (ptrdiff_t).
 */
void *arena_alloc_align(struct Arena *arena, size_t size, size_t align)
{
    assert(!(align & (align - 1)) && "aligment must equal to 0 or be a power of two");

    ptrdiff_t offset = arena->cursor - arena->mem_block;
    size_t padding = 0;

    if (align > 0 && !IS_ALIGNED(offset, align))
        padding = align - (offset & (align - 1));

    if(arena->cursor + padding + size > arena->mem_block + arena->capacity)
        return NULL;

#ifdef ARENA_DEBUG_PRINT
    printf("# ARENA_ALLOC_ALIGN(size = %zu, align = %zu)\n", size, align);
    printf("ARENA_DEBUG:\tmem_block       %p\n", arena->mem_block);
    printf("ARENA_DEBUG:\toffset          %zu\n", offset);
    printf("ARENA_DEBUG:\tpadding         %zu\n", padding);
    printf("ARENA_DEBUG:\tcapacity        %zuB\n", arena->capacity);
    printf("ARENA_DEBUG:\tcursor (before) %p\n", arena->cursor);
    printf("ARENA_DEBUG:\tcursor (after)  %p\n", arena->cursor + padding + size);
    printf("ARENA_DEBUG:\tremains         %zuB\n",
           (arena->mem_block + arena->capacity) - (arena->cursor + padding + size));
#endif

    void *ptr = arena->cursor + padding;
    arena->cursor += padding + size;
    return ptr;
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

