#include "avl_tree.h"
#include <stdio.h>

int height(avl_tree *node) {
    if (node == NULL)
        return 0;

    return node->height;
}

int max(int a, int b) {
    return a > b ? a : b;
}

avl_tree *right_rotate(avl_tree *y) {
    avl_tree *x = y->left;
    avl_tree *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height =
        1 + max(height(y->left), height(y->right));

    x->height =
        1 + max(height(x->left), height(x->right));

    return x;
}

avl_tree *left_rotate(avl_tree *x) {
    avl_tree *y = x->right;
    avl_tree *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height =
        1 + max(height(x->left), height(x->right));

    y->height =
        1 + max(height(y->left), height(y->right));

    return y;
}

int get_balance(avl_tree *node) {
    if (node == NULL)
        return 0;

    return height(node->left) - height(node->right);
}


static int compare_nodes(avl_tree *a, avl_tree *b) {
    if (a->size < b->size)
        return -1;

    if (a->size > b->size)
        return 1;

    uintptr_t addr_a = (uintptr_t)a;
    uintptr_t addr_b = (uintptr_t)b;

    if (addr_a < addr_b)
        return -1;

    if (addr_a > addr_b)
        return 1;

    return 0;
}


avl_tree *insert_chunk(avl_tree *root, avl_tree *node) {
    if (root == NULL)
        return node;

    if (compare_nodes(node, root) < 0)
        root->left = insert_chunk(root->left, node);
    else
        root->right = insert_chunk(root->right, node);

    root->height =
        1 + max(height(root->left), height(root->right));

    int balance = get_balance(root);

    if (balance > 1 &&
        compare_nodes(node, root->left) < 0)
        return right_rotate(root);

    if (balance < -1 &&
        compare_nodes(node, root->right) > 0)
        return left_rotate(root);

    if (balance > 1 &&
        compare_nodes(node, root->left) > 0) {

        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    if (balance < -1 &&
        compare_nodes(node, root->right) < 0) {

        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}


avl_tree *min_value_node(avl_tree *node) {
    avl_tree *current = node;

    while (current->left != NULL)
        current = current->left;

    return current;
}


static avl_tree *detach_min(avl_tree *root, avl_tree **min) {
    if (root->left == NULL) {
        *min = root;

        root->left = NULL;

        avl_tree *right = root->right;
        root->right = NULL;

        return right;
    }

    root->left = detach_min(root->left, min);

    root->height =
        1 + max(height(root->left), height(root->right));

    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0)
        return right_rotate(root);

    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    if (balance < -1 && get_balance(root->right) <= 0)
        return left_rotate(root);

    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}


avl_tree *delete_chunk(avl_tree *root, avl_tree *node) {
    if (root == NULL || node == NULL)
        return root;

    int cmp = compare_nodes(node, root);

    if (cmp < 0) {
        root->left = delete_chunk(root->left, node);
    }
    else if (cmp > 0) {
        root->right = delete_chunk(root->right, node);
    }
    else {
        if (root->left == NULL) {
            avl_tree *right = root->right;

            root->left = NULL;
            root->right = NULL;

            return right;
        }

        if (root->right == NULL) {
            avl_tree *left = root->left;

            root->left = NULL;
            root->right = NULL;

            return left;
        }

        avl_tree *successor;

        root->right =
            detach_min(root->right, &successor);

        successor->left = root->left;
        successor->right = root->right;

        successor->height =
            1 + max(
                height(successor->left),
                height(successor->right)
            );

        root->left = NULL;
        root->right = NULL;

        root = successor;
    }

    root->height =
        1 + max(
            height(root->left),
            height(root->right)
        );

    int balance = get_balance(root);

    if (balance > 1 &&
        get_balance(root->left) >= 0)
        return right_rotate(root);

    if (balance > 1 &&
        get_balance(root->left) < 0) {

        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    if (balance < -1 &&
        get_balance(root->right) <= 0)
        return left_rotate(root);

    if (balance < -1 &&
        get_balance(root->right) > 0) {

        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}

double diff(avl_tree *node, size_t size) {
    if (node == NULL || node->size < size)
        return INFINITY;

    return (double)(node->size - size);
}

static avl_tree *find_helper(
    avl_tree *node,
    size_t size,
    avl_tree *best) {
    if (node == NULL)
        return best;

    if (node->size < size)
        return find_helper(node->right, size, best);

    if (!node->is_used) {
        best = node;

        return find_helper(node->left, size, best);
    }

    best = find_helper(node->left, size, best);

    return find_helper(node->right, size, best);
}


avl_tree *find_chunk(avl_tree *root, size_t size) {
    return find_helper(root, size, NULL);
}