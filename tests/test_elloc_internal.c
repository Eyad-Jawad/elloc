#include <criterion/criterion.h>
#include "../src/elloc.h"

static void expect_chunk(chunks_double_list *chunk, size_t size, bool is_used, chunks_double_list *next, chunks_double_list *past) {
    cr_expect_eq(chunk->size, size);
    cr_expect_eq(chunk->is_used, is_used);
    cr_expect_eq(chunk->next, next);
    cr_expect_eq(chunk->past, past);
}

static chunks_double_list *chunk_of(void *ptr) {
    return (chunks_double_list *) ((char *)ptr - sizeof(chunks_double_list));
}

Test(test_internal_elloc, test_chunk_one_allocation) {
    void *first = elloc(5);

    chunks_double_list *chunk = chunk_of(first);

    expect_chunk(chunk, 5, true, NULL, NULL);

    cr_assert_eq(efree(first), 0);
}

Test(test_internal_elloc, test_chunk_two_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);

    chunks_double_list *first_chunk = chunk_of(first);
    chunks_double_list *second_chunk = chunk_of(second);

    expect_chunk(first_chunk, 10, true, second_chunk, NULL);
    expect_chunk(second_chunk, 10, true, NULL, first_chunk);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
}

Test(test_internal_elloc, test_chunk_three_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);
    void *third = elloc(10);

    chunks_double_list *first_chunk = chunk_of(first);
    chunks_double_list *second_chunk = chunk_of(second);
    chunks_double_list *third_chunk = chunk_of(third);
    chunks_double_list *fourth_chunk = (chunks_double_list *) ((char *)third + 10);;

    expect_chunk(first_chunk, 10, true, second_chunk, NULL);
    expect_chunk(second_chunk, 10, true, third_chunk, first_chunk);
    expect_chunk(third_chunk, 10, true, fourth_chunk, second_chunk);
    expect_chunk(fourth_chunk, 10, false, NULL, third_chunk);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
    cr_assert_eq(efree(third), 0);
}

Test(test_internal_elloc, test_chunk_four_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);
    void *third = elloc(10);
    void *fourth = elloc(15);

    chunks_double_list *first_chunk = chunk_of(first);
    chunks_double_list *second_chunk = chunk_of(second);
    chunks_double_list *third_chunk = chunk_of(third);
    chunks_double_list *fourth_chunk = chunk_of(fourth);
    chunks_double_list *fifth_chunk = (chunks_double_list *) ((char *)fourth + 15);

    expect_chunk(first_chunk, 10, true, second_chunk, NULL);
    expect_chunk(second_chunk, 10, true, third_chunk, first_chunk);
    expect_chunk(third_chunk, 10, true, fourth_chunk, second_chunk);
    expect_chunk(fourth_chunk, 15, true, fifth_chunk, third_chunk);
    expect_chunk(fifth_chunk, 131, false, NULL, fourth_chunk);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
    cr_assert_eq(efree(third), 0);
    cr_assert_eq(efree(fourth), 0);
}