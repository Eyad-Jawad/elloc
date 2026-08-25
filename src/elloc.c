#include "elloc.h"


metadata mtda = {
    .total_number_of_chunks = 0,
    .total_size = 0,
    .first_chunk = nullptr,
    .last_chunk = nullptr,
    .heap_start = nullptr,
    .heap_end = nullptr,
};

char *elloc(long long int given_size) {
    /*
    zero or negative sizes return nullptr ptr
    if the total allocation is bigger than the 
    max int which is 2gb or 4gb depending on the os
    a nullptr ptr will be returned, though it doesn't check 
    for the actual totoal available memory though it includes
    the size of the chunk metadata
    */

    if (given_size <= 0)
        return nullptr;
    
    size_t size = given_size + given_size % alignof(max_align_t);

    if (mtda.total_size + size >= INT_MAX) 
        return nullptr;
    
    if (mtda.heap_start == nullptr) {
        mtda.heap_start = sbrk(0);

        sbrk(size + CHUNK_SIZE);
        mtda.total_size = size + CHUNK_SIZE;
        mtda.total_number_of_chunks = 1;

        mtda.heap_end = sbrk(0);

        chunks_double_list *chunk = (chunks_double_list *) mtda.heap_start;

        chunk->size = get_chunk_size(chunk);
        chunk->is_used = false;
        chunk->next = nullptr;
        chunk->past = nullptr;

        mtda.first_chunk = chunk;
    }

    while (1) {
        for (chunks_double_list *i = mtda.first_chunk; i != nullptr; i = i->next) {
            if (i->size >= size && !i->is_used) {
                split_chunk(i, size);
                i->is_used = true;
                return (char *) i + CHUNK_SIZE;
            }
        }

        extend_heap();
    }
}

int efree(char *location) {
    /*
    0 success
    1 error: invalid ptr
    2 error: already freed memory
    */

    if (location == nullptr) 
        return 1;
    
    if (location <= mtda.heap_start || location > mtda.heap_end) 
        return 1;

    chunks_double_list *chunk = (chunks_double_list *) (location - CHUNK_SIZE);

    if (!chunk->is_used) 
        return 2;

    chunk->is_used = false;
    int merge;

    if (chunk->next != nullptr) {
        merge = merge_chunk(chunk, chunk->next);
        if (merge == 0) mtda.total_number_of_chunks--;
    }

    if (chunk->past != nullptr) {
        merge = merge_chunk(chunk->past, chunk);
        if (merge == 0) mtda.total_number_of_chunks--;
    }

    return 0;
}

void extend_heap() {
    /*
    Don't call this if you have not initialized the heap yet.
    */
    sbrk(mtda.total_size);

    chunks_double_list *new_chunk = (chunks_double_list *) (mtda.heap_end);
    new_chunk->is_used = false;
    new_chunk->size = mtda.total_size - CHUNK_SIZE;

    new_chunk->past = mtda.last_chunk;
    if (mtda.last_chunk == nullptr) {
        new_chunk->past = mtda.first_chunk;
        mtda.first_chunk->next = new_chunk;
    } else {
        mtda.last_chunk->next = new_chunk;
    }

    mtda.last_chunk = new_chunk;

    mtda.total_number_of_chunks += merge_chunk(new_chunk->past, new_chunk);
    mtda.heap_end = sbrk(0);
    mtda.total_size = mtda.heap_end - mtda.heap_start;

    // fprintf(stderr, "Heap was extended to %lld\n", mtda.total_size);
}

int merge_chunk(chunks_double_list *a, chunks_double_list *b) {
    /*
    Don't call this if you have not initialized the heap yet
    and not before making one another chunk yet
    it is assumed that a->next = b

    1 indicates no merge, 0 indicates merge
    */
   
    if (a->is_used || b->is_used)
        return 1;

    if (mtda.last_chunk == b) {
        mtda.last_chunk = a;
        if (a == mtda.first_chunk)
            mtda.last_chunk = nullptr;
    } else {
        b->next->past = a;
    }

    a->size += b->size + CHUNK_SIZE;
    a->next = b->next; 

    // fprintf(stderr, "Chunk %p (%ld) was merged with %p (%ld now)\n", b, b->size, a, a->size);
    return 0;
}

void split_chunk(chunks_double_list *chunk, size_t size) {
    if (chunk->size == size || chunk->size - size <= CHUNK_SIZE)
        return;

    // this CHUNK_SIZE is for the data of the first chunk
    chunks_double_list *new_chunk = (chunks_double_list *) (((char *) chunk) + size + CHUNK_SIZE);

    new_chunk->is_used = false;

    new_chunk->past = chunk;
    if (chunk->next) {
        chunk->next->past = new_chunk;
        new_chunk->next = chunk->next;
    } else {
        mtda.last_chunk = new_chunk;
    }

    new_chunk->next = chunk->next;
    chunk->next = new_chunk;

    new_chunk->size = get_chunk_size(new_chunk);
    chunk->size = get_chunk_size(chunk);

    mtda.total_number_of_chunks += 1;

    // fprintf(stderr, "Split chunk %p to %ld at %p and %ld at %p\n", chunk, chunk->size, chunk, new_chunk->size, new_chunk);
}

int get_chunk_size(chunks_double_list *chunk) {
    if (chunk->next)
        return ((char *) chunk->next) - ((char *) chunk) - CHUNK_SIZE;
    else
        return mtda.heap_end - ((char *) chunk) - CHUNK_SIZE;
}