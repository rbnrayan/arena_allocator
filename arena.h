#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

struct Arena;

/* Returns a new arena allocator with a capacity of SIZE */
struct Arena *arena_new(size_t size);
/* Allocates SIZE bytes of memory from ARENA aligned with a default alignment
   set to `__alignof__(max_align_t)`. */
void *arena_alloc(struct Arena *arena, size_t size);
/* Allocates SIZE bytes of memory from ARENA with no alignment */
void *arena_alloc_packed(struct Arena *arena, size_t size);
/* Allocates SIZE bytes of memory from ARENA aligned on ALIGN. */
void *arena_alloc_align(struct Arena *arena, size_t size, size_t align);
/* Free all memory allocated with ARENA */
void arena_free(struct Arena *arena);

#endif /* ARENA_H */
