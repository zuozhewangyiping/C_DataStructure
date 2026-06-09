#include <stdio.h>
#include "ds_avltree.h"

static void print_value(DS_AVLTREE_TYPE *value, void *user_data)
{
    (void)user_data;
    printf("%c ", value->key);
}

int main(void)
{
    DS_AVLTree *tree = ds_avltree_create();

    int chars[] = {70, 67, 75, 65, 68, 72, 77};
    for (int i = 0; i < 7; i++)
        ds_avltree_insert(tree, (ds_avltree_type){chars[i]});

    printf("in-order: ");
    ds_avltree_traverse_inorder_value(tree, NULL, print_value);
    printf("\nheight: %d, count: %d\n",
           ds_avltree_height(tree), ds_avltree_count(tree));

    ds_avltree_destroy(tree);
    return 0;
}
