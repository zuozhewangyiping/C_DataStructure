#include <stdio.h>
#include "ds_avltree.h"

static void visit_key(DS_AVLTREE_TYPE *v, void *ud) { (void)ud; printf("%d ", v->key); }

static void visit_range(DS_AVLTREE_TYPE *v, void *ud)
{
    (void)ud;
    printf("  key=%d, value=%d\n", v->key, v->value);
}

int main(void)
{
    DS_AVLTree *tree = ds_avltree_create();

    printf("is_empty: %d, height: %d, count: %d\n",
           ds_avltree_is_empty(tree), ds_avltree_height(tree), ds_avltree_count(tree));

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++)
        ds_avltree_insert(tree, (ds_avltree_type){keys[i], keys[i] * 10});
    printf("after 7 inserts: height=%d, count=%d\n",
           ds_avltree_height(tree), ds_avltree_count(tree));

    printf("preorder:   "); ds_avltree_traverse_preorder_value(tree, NULL, visit_key);  printf("\n");
    printf("inorder:    "); ds_avltree_traverse_inorder_value(tree, NULL, visit_key);   printf("\n");
    printf("postorder:  "); ds_avltree_traverse_postorder_value(tree, NULL, visit_key); printf("\n");
    printf("levelorder: "); ds_avltree_traverse_levelorder_value(tree, NULL, visit_key); printf("\n");

    AVLTreeNode *cur = ds_avltree_search(tree, (ds_avltree_type){.key = 40});
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cur, &data);
    printf("search(40): key=%d, value=%d\n", data->key, data->value);

    cur = ds_avltree_find_min(tree);
    ds_avltree_node_get_data(cur, &data);
    printf("min: key=%d\n", data->key);

    cur = ds_avltree_find_max(tree);
    ds_avltree_node_get_data(cur, &data);
    printf("max: key=%d\n", data->key);

    cur = ds_avltree_successor(tree, cur);
    printf("successor(max): %s\n", cur ? "exists" : "NULL");

    cur = ds_avltree_find_min(tree);
    cur = ds_avltree_predecessor(tree, cur);
    printf("predecessor(min): %s\n", cur ? "exists" : "NULL");

    cur = ds_avltree_find_min(tree);
    cur = ds_avltree_successor(tree, cur);
    ds_avltree_node_get_data(cur, &data);
    printf("successor(min): key=%d\n", data->key);

    printf("range_query [35, 65]:\n");
    ds_avltree_range_query(tree, (ds_avltree_type){.key = 35}, (ds_avltree_type){.key = 65}, NULL, visit_range);

    ds_avltree_delete(tree, (ds_avltree_type){.key = 20});
    ds_avltree_delete(tree, (ds_avltree_type){.key = 80});
    printf("after delete 20 and 80: count=%d\n", ds_avltree_count(tree));

    int judge;
    DS_AVLTree *clone = ds_avltree_clone(tree, &judge);
    printf("clone: count=%d, height=%d\n", ds_avltree_count(clone), ds_avltree_height(clone));

    ds_avltree_destroy(tree);
    ds_avltree_destroy(clone);
    return 0;
}
