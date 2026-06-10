#include <stdio.h>
#include "ds_redblacktree.h"

static void visit_key(DS_REDBLACKTREE_TYPE *v, void *ud) { (void)ud; printf("%d ", v->key); }

static void visit_range(DS_REDBLACKTREE_TYPE *v, void *ud)
{
    (void)ud;
    printf("  key=%d, value=%d\n", v->key, v->value);
}

int main(void)
{
    DS_RedBlackTree *tree = ds_redblacktree_create();

    printf("is_empty: %d, height: %d, count: %d\n",
           ds_redblacktree_is_empty(tree), ds_redblacktree_height(tree), ds_redblacktree_count(tree));

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++)
        ds_redblacktree_insert(tree, (ds_redblacktree_type){keys[i], keys[i] * 10});
    printf("after 7 inserts: height=%d, count=%d\n",
           ds_redblacktree_height(tree), ds_redblacktree_count(tree));

    printf("preorder:   "); ds_redblacktree_traverse_preorder_value(tree, NULL, visit_key);  printf("\n");
    printf("inorder:    "); ds_redblacktree_traverse_inorder_value(tree, NULL, visit_key);   printf("\n");
    printf("postorder:  "); ds_redblacktree_traverse_postorder_value(tree, NULL, visit_key); printf("\n");
    printf("levelorder: "); ds_redblacktree_traverse_levelorder_value(tree, NULL, visit_key); printf("\n");

    RedBlackTreeNode *cur = ds_redblacktree_search(tree, (ds_redblacktree_type){.key = 40});
    DS_REDBLACKTREE_TYPE *data;
    ds_redblacktree_node_get_data(cur, &data);
    printf("search(40): key=%d, value=%d\n", data->key, data->value);

    cur = ds_redblacktree_find_min(tree);
    ds_redblacktree_node_get_data(cur, &data);
    printf("min: key=%d\n", data->key);

    cur = ds_redblacktree_find_max(tree);
    ds_redblacktree_node_get_data(cur, &data);
    printf("max: key=%d\n", data->key);

    cur = ds_redblacktree_successor(tree, cur);
    printf("successor(max): %s\n", cur ? "exists" : "NULL");

    cur = ds_redblacktree_find_min(tree);
    cur = ds_redblacktree_predecessor(tree, cur);
    printf("predecessor(min): %s\n", cur ? "exists" : "NULL");

    cur = ds_redblacktree_find_min(tree);
    cur = ds_redblacktree_successor(tree, cur);
    ds_redblacktree_node_get_data(cur, &data);
    printf("successor(min): key=%d\n", data->key);

    printf("range_query [35, 65]:\n");
    ds_redblacktree_range_query(tree, (ds_redblacktree_type){.key = 35}, (ds_redblacktree_type){.key = 65}, NULL, visit_range);

    ds_redblacktree_delete(tree, (ds_redblacktree_type){.key = 20});
    ds_redblacktree_delete(tree, (ds_redblacktree_type){.key = 80});
    printf("after delete 20 and 80: count=%d\n", ds_redblacktree_count(tree));

    int judge;
    DS_RedBlackTree *clone = ds_redblacktree_clone(tree, &judge);
    printf("clone: count=%d, height=%d\n", ds_redblacktree_count(clone), ds_redblacktree_height(clone));

    ds_redblacktree_destroy(tree);
    ds_redblacktree_destroy(clone);
    return 0;
}
