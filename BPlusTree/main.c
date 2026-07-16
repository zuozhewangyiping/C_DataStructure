#include <stdio.h>
#include <stdlib.h>
#include "ds_bplustree.h"

static void visit_entry(const DS_BPLUSTREE_KEY_TYPE *key,
                         const DS_BPLUSTREE_VALUE_TYPE *v, void *ud)
{
    (void)ud;
    printf("  key=%d, value=%d\n", key->key, v->value);
}

int main(void)
{
    const char *filename = "_bpt_demo.db";

    DS_BPlusTree *bpt = ds_bplustree_create(filename);

    printf("is_empty: %d, size: %d, level: %d\n",
           ds_bplustree_is_empty(bpt), ds_bplustree_size(bpt), ds_bplustree_level(bpt));

    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 30}, (DS_BPLUSTREE_VALUE_TYPE){.value = 300});
    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 10}, (DS_BPLUSTREE_VALUE_TYPE){.value = 100});
    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 50}, (DS_BPLUSTREE_VALUE_TYPE){.value = 500});
    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 20}, (DS_BPLUSTREE_VALUE_TYPE){.value = 200});
    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 40}, (DS_BPLUSTREE_VALUE_TYPE){.value = 400});
    printf("after 5 inserts: size=%d, level=%d\n",
           ds_bplustree_size(bpt), ds_bplustree_level(bpt));

    printf("traverse:\n");
    ds_bplustree_traverse(bpt, NULL, visit_entry);

    BPlusTreeNode *cur = ds_bplustree_search(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 30});
    DS_BPLUSTREE_KEY_TYPE *key;
    DS_BPLUSTREE_VALUE_TYPE *val;
    ds_bplustree_node_get_data(cur, &key, &val);
    printf("search(30): key=%d, value=%d\n", key->key, val->value);
    free(cur);

    cur = ds_bplustree_find_min(bpt);
    ds_bplustree_node_get_data(cur, &key, &val);
    printf("min: key=%d, value=%d\n", key->key, val->value);
    free(cur);

    cur = ds_bplustree_find_max(bpt);
    ds_bplustree_node_get_data(cur, &key, &val);
    printf("max: key=%d, value=%d\n", key->key, val->value);
    free(cur);

    cur = ds_bplustree_find_min(bpt);
    BPlusTreeNode *next = ds_bplustree_successor(bpt, cur);
    ds_bplustree_node_get_data(next, &key, &val);
    printf("successor(min): key=%d\n", key->key);
    free(cur);
    free(next);

    cur = ds_bplustree_find_max(bpt);
    next = ds_bplustree_predecessor(bpt, cur);
    ds_bplustree_node_get_data(next, &key, &val);
    printf("predecessor(max): key=%d\n", key->key);
    free(cur);
    free(next);

    printf("range_query [25, 45]:\n");
    ds_bplustree_range_query(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 25},
                             (DS_BPLUSTREE_KEY_TYPE){.key = 45}, NULL, visit_entry);

    int judge;
    DS_BPlusTree *clone = ds_bplustree_clone(bpt, "_bpt_clone.db", &judge);
    printf("clone: size=%d, level=%d\n",
           ds_bplustree_size(clone), ds_bplustree_level(clone));

    ds_bplustree_delete(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 30});
    ds_bplustree_delete(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 10});
    printf("after delete 30 and 10: size=%d\n", ds_bplustree_size(bpt));

    ds_bplustree_destroy(bpt);
    ds_bplustree_destroy(clone);

    remove(filename);
    remove("_bpt_clone.db");
    return 0;
}
