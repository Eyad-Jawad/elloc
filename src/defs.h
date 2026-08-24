#include <unistd.h>
#include <string.h>
#include <stdbool.h>

typedef struct chunks_double_list chunks_double_list;
typedef struct free_chunks_double_list free_chunks_double_list;
typedef struct used_chunks_double_list used_chunks_double_list;
void split_chunk(chunks_double_list *chunk, size_t size);
void extend_heap();
int merge_chunk(chunks_double_list *a, chunks_double_list *b);
int get_chunk_size(chunks_double_list *chunk);

struct chunks_double_list{
    size_t size;
    chunks_double_list *next;
    chunks_double_list *past;
    bool is_used;
};

#define CHUNK_SIZE sizeof(chunks_double_list)

typedef struct {
    int total_number_of_chunks;
    long long total_size;
    chunks_double_list *first_chunk;
    chunks_double_list *last_chunk;
    char *heap_start;
    char *heap_end;
} metadata;

struct free_chunks_double_list{
    chunks_double_list *chunk;
    free_chunks_double_list *next;
    free_chunks_double_list *past;
};

struct used_chunks_double_list{
    chunks_double_list *chunk;
    used_chunks_double_list *next;
    used_chunks_double_list *past;
};

metadata mtda = {
    .total_number_of_chunks = 0,
    .total_size = 0,
    .first_chunk = NULL,
    .last_chunk = NULL,
    .heap_start = NULL,
    .heap_end = NULL,
};

free_chunks_double_list fcdl = {
    .chunk = NULL,
    .next = NULL,
    .past = NULL,
};

used_chunks_double_list ucdl = {
    .chunk = NULL,
    .next = NULL,
    .past = NULL,
};

