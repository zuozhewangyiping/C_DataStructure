#include <stdlib.h>
#include "ds_dynamicarray.h"

struct DS_DynamicArray
{
    DS_DYNAMICARRAY_TYPE *data;
    int size;
    int capacity;
};

DS_DynamicArray *ds_dynamicarray_create(void)
{
    DS_DynamicArray *da = (DS_DynamicArray *)malloc(sizeof(DS_DynamicArray));
    if (da == NULL)
    {
        return NULL;
    }

    da->data = NULL;
    da->capacity = 0;
    da->size = 0;

    return da;
}

void ds_dynamicarray_destroy(DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < da->size; i++)
    {
        DS_DYNAMICARRAY_DESTROY_ELEMENT(da->data[i]);
    }

    free(da->data);
    free(da);
}

int ds_dynamicarray_size(const DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return -1;
    }

    return da->size;
}

int ds_dynamicarray_capacity(const DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return -1;
    }

    return da->capacity;
}

int ds_dynamicarray_is_empty(const DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return -1;
    }

    return (da->size == 0 ? 1 : 0);
}

int ds_dynamicarray_get(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE **x)
{
    if (x == NULL || da == NULL || index < 0 || index >= da->size)
    {
        return 0;
    }

    *x = &(da->data[index]);

    return 1;
}

int ds_dynamicarray_set(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE value)
{
    if (da == NULL || index < 0 || index >= da->size)
    {
        return 0;
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DYNAMICARRAY_TYPE temp = DS_DYNAMICARRAY_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DYNAMICARRAY_DESTROY_ELEMENT(temp);
        return 0;
    }

    // step 2.彻底摧毁原来da->data[index]里的旧data
    DS_DYNAMICARRAY_DESTROY_ELEMENT(da->data[index]);

    // step 3.新temp放入位置
    da->data[index] = temp;

    return 1;
}

int ds_dynamicarray_find(DS_DynamicArray *da, DS_DYNAMICARRAY_MATCH_TYPE target, DS_DYNAMICARRAY_TYPE **x)
{
    if (da == NULL || x == NULL)
    {
        return 0;
    }

    int i;
    for (i = 0; i < da->size; i++)
    {
        if (DS_DYNAMICARRAY_MATCH(da->data[i], target))
        {
            *x = &(da->data[i]);

            return 1;
        }
    }

    return 0;
}

