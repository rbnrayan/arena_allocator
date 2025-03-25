#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>
#include <stddef.h>

struct Arena;

/* Returns a new arena allocator with a capacity of SIZE. */
struct Arena *arena_new(size_t size);
/* Allocates SIZE bytes of memory from ARENA. */
void *arena_alloc(struct Arena *arena, size_t size);
/* Free all memory allocated with ARENA */
void arena_free(struct Arena *arena);

#endif /* ARENA_H */
