#include <criterion/criterion.h>
#include "../src/elloc.h"

static void expect_chunk(avl_tree *chunk, bool is_used) {
    cr_expect_eq(chunk->is_used, is_used);
}

static avl_tree *chunk_of(void *ptr) {
    return (avl_tree *) ((char *)ptr - sizeof(avl_tree));
}

static avl_tree *chunk_after(void *ptr) {
    avl_tree *chunk = chunk_of(ptr);

    return (avl_tree *) ((char *)chunk + chunk->size + sizeof(avl_tree));
}

Test(test_internal_elloc, test_chunk_one_allocation) {
    void *first = elloc(5);

    avl_tree *chunk = chunk_of(first);

    expect_chunk(chunk, true);

    cr_assert_eq(efree(first), 0);
}

Test(test_internal_elloc, test_chunk_two_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);

    avl_tree *first_chunk = chunk_of(first);
    avl_tree *second_chunk = chunk_of(second);

    expect_chunk(first_chunk, true);
    expect_chunk(second_chunk, true);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
}

Test(test_internal_elloc, test_chunk_three_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);
    void *third = elloc(10);

    avl_tree *first_chunk = chunk_of(first);
    avl_tree *second_chunk = chunk_of(second);
    avl_tree *third_chunk = chunk_of(third);
    avl_tree *fourth_chunk = chunk_after(third);

    expect_chunk(first_chunk, true);
    expect_chunk(second_chunk, true);
    expect_chunk(third_chunk, true);
    expect_chunk(fourth_chunk, false);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
    cr_assert_eq(efree(third), 0);
}

Test(test_internal_elloc, test_chunk_four_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);
    void *third = elloc(10);
    void *fourth = elloc(15);

    avl_tree *first_chunk = chunk_of(first);
    avl_tree *second_chunk = chunk_of(second);
    avl_tree *third_chunk = chunk_of(third);
    avl_tree *fourth_chunk = chunk_of(fourth);
    avl_tree *fifth_chunk = chunk_after(fourth);

    expect_chunk(first_chunk, true);
    expect_chunk(second_chunk, true);
    expect_chunk(third_chunk, true);
    expect_chunk(fourth_chunk, true);
    expect_chunk(fifth_chunk, false);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
    cr_assert_eq(efree(third), 0);
    cr_assert_eq(efree(fourth), 0);
}