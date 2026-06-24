#include <stdio.h>
#include "ds_skiplist.h"

static void visit_entry(DS_SKIPLIST_TYPE *v, void *ud)
{
    (void)ud;
    printf("  key=%d, value=%d\n", v->key, v->value);
}

int main(void)
{
    DS_SkipList *skl = ds_skiplist_create();

    printf("is_empty: %d, size: %d, level: %d\n",
           ds_skiplist_is_empty(skl), ds_skiplist_size(skl), ds_skiplist_level(skl));

    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 30, .value = 300});
    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 10, .value = 100});
    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 50, .value = 500});
    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 20, .value = 200});
    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 40, .value = 400});
    printf("after 5 inserts: size=%d, level=%d\n",
           ds_skiplist_size(skl), ds_skiplist_level(skl));

    printf("traverse:\n");
    ds_skiplist_traverse(skl, NULL, visit_entry);

    SkipListNode *cur = ds_skiplist_search(skl, (DS_SKIPLIST_TYPE){.key = 30});
    DS_SKIPLIST_TYPE *data;
    ds_skiplist_node_get_data(cur, &data);
    printf("search(30): key=%d, value=%d\n", data->key, data->value);

    cur = ds_skiplist_find_min(skl);
    ds_skiplist_node_get_data(cur, &data);
    printf("min: key=%d\n", data->key);

    cur = ds_skiplist_find_max(skl);
    ds_skiplist_node_get_data(cur, &data);
    printf("max: key=%d\n", data->key);

    cur = ds_skiplist_find_min(skl);
    cur = ds_skiplist_successor(skl, cur);
    ds_skiplist_node_get_data(cur, &data);
    printf("successor(min): key=%d\n", data->key);

    cur = ds_skiplist_find_max(skl);
    cur = ds_skiplist_predecessor(skl, cur);
    ds_skiplist_node_get_data(cur, &data);
    printf("predecessor(max): key=%d\n", data->key);

    printf("range_query [25, 45]:\n");
    ds_skiplist_range_query(skl, (DS_SKIPLIST_TYPE){.key = 25},
                            (DS_SKIPLIST_TYPE){.key = 45}, NULL, visit_entry);

    int judge;
    DS_SkipList *clone = ds_skiplist_clone(skl, &judge);
    printf("clone: size=%d, level=%d\n",
           ds_skiplist_size(clone), ds_skiplist_level(clone));

    ds_skiplist_delete(skl, (DS_SKIPLIST_TYPE){.key = 30});
    ds_skiplist_delete(skl, (DS_SKIPLIST_TYPE){.key = 10});
    printf("after delete 30 and 10: size=%d\n", ds_skiplist_size(skl));

    ds_skiplist_destroy(skl);
    ds_skiplist_destroy(clone);
    return 0;
}