static int capacity_expansion(DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (da->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * da->capacity;
    }

    DS_DYNAMICARRAY_TYPE *temp = (DS_DYNAMICARRAY_TYPE *)realloc(da->data, new_capacity * sizeof(DS_DYNAMICARRAY_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    da->data = temp;
    da->capacity = new_capacity;

    return 1;
}

int ds_dynamicarray_push_back(DS_DynamicArray *da, DS_DYNAMICARRAY_TYPE value)
{
    if (da == NULL)
    {
        return 0;
    }

    if (da->size >= da->capacity)
    {
        if (!capacity_expansion(da))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DYNAMICARRAY_TYPE temp = DS_DYNAMICARRAY_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DYNAMICARRAY_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    da->data[da->size] = temp;

    da->size++;

    return 1;
}

int ds_dynamicarray_insert(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE value)
{
    if (da == NULL || index < 0 || index > da->size)
    {
        return 0;
    }

    if (da->size >= da->capacity)
    {
        if (!capacity_expansion(da))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DYNAMICARRAY_TYPE temp = DS_DYNAMICARRAY_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DYNAMICARRAY_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.将index以后的元素都往后平移一位（从后往前依此）
    int i;
    for (i = da->size; i > index; i--)
    {
        da->data[i] = da->data[i - 1];
    }

    // step 3.新temp放入index位置
    da->data[index] = temp;

    da->size++;

    return 1;
}

int ds_dynamicarray_pop_back(DS_DynamicArray *da, DS_DYNAMICARRAY_TYPE **x)
{
    if (da == NULL || x == NULL || da->size == 0)
    {
        return 0;
    }

    *x = &(da->data[da->size - 1]);

    da->size--;

    return 1;
}

int ds_dynamicarray_pop_back_and_destroy(DS_DynamicArray *da)
{
    DS_DYNAMICARRAY_TYPE *x = NULL;

    if (!ds_dynamicarray_pop_back(da, &x))
    {
        return 0;
    }

    DS_DYNAMICARRAY_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向数组内部

    return 1;
}

int ds_dynamicarray_erase(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE **x)
{
    if (da == NULL || x == NULL || index < 0 || index >= da->size)
    {
        return 0;
    }

    DS_DYNAMICARRAY_TYPE temp = da->data[index];

    int i;
    for (i = index; i < da->size - 1; i++)
    {
        da->data[i] = da->data[i + 1];
    }

    da->data[da->size - 1] = temp;

    *x = &(da->data[da->size - 1]);

    da->size--;

    return 1;
}

int ds_dynamicarray_erase_and_destroy(DS_DynamicArray *da, int index)
{
    DS_DYNAMICARRAY_TYPE *x = NULL;

    if (!ds_dynamicarray_erase(da, index, &x))
    {
        return 0;
    }

    DS_DYNAMICARRAY_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向数组内部

    return 1;
}

DS_DynamicArray *ds_dynamicarray_clone(const DS_DynamicArray *da, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (da == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新数组
    DS_DynamicArray *new_da = ds_dynamicarray_create();
    if (new_da == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (da->capacity == 0)
    {
        new_da->data = NULL;
        new_da->capacity = 0;
        new_da->size = 0;

        *judge = 1;
        return new_da;
    }

    // step 2.为新数组分配内存
    if (!ds_dynamicarray_reserve(new_da, da->capacity))
    {
        ds_dynamicarray_destroy(new_da);
        *judge = 0;
        return NULL;
    }

    // step 3.依此深拷贝每一个元素
    int i;
    for (i = 0; i < da->size; i++)
    {
        new_da->data[i] = DS_DYNAMICARRAY_CLONE_ELEMENT(da->data[i], judge);
        new_da->size = i + 1;
        if (*judge == 0)
        {
            ds_dynamicarray_destroy(new_da);
            return NULL;
        }
    }

    return new_da;
}

int ds_dynamicarray_reserve(DS_DynamicArray *da, int new_capacity)
{
    // 守卫1: da NULL 或 新容量不够容纳现有元素 → 拒绝
    if (da == NULL || new_capacity < da->size)
    {
        return 0;
    }

    // 守卫2: 容量不变 → 无事可做
    if (new_capacity == da->capacity)
    {
        return 1;
    }

    // 分支1: 要求缩到 0
    if (new_capacity == 0)
    {
        free(da->data);
        da->data = NULL;
        da->capacity = 0;

        return 1;
    }

    // 分支2: 正常扩/缩容
    DS_DYNAMICARRAY_TYPE *new_data = (DS_DYNAMICARRAY_TYPE *)realloc(da->data, new_capacity * sizeof(DS_DYNAMICARRAY_TYPE));
    if (new_data == NULL)
    {
        return 0;
    }

    da->data = new_data;
    da->capacity = new_capacity;

    return 1;
}

int ds_dynamicarray_shrink_to_fit(DS_DynamicArray *da)
{
    if (da == NULL)
    {
        return 0;
    }

    return ds_dynamicarray_reserve(da, da->size);
}

int ds_dynamicarray_concat(DS_DynamicArray *da1, DS_DynamicArray *da2)
{
    if (da1 == NULL || da1 == da2)
    {
        return 0;
    }
    if (da2 == NULL || da2->size == 0)
    {
        return 1;
    }

    // step 1.可能的扩容
    if (da1->capacity < da1->size + da2->size)
    {
        if (!ds_dynamicarray_reserve(da1, da1->size + da2->size))
        {
            return 0;
        }
    }

    // step 2.深拷贝 da2 的每一个元素至 da1 末尾
    int i;
    for (i = 0; i < da2->size; i++)
    {
        int judge = 1;
        da1->data[da1->size + i] = DS_DYNAMICARRAY_CLONE_ELEMENT(da2->data[i], &judge);
        if (judge == 0) // 若拷贝失败，回滚所有已拷贝部分
        {
            int k;
            for (k = 0; k < i; k++)
            {
                DS_DYNAMICARRAY_DESTROY_ELEMENT(da1->data[da1->size + k]);
            }

            return 0;
        }
    }

    da1->size += da2->size;

    // step 3.将 da2 置空（capacity 还在，元素销毁，size 置零）
    for (i = 0; i < da2->size; i++)
    {
        DS_DYNAMICARRAY_DESTROY_ELEMENT(da2->data[i]);
    }
    da2->size = 0;

    return 1;
}
