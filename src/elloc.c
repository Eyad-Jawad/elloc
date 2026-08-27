#include "elloc.h"

metadata mtda = {
    .total_number_of_chunks = 0,
    .total_size = 0,
    .first_chunk = NULL,
    .last_chunk = NULL,
    .heap_start = NULL,
    .heap_end = NULL,
};

avl_tree *root = NULL;


char *elloc(long long int given_size) {
    if (given_size <= 0)
        return NULL;

    size_t size = (size_t)given_size;

    if (size < 16)
        size = 16;

    size_t alignment = alignof(max_align_t);

    size = ((size + alignment - 1) / alignment) * alignment;

    if (size > INT_MAX - CHUNK_SIZE)
        return NULL;

    if (mtda.total_size > INT_MAX - size - CHUNK_SIZE)
        return NULL;

    if (mtda.heap_start == NULL) {
        char *brk = sbrk(0);

        if (brk == (void *)-1)
            return NULL;

        size_t offset =
            (alignment -
             ((uintptr_t)brk + CHUNK_SIZE) % alignment)
            % alignment;

        if (offset != 0) {
            if (sbrk(offset) == (void *)-1)
                return NULL;
        }

        mtda.heap_start = sbrk(0);

        if (sbrk(size + CHUNK_SIZE) == (void *)-1)
            return NULL;

        mtda.total_size =
            size + CHUNK_SIZE + offset;

        mtda.total_number_of_chunks = 1;

        mtda.heap_end = sbrk(0);

        avl_tree *chunk =
            (avl_tree *)mtda.heap_start;

        chunk->right = NULL;
        chunk->left = NULL;
        chunk->height = 1;

        chunk->size = size;
        chunk->prev_size = 0;
        chunk->is_used = false;

        mtda.first_chunk = chunk;
        mtda.last_chunk = chunk;

        root = insert_chunk(root, chunk);
    }

    while (1) {
        avl_tree *chunk =
            find_chunk(root, size);

        if (chunk != NULL) {
            split_chunk(chunk, size);

            chunk->is_used = true;

            return (char *)chunk + CHUNK_SIZE;
        }

        extend_heap();
    }
}

int efree(char *location) {
    /*
     * 0 = success
     * 1 = invalid pointer
     * 2 = already freed
     */

    if (location == NULL)
        return 1;

    char *chunk_address =
        location - CHUNK_SIZE;

    if (chunk_address < (char *)mtda.heap_start ||
        chunk_address >= (char *)mtda.heap_end)
        return 1;

    avl_tree *chunk =
        (avl_tree *)chunk_address;

    if (!chunk->is_used)
        return 2;

    chunk->is_used = false;

    mtda.total_number_of_chunks +=
        merge_chunk(chunk);

    return 0;
}


avl_tree *get_next_chunk(avl_tree *chunk) {
    char *next_chunk_address =
        (char *)chunk +
        chunk->size +
        CHUNK_SIZE;

    if (next_chunk_address >=
        (char *)mtda.heap_end)
        return NULL;

    return (avl_tree *)next_chunk_address;
}


avl_tree *get_past_chunk(avl_tree *chunk) {
    if (chunk->prev_size == 0)
        return NULL;

    char *past_chunk_address =
        (char *)chunk -
        chunk->prev_size -
        CHUNK_SIZE;

    if (past_chunk_address <
        (char *)mtda.heap_start)
        return NULL;

    return (avl_tree *)past_chunk_address;
}


void extend_heap(void) {
    /*
     * Don't call before heap initialization.
     */

    size_t old_total_size = mtda.total_size;

    if (sbrk(old_total_size) == (void *)-1)
        return;

    avl_tree *new_chunk =
        (avl_tree *)mtda.heap_end;

    new_chunk->right = NULL;
    new_chunk->left = NULL;
    new_chunk->height = 1;

    new_chunk->is_used = false;

    new_chunk->size =
        old_total_size - CHUNK_SIZE;

    new_chunk->prev_size =
        mtda.last_chunk ?
        mtda.last_chunk->size :
        0;

    mtda.last_chunk = new_chunk;

    root = insert_chunk(root, new_chunk);

    mtda.total_number_of_chunks++;

    mtda.heap_end = sbrk(0);

    mtda.total_size =
        (char *)mtda.heap_end -
        (char *)mtda.heap_start;

    mtda.total_number_of_chunks +=
        merge_chunk(new_chunk);
}


int merge_chunk(avl_tree *a) {
    /*
     * -2 = two merges
     * -1 = one merge
     *  0 = no merge
     */

    if (a == NULL || a->is_used)
        return 0;

    int merge_stats = 0;

    avl_tree *b = get_next_chunk(a);

    if (b != NULL && !b->is_used) {
        root = delete_chunk(root, a);
        root = delete_chunk(root, b);

        a->size +=
            b->size +
            CHUNK_SIZE;

        avl_tree *c = get_next_chunk(a);

        if (c != NULL)
            c->prev_size = a->size;

        if (mtda.last_chunk == b)
            mtda.last_chunk = a;

        root = insert_chunk(root, a);

        merge_stats--;
    }

    avl_tree *d = get_past_chunk(a);

    if (d == NULL || d->is_used)
        return merge_stats;

    return merge_chunk(d) + merge_stats;
}


void split_chunk(avl_tree *chunk, size_t size) {
    if (chunk == NULL)
        return;

    if (chunk->size == size)
        return;

    if (chunk->size - size <= CHUNK_SIZE)
        return;

    size_t old_size = chunk->size;

    avl_tree *new_chunk =
        (avl_tree *)(
            (char *)chunk +
            size +
            CHUNK_SIZE
        );

    root = delete_chunk(root, chunk);

    new_chunk->right = NULL;
    new_chunk->left = NULL;
    new_chunk->height = 1;
    new_chunk->is_used = false;

    chunk->size = size;

    new_chunk->size =
        old_size - size - CHUNK_SIZE;

    new_chunk->prev_size =
        chunk->size;

    avl_tree *next_chunk =
        get_next_chunk(new_chunk);

    if (next_chunk != NULL)
        next_chunk->prev_size =
            new_chunk->size;

    root = insert_chunk(root, chunk);
    root = insert_chunk(root, new_chunk);

    mtda.total_number_of_chunks++;

    if (mtda.last_chunk == chunk)
        mtda.last_chunk = new_chunk;
}


size_t get_chunk_size(avl_tree *chunk) {
    avl_tree *next_chunk =
        get_next_chunk(chunk);

    if (next_chunk != NULL) {

        return (size_t)(
            (char *)next_chunk -
            (char *)chunk -
            CHUNK_SIZE
        );
    }

    return (size_t)(
        (char *)mtda.heap_end -
        (char *)chunk -
        CHUNK_SIZE
    );
}