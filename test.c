#include "arena.h"
#include <assert.h>
#include <stdio.h>

inline static void test_passed(char *test_name)
{
    fprintf(stdout, "test: `%s' - successful.\n", test_name);
}

void test_arena_new_default_capacity(void)
{
    struct Arena *arena = arena_new(0);
    assert(arena != NULL && "test_arena_new_default_capacity");
    arena_free(arena);

    test_passed("test_arena_new_default_capacity");
}

void test_arena_new_with_capacity(void)
{
    struct Arena *arena = arena_new(4096);
    assert(arena != NULL && "test_arena_new_with_capacity");
    arena_free(arena);

    test_passed("test_arena_new_with_capacity");
}

void test_arena_alloc(void)
{
    struct Arena *arena = arena_new(0);
    assert(arena != NULL && "test_arena_alloc");
    
    char *buffer = arena_alloc(arena, 16);
    assert(buffer != NULL && "test_arena_alloc");

    arena_free(arena);

    test_passed("test_arena_alloc");
}

void test_arena_alloc_overflow_initial_capacity(void)
{
    struct Arena *arena = arena_new(16);
    assert(arena != NULL && "test_arena_alloc_overflow_initial_capacity");

    char *first_buffer = arena_alloc(arena, 16);
    assert(first_buffer != NULL && "test_arena_alloc_overflow_initial_capacity");

    /* Overflow initial capacity, +1 block */
    char *second_buffer = arena_alloc(arena, 16);
    assert(second_buffer != NULL && "test_arena_alloc_overflow_initial_capacity");

    arena_free(arena);

    test_passed("test_arena_alloc_overflow_initial_capacity");
}

int main(void)
{
    test_arena_new_default_capacity();
    test_arena_new_with_capacity();
    test_arena_alloc();
    test_arena_alloc_overflow_initial_capacity();

    return 0;
}
