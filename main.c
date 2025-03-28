#include "arena.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

struct Sample {
    struct Child *child;
};

struct Child {
    int foo;
};

int main(void)
{
    /* Lifetime for struct and members */
    size_t arena_size = sizeof(struct Sample) + sizeof(struct Child) * 5;
    struct Arena *arena = arena_new(arena_size);

    struct Sample *sample = arena_alloc_align(arena, sizeof(struct Sample), 8);
    assert(sample != NULL);

    sample->child = arena_alloc_align(arena, sizeof(struct Child) * 5, 4);
    assert(sample->child != NULL);

    arena_free(arena);

    /* Packed allocation for char buffers */
    size_t hello_world_size = 12;
    struct Arena *packed_arena = arena_new(hello_world_size);

    char *hello = arena_alloc_packed(packed_arena, 6);
    assert(hello != NULL);
    strcpy(hello, "hello");

    char *world = arena_alloc_packed(packed_arena, 6);
    assert(world != NULL);
    strcpy(world, "world");

    printf("%s %s\n", hello, world);

    arena_free(packed_arena);

    return 0;
}
