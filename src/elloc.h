#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <stdalign.h>

#include "avl_tree.h"

#define CHUNK_SIZE sizeof(avl_tree)

typedef struct metadata {
    size_t total_number_of_chunks;
    size_t total_size;

    avl_tree *first_chunk;
    avl_tree *last_chunk;

    void *heap_start;
    void *heap_end;
} metadata;

extern metadata mtda;
extern avl_tree *root;

char *elloc(long long int given_size);
int efree(char *location);
void extend_heap(void);
void split_chunk(avl_tree *chunk, size_t size);
int merge_chunk(avl_tree *chunk);
avl_tree *get_next_chunk(avl_tree *chunk);
avl_tree *get_past_chunk(avl_tree *chunk);
size_t get_chunk_size(avl_tree *chunk);
