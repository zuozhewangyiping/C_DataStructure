#include <stdio.h>
#include "ds_dynamicarray.h"

int main(void)
{
    DS_DynamicArray *da = ds_dynamicarray_create();

    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){100});
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){200});
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){300});

    DS_DYNAMICARRAY_TYPE *ptr;
    for (int i = 0; i < ds_dynamicarray_size(da); i++)
    {
        ds_dynamicarray_get(da, i, &ptr);
        printf("element[%d]: %d\n", i, ptr->data);
    }

    ds_dynamicarray_destroy(da);
    return 0;
}
