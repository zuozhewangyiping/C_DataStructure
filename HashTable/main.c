#include <stdio.h>
#include <stdlib.h>
#include "ds_hashtable.h"

static void print_entry(DS_HASHTABLE_TYPE *v, void *ud)
{
    (void)ud;
    printf("  [%d]=%d\n", v->key, v->value);
}

int main(void)
{
    DS_HashTable *ht = ds_hashtable_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_hashtable_is_empty(ht), ds_hashtable_size(ht), ds_hashtable_capacity(ht));

    ds_hashtable_insert(ht, (ds_hashtable_type){1, 100});
    ds_hashtable_insert(ht, (ds_hashtable_type){2, 200});
    ds_hashtable_put(ht, (ds_hashtable_type){2, 222});
    ds_hashtable_put(ht, (ds_hashtable_type){3, 300});
    printf("after insert/put: size=%d\n", ds_hashtable_size(ht));

    DS_HASHTABLE_TYPE *found;
    ds_hashtable_find(ht, 2, &found);
    printf("find(2): value=%d\n", found->value);

    printf("traverse:\n");
    ds_hashtable_traverse(ht, NULL, print_entry);

    ds_hashtable_reserve(ht, 10);
    printf("after reserve(10): capacity=%d\n", ds_hashtable_capacity(ht));

    ds_hashtable_erase(ht, 3, &found);
    printf("erase(3): key=%d, value=%d\n", found->key, found->value);
    DS_HASHTABLE_DESTROY_ELEMENT(*found);
    free(found);

    ds_hashtable_insert(ht, (ds_hashtable_type){4, 400});
    ds_hashtable_erase_and_destroy(ht, 4);
    printf("after erase_and_destroy(4): size=%d\n", ds_hashtable_size(ht));

    int judge;
    DS_HashTable *clone = ds_hashtable_clone(ht, &judge);
    printf("clone: size=%d\n", ds_hashtable_size(clone));

    ds_hashtable_destroy(ht);
    ds_hashtable_destroy(clone);
    return 0;
}
