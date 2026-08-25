#include <criterion/criterion.h>
#include "../src/elloc.h"

static void expect_chunk(chunks_double_list *chunk, bool is_used, chunks_double_list *next, chunks_double_list *past) {
    cr_expect_eq(chunk->is_used, is_used);
    cr_expect_eq(chunk->next, next);
    cr_expect_eq(chunk->past, past);
}

static chunks_double_list *chunk_of(void *ptr) {
    return (chunks_double_list *) ((char *)ptr - sizeof(chunks_double_list));
}

static chunks_double_list *chunk_after(void *ptr) {
    chunks_double_list *chunk = chunk_of(ptr);

    return (chunks_double_list *) ((char *)chunk + chunk->size + sizeof(chunks_double_list));
}

Test(test_internal_elloc, test_chunk_one_allocation) {
    void *first = elloc(5);

    chunks_double_list *chunk = chunk_of(first);

    expect_chunk(chunk, true, nullptr, nullptr);

    cr_assert_eq(efree(first), 0);
}

Test(test_internal_elloc, test_chunk_two_allocation) {
    void *first = elloc(10);
    void *second = elloc(10);

    chunks_double_list *first_chunk = chunk_of(first);
    chunks_double_list *second_chunk = chunk_of(second);

    expect_chunk(first_chunk, true, second_chunk, nullptr);
    expect_chunk(second_chunk, true, nullptr, first_chunk);

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
    chunks_double_list *fourth_chunk = chunk_after(third);

    expect_chunk(first_chunk, true, second_chunk, nullptr);
    expect_chunk(second_chunk, true, third_chunk, first_chunk);
    expect_chunk(third_chunk, true, fourth_chunk, second_chunk);
    expect_chunk(fourth_chunk, false, nullptr, third_chunk);

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
    chunks_double_list *fifth_chunk = chunk_after(fourth);

    expect_chunk(first_chunk, true, second_chunk, nullptr);
    expect_chunk(second_chunk, true, third_chunk, first_chunk);
    expect_chunk(third_chunk, true, fourth_chunk, second_chunk);
    expect_chunk(fourth_chunk, true, fifth_chunk, third_chunk);
    expect_chunk(fifth_chunk, false, nullptr, fourth_chunk);

    cr_assert_eq(efree(first), 0);
    cr_assert_eq(efree(second), 0);
    cr_assert_eq(efree(third), 0);
    cr_assert_eq(efree(fourth), 0);
}