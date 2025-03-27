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
    struct Arena *arena = arena_new(4096);

    struct Sample *sample = arena_alloc_align(arena, sizeof(struct Sample), 8);
    assert(sample != NULL);
    sample->child = arena_alloc_align(arena, sizeof(struct Child) * 5, 4);
    assert(sample->child != NULL);

    arena_free(arena);

    return 0;
}
