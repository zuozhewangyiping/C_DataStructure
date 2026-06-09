#include <stdio.h>
#include "ds_redblacktree.h"

static void print_value(DS_REDBLACKTREE_TYPE *value, void *user_data)
{
    (void)user_data;
    printf("%d ", value->key);
}

int main(void)
{
    DS_RedBlackTree *tree = ds_redblacktree_create();

    int chars[] = {70, 67, 75, 65, 68, 72, 77};
    for (int i = 0; i < 7; i++)
        ds_redblacktree_insert(tree, (ds_redblacktree_type){chars[i]});

    printf("in-order: ");
    ds_redblacktree_traverse_inorder_value(tree, NULL, print_value);
    printf("\nheight: %d, count: %d\n",
           ds_redblacktree_height(tree), ds_redblacktree_count(tree));

    ds_redblacktree_destroy(tree);
    return 0;
}
