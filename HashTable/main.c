#include <stdio.h>
#include <stdlib.h>
#include "ds_hashtable.h"

void print_entry(DS_HASHTABLE_TYPE *value, void *user_data)
{
    (void)user_data;
    printf("  [%d] => %d\n", value->key, value->value);
}

int main(void)
{
    DS_HashTable *ht = ds_hashtable_create();

    /* insert — fails on duplicate key */
    ds_hashtable_insert(ht, (ds_hashtable_type){1, 100});
    ds_hashtable_insert(ht, (ds_hashtable_type){2, 200});
    ds_hashtable_insert(ht, (ds_hashtable_type){3, 300});
    ds_hashtable_insert(ht, (ds_hashtable_type){1, 999}); /* duplicate key, ignored */

    printf("size=%d, capacity=%d\n", ds_hashtable_size(ht), ds_hashtable_capacity(ht));

    /* put — overwrites if key exists */
    ds_hashtable_put(ht, (ds_hashtable_type){2, 222});

    /* find */
    DS_HASHTABLE_TYPE *found;
    if (ds_hashtable_find(ht, 2, &found))
        printf("find key=2: value=%d\n", found->value);

    /* traverse */
    printf("traverse:\n");
    ds_hashtable_traverse(ht, NULL, print_entry);

    /* erase */
    DS_HASHTABLE_TYPE *erased;
    if (ds_hashtable_erase(ht, 3, &erased))
    {
        printf("erased key=%d, value=%d\n", erased->key, erased->value);
        DS_HASHTABLE_DESTROY_ELEMENT(*erased);
        free(erased);
    }

    /* clone */
    int judge;
    DS_HashTable *copy = ds_hashtable_clone(ht, &judge);
    if (judge)
    {
        printf("clone size=%d\n", ds_hashtable_size(copy));
        ds_hashtable_destroy(copy);
    }

    ds_hashtable_destroy(ht);
    return 0;
}
