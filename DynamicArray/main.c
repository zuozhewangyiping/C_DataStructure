#include <stdio.h>
#include "ds_dynamicarray.h"

int main(void)
{
    DS_DynamicArray *da = ds_dynamicarray_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_dynamicarray_is_empty(da), ds_dynamicarray_size(da), ds_dynamicarray_capacity(da));

    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){10});
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){20});
    ds_dynamicarray_insert(da, 1, (ds_dynamicarray_type){15});

    printf("after push/insert: size=%d, capacity=%d\n",
           ds_dynamicarray_size(da), ds_dynamicarray_capacity(da));

    ds_dynamicarray_reserve(da, 10);
    printf("after reserve(10): capacity=%d\n", ds_dynamicarray_capacity(da));

    DS_DYNAMICARRAY_TYPE *ptr;
    ds_dynamicarray_get(da, 1, &ptr);
    printf("get[1]=%d\n", ptr->data);

    ds_dynamicarray_set(da, 1, (ds_dynamicarray_type){99});
    ds_dynamicarray_get(da, 1, &ptr);
    printf("after set[1]=99: %d\n", ptr->data);

    ds_dynamicarray_find(da, 10, &ptr);
    printf("find(10): data=%d\n", ptr->data);

    ds_dynamicarray_shrink_to_fit(da);
    printf("after shrink_to_fit: capacity=%d\n", ds_dynamicarray_capacity(da));

    ds_dynamicarray_pop_back(da, &ptr);
    printf("pop_back: %d\n", ptr->data);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(*ptr);

    ds_dynamicarray_erase(da, 0, &ptr);
    printf("erase[0]: %d\n", ptr->data);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(*ptr);

    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){100});
    ds_dynamicarray_pop_back_and_destroy(da);
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){200});
    ds_dynamicarray_erase_and_destroy(da, 0);

    int judge;
    DS_DynamicArray *clone = ds_dynamicarray_clone(da, &judge);
    printf("clone: size=%d, capacity=%d\n",
           ds_dynamicarray_size(clone), ds_dynamicarray_capacity(clone));

    DS_DynamicArray *da2 = ds_dynamicarray_create();
    ds_dynamicarray_push_back(da2, (ds_dynamicarray_type){300});
    ds_dynamicarray_concat(da, da2);
    printf("after concat: da size=%d, da2 is_empty=%d\n",
           ds_dynamicarray_size(da), ds_dynamicarray_is_empty(da2));

    ds_dynamicarray_destroy(da);
    ds_dynamicarray_destroy(da2);
    ds_dynamicarray_destroy(clone);
    return 0;
}
