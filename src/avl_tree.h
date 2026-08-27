#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

typedef struct avl_tree {
    size_t size;
    size_t prev_size;

    int height;
    bool is_used;

    struct avl_tree *left;
    struct avl_tree *right;
} avl_tree;

int height(avl_tree *node);
int max(int a, int b);
int get_balance(avl_tree *node);
avl_tree *right_rotate(avl_tree *y);
avl_tree *left_rotate(avl_tree *x);
avl_tree *insert_chunk(avl_tree *root, avl_tree *node);
avl_tree *delete_chunk(avl_tree *root, avl_tree *node);
avl_tree *min_value_node(avl_tree *node);
double diff(avl_tree *node, size_t size);
avl_tree *find_chunk(avl_tree *root, size_t size);
