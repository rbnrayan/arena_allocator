#include "arena.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void)
{
    struct Arena *arena = arena_new(32);
    assert(arena != NULL);
    char *hello = arena_alloc(arena, sizeof(char) * 6);
    assert(hello != NULL);
    strcpy(hello, "Hello");
    char *world = arena_alloc(arena, sizeof(char) * 6);
    assert(world != NULL);
    strcpy(world, "world");

    printf("%s %s\n", hello, world);

    arena_free(arena);

    return 0;
}
