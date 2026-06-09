#include <stdlib.h>
#include "ds_stack.h"

struct DS_Stack
{
    DS_STACK_TYPE *data;
    int size;
    int capacity;
};

DS_Stack *ds_stack_create(void)
{
    DS_Stack *s = (DS_Stack *)malloc(sizeof(DS_Stack));

    if (s == NULL)
    {
        return NULL;
    }

    s->data = NULL;
    s->size = 0;
    s->capacity = 0;

    return s;
}

void ds_stack_destroy(DS_Stack *s)
{
    if (s == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < s->size; i++)
    {
        DS_STACK_DESTROY_ELEMENT(s->data[i]);
    }

    free(s->data);

    free(s);
}

int ds_stack_size(const DS_Stack *s)
{
    if (s == NULL)
    {
        return -1;
    }

    return s->size;
}

int ds_stack_capacity(const DS_Stack *s)
{
    if (s == NULL)
    {
        return -1;
    }

    return s->capacity;
}

int ds_stack_is_empty(const DS_Stack *s)
{
    if (s == NULL)
    {
        return -1;
    }

    return (s->size == 0 ? 1 : 0);
}

static int capacity_expansion(DS_Stack *s)
{
    if (s == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (s->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * s->capacity;
    }

    DS_STACK_TYPE *temp = (DS_STACK_TYPE *)realloc(s->data, new_capacity * sizeof(DS_STACK_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    s->data = temp;
    s->capacity = new_capacity;

    return 1;
}

int ds_stack_push(DS_Stack *s, DS_STACK_TYPE value)
{
    if (s == NULL)
    {
        return 0;
    }

    if (s->size >= s->capacity)
    {
        if (!capacity_expansion(s))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_STACK_TYPE temp = DS_STACK_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_STACK_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    s->data[s->size] = temp;

    s->size++;

    return 1;
}

int ds_stack_peek(DS_Stack *s, DS_STACK_TYPE **x)
{
    if (s == NULL || x == NULL)
    {
        return 0;
    }

    if (s->size == 0)
    {
        return 0;
    }

    *x = &(s->data[s->size - 1]);

    return 1;
}

int ds_stack_pop(DS_Stack *s, DS_STACK_TYPE **x)
{
    if (s == NULL || x == NULL || s->size == 0)
    {
        return 0;
    }

    *x = &(s->data[s->size - 1]);

    s->size--;

    return 1;
}

int ds_stack_pop_and_destroy(DS_Stack *s)
{
    DS_STACK_TYPE *x = NULL;

    if (!ds_stack_pop(s, &x))
    {
        return 0;
    }

    DS_STACK_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向容器数组内部

    return 1;
}

DS_Stack *ds_stack_clone(const DS_Stack *s, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (s == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新栈
    DS_Stack *new_s = ds_stack_create();
    if (new_s == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (s->capacity == 0)
    {
        new_s->data = NULL;
        new_s->capacity = 0;
        new_s->size = 0;

        *judge = 1;
        return new_s;
    }

    // step 2.为新栈分配内存
    if (!ds_stack_reserve(new_s, s->capacity))
    {
        ds_stack_destroy(new_s);
        *judge = 0;
        return NULL;
    }

    // step 3.依此深拷贝每一个元素
    int i;
    for (i = 0; i < s->size; i++)
    {
        new_s->data[i] = DS_STACK_CLONE_ELEMENT(s->data[i], judge);
        new_s->size = i + 1;
        if (*judge == 0)
        {
            ds_stack_destroy(new_s);
            return NULL;
        }
    }

    return new_s;
}

int ds_stack_reserve(DS_Stack *s, int new_capacity)
{
    // 守卫1: s NULL 或 新容量不够容纳现有元素 → 拒绝
    if (s == NULL || new_capacity < s->size)
    {
        return 0;
    }

    // 守卫2: 容量不变 → 无事可做
    if (new_capacity == s->capacity)
    {
        return 1;
    }

    // 分支1: 要求缩到 0
    if (new_capacity == 0)
    {
        free(s->data);
        s->data = NULL;
        s->capacity = 0;

        return 1;
    }

    // 分支2: 正常扩/缩容
    DS_STACK_TYPE *new_data = (DS_STACK_TYPE *)realloc(s->data, new_capacity * sizeof(DS_STACK_TYPE));
    if (new_data == NULL)
    {
        return 0;
    }

    s->data = new_data;
    s->capacity = new_capacity;

    return 1;
}

int ds_stack_shrink_to_fit(DS_Stack *s)
{
    if (s == NULL)
    {
        return 0;
    }

    return ds_stack_reserve(s, s->size);
}
