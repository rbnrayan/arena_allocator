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
    struct Arena *arena = arena_new(4096);

    struct Sample *sample = arena_alloc(arena, sizeof(struct Sample));
    assert(sample != NULL);

    sample->child = arena_alloc(arena, sizeof(struct Child) * 5);
    assert(sample->child != NULL);
    for (size_t i = 0; i < 5; ++i) {
        sample->child[i].foo = i;
    }

    for (size_t i = 0; i < 5; ++i) {
        printf("child %d\n", sample->child[i].foo);
    }

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
