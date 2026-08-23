#include <stdio.h>
#include "defs.h"


char *elloc(size_t size) {
    if (mtda.heap_start == NULL) {
        mtda.heap_start = sbrk(0);


        sbrk(size + CHUNK_SIZE);
        mtda.total_size = size + CHUNK_SIZE;
        mtda.total_number_of_chunks = 1;

        mtda.heap_end = sbrk(0);

        chunks_double_list *chunk = (chunks_double_list *) mtda.heap_start;

        chunk->size = size;
        chunk->is_used = false;
        chunk->next = NULL;
        chunk->past = NULL;

        mtda.first_chunk = chunk;
    }
    while (1) {
        for (chunks_double_list *i = mtda.first_chunk; i != NULL; i = i->next) {
            if (i->size >= size && !i->is_used) {
                split_chunk(i, size);
                i->is_used = true;
                return (char *) i + CHUNK_SIZE;
            }
        }

        extend_heap();
    }
}

void efree(char *location) {
    chunks_double_list *chunk = (chunks_double_list *) (location - CHUNK_SIZE);
    chunk->is_used = false;
    if (chunk->next != NULL) {
        merge_chunk(chunk, chunk->next);
    } 
    if (chunk->past != NULL) {
        merge_chunk(chunk->past, chunk);
    }
}

void extend_heap() {
    /*
    Don't call this if you have not initialized the heap yet.
    */
    sbrk(mtda.total_size * 2);

    chunks_double_list *new_chunk = (chunks_double_list *) (mtda.heap_end + 1);
    new_chunk->is_used = false;
    new_chunk->size = mtda.total_size - CHUNK_SIZE;

    new_chunk->past = mtda.last_chunk;
    if (mtda.last_chunk == NULL) {
        new_chunk->past = mtda.first_chunk;
        mtda.first_chunk->next = new_chunk;
    } else {
        mtda.last_chunk->next = new_chunk;
    }

    mtda.last_chunk = new_chunk;

    mtda.total_number_of_chunks += merge_chunk(new_chunk->past, new_chunk);
    mtda.heap_end = sbrk(0);
    mtda.total_size *= 2;
}

int merge_chunk(chunks_double_list *a, chunks_double_list *b) {
    /*
    Don't call this if you have not initialized the heap yet
    and not before making one another chunk yet
    it is assumed that a->next = b

    1 indicates no merge, 0 indicates merge
    */
   
    if (a->is_used || b->is_used) {
        return 1;
    }

    if (mtda.last_chunk == b) {
        mtda.last_chunk = b->past;
        if (b->past == mtda.first_chunk) {
            mtda.last_chunk = NULL;
        }
    }

    a->size += b->size + CHUNK_SIZE;
    a->next = b->next; 

    return 0;
}

void split_chunk(chunks_double_list *chunk, size_t size) {
    if (chunk->size == size || chunk->size - size <= CHUNK_SIZE) {
        return;
    }

    // this CHUNK_SIZE is for the data of the first chunk
    chunks_double_list *new_chunk = (chunks_double_list *) (((char *) chunk) + size + CHUNK_SIZE);

    new_chunk->is_used = false;

    // this CHUNK_SIZE is for the data of the second chunk
    new_chunk->size = chunk->size - size - CHUNK_SIZE;

    new_chunk->past = chunk;
    if (chunk->next) {
        chunk->next->past = new_chunk;
        new_chunk->next = chunk->next->past;
    } else {
        mtda.last_chunk = new_chunk;
    }

    chunk->next = new_chunk;
}