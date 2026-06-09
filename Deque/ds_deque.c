#include <stdlib.h>
#include "ds_deque.h"

struct DS_Deque
{
    DS_DEQUE_TYPE *data;
    int front;
    int rear;
    int size;
    int capacity;
};

DS_Deque *ds_deque_create(void)
{
    DS_Deque *dq = (DS_Deque *)malloc(sizeof(DS_Deque));
    if (dq == NULL)
    {
        return NULL;
    }

    dq->data = NULL;
    dq->front = 0;
    dq->rear = 0;
    dq->size = 0;
    dq->capacity = 0;

    return dq;
}

void ds_deque_destroy(DS_Deque *dq)
{
    if (dq == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < dq->size; i++)
    {
        DS_DEQUE_DESTROY_ELEMENT(dq->data[(dq->front + i) % dq->capacity]);
    }

    free(dq->data);
    free(dq);
}

int ds_deque_size(const DS_Deque *dq)
{
    if (dq == NULL)
    {
        return -1;
    }

    return dq->size;
}

int ds_deque_capacity(const DS_Deque *dq)
{
    if (dq == NULL)
    {
        return -1;
    }

    return dq->capacity;
}

int ds_deque_is_empty(const DS_Deque *dq)
{
    if (dq == NULL)
    {
        return -1;
    }

    return (dq->size == 0 ? 1 : 0);
}

int ds_deque_get(DS_Deque *dq, int index, DS_DEQUE_TYPE **x)
{
    if (x == NULL || dq == NULL || index < 0 || index >= dq->size)
    {
        return 0;
    }

    *x = &(dq->data[(dq->front + index) % dq->capacity]);

    return 1;
}

int ds_deque_set(DS_Deque *dq, int index, DS_DEQUE_TYPE value)
{
    if (dq == NULL || index < 0 || index >= dq->size)
    {
        return 0;
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DEQUE_TYPE temp = DS_DEQUE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DEQUE_DESTROY_ELEMENT(temp);
        return 0;
    }

    int pos = (dq->front + index) % dq->capacity;

    // step 2.彻底摧毁原来dq->data[pos]里的旧data
    DS_DEQUE_DESTROY_ELEMENT(dq->data[pos]);

    // step 3.新temp放入位置
    dq->data[pos] = temp;

    return 1;
}

int ds_deque_find(DS_Deque *dq, DS_DEQUE_MATCH_TYPE target, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL)
    {
        return 0;
    }

    int i = 0;
    for (i = 0; i < dq->size; i++)
    {
        int pos = (dq->front + i) % dq->capacity;

        if (DS_DEQUE_MATCH(dq->data[pos], target))
        {
            *x = &(dq->data[pos]);

            return 1;
        }
    }

    return 0;
}

int ds_deque_peek_front(DS_Deque *dq, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL || dq->size == 0)
    {
        return 0;
    }

    *x = &(dq->data[dq->front]);

    return 1;
}

int ds_deque_peek_back(DS_Deque *dq, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL || dq->size == 0)
    {
        return 0;
    }

    *x = &(dq->data[(dq->capacity + dq->rear - 1) % dq->capacity]);

    return 1;
}

static int capacity_expansion(DS_Deque *dq)
{
    if (dq == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (dq->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * dq->capacity;
    }

    DS_DEQUE_TYPE *temp = (DS_DEQUE_TYPE *)malloc(new_capacity * sizeof(DS_DEQUE_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    int i;
    for (i = 0; i < dq->size; i++)
    {
        temp[i] = dq->data[(dq->front + i) % dq->capacity];
    }

    free(dq->data);

    dq->data = temp;
    dq->front = 0;
    dq->rear = dq->size;
    dq->capacity = new_capacity;

    return 1;
}

int ds_deque_push_front(DS_Deque *dq, DS_DEQUE_TYPE value)
{
    if (dq == NULL)
    {
        return 0;
    }

    if (dq->size >= dq->capacity)
    {
        if (!capacity_expansion(dq))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DEQUE_TYPE temp = DS_DEQUE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DEQUE_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    dq->data[(dq->capacity + dq->front - 1) % dq->capacity] = temp;

    // step 3.更新front的位置
    dq->front = (dq->capacity + dq->front - 1) % dq->capacity;

    dq->size++;

    return 1;
}

int ds_deque_push_back(DS_Deque *dq, DS_DEQUE_TYPE value)
{
    if (dq == NULL)
    {
        return 0;
    }

    if (dq->size >= dq->capacity)
    {
        if (!capacity_expansion(dq))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DEQUE_TYPE temp = DS_DEQUE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DEQUE_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    dq->data[dq->rear] = temp;

    // step 3.更新rear的位置
    dq->rear = (dq->rear + 1) % dq->capacity;

    dq->size++;

    return 1;
}

int ds_deque_insert(DS_Deque *dq, int index, DS_DEQUE_TYPE value)
{
    if (dq == NULL || index < 0 || index > dq->size)
    {
        return 0;
    }

    if (dq->size >= dq->capacity)
    {
        if (!capacity_expansion(dq))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_DEQUE_TYPE temp = DS_DEQUE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DEQUE_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.按需选择哪半边移动
    int i;
    if (index <= dq->size / 2)
    {
        dq->front = (dq->capacity + dq->front - 1) % dq->capacity;

        for (i = 0; i < index; i++)
        {
            dq->data[(dq->front + i) % dq->capacity] = dq->data[(dq->front + i + 1) % dq->capacity];
        }
    }
    else
    {
        dq->rear = (dq->rear + 1) % dq->capacity;

        for (i = dq->size - 1; i >= index; i--)
        {
            dq->data[(dq->front + i + 1) % dq->capacity] = dq->data[(dq->front + i) % dq->capacity];
        }
    }

    // step 3.新temp放入index位置
    dq->data[(dq->front + index) % dq->capacity] = temp;

    dq->size++;

    return 1;
}

int ds_deque_pop_front(DS_Deque *dq, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL || dq->size == 0)
    {
        return 0;
    }

    *x = &(dq->data[dq->front]);

    dq->front = (dq->front + 1) % dq->capacity;

    dq->size--;

    return 1;
}

int ds_deque_pop_front_and_destroy(DS_Deque *dq)
{
    DS_DEQUE_TYPE *x = NULL;

    if (!ds_deque_pop_front(dq, &x))
    {
        return 0;
    }

    DS_DEQUE_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向数组内部

    return 1;
}

int ds_deque_pop_back(DS_Deque *dq, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL || dq->size == 0)
    {
        return 0;
    }

    *x = &(dq->data[(dq->capacity + dq->rear - 1) % dq->capacity]);

    dq->rear = (dq->capacity + dq->rear - 1) % dq->capacity;

    dq->size--;

    return 1;
}

int ds_deque_pop_back_and_destroy(DS_Deque *dq)
{
    DS_DEQUE_TYPE *x = NULL;

    if (!ds_deque_pop_back(dq, &x))
    {
        return 0;
    }

    DS_DEQUE_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向数组内部

    return 1;
}

int ds_deque_erase(DS_Deque *dq, int index, DS_DEQUE_TYPE **x)
{
    if (dq == NULL || x == NULL || index < 0 || index >= dq->size)
    {
        return 0;
    }

    DS_DEQUE_TYPE temp = dq->data[(dq->front + index) % dq->capacity];

    int i;
    if (index <= dq->size / 2)
    {
        for (i = 0; i < index; i++)
        {
            dq->data[(dq->front + index - i) % dq->capacity] = dq->data[(dq->front + index - i - 1) % dq->capacity];
        }

        dq->data[dq->front] = temp;

        *x = &(dq->data[dq->front]);

        dq->front = (dq->front + 1) % dq->capacity;
    }
    else
    {
        for (i = index; i < dq->size - 1; i++)
        {
            dq->data[(dq->front + i) % dq->capacity] = dq->data[(dq->front + i + 1) % dq->capacity];
        }

        dq->rear = (dq->capacity + dq->rear - 1) % dq->capacity;

        dq->data[dq->rear] = temp;

        *x = &(dq->data[dq->rear]);
    }

    dq->size--;

    return 1;
}

int ds_deque_erase_and_destroy(DS_Deque *dq, int index)
{
    DS_DEQUE_TYPE *x = NULL;

    if (!ds_deque_erase(dq, index, &x))
    {
        return 0;
    }

    DS_DEQUE_DESTROY_ELEMENT(*x);

    return 1;
}

DS_Deque *ds_deque_clone(const DS_Deque *dq, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (dq == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新队列
    DS_Deque *new_dq = ds_deque_create();
    if (new_dq == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (dq->capacity == 0)
    {
        new_dq->data = NULL;
        new_dq->capacity = 0;
        new_dq->size = 0;

        *judge = 1;
        return new_dq;
    }

    // step 2.为新队列分配内存
    if (!ds_deque_reserve(new_dq, dq->capacity))
    {
        ds_deque_destroy(new_dq);
        *judge = 0;
        return NULL;
    }

    // step 3.依此深拷贝每一个元素
    int i;
    for (i = 0; i < dq->size; i++)
    {
        new_dq->data[i] = DS_DEQUE_CLONE_ELEMENT(dq->data[(dq->front + i) % dq->capacity], judge);
        new_dq->size++;

        if (*judge == 0)
        {
            ds_deque_destroy(new_dq);

            return NULL;
        }
    }

    new_dq->front = 0;
    new_dq->rear = new_dq->size;

    return new_dq;
}

int ds_deque_reserve(DS_Deque *dq, int new_capacity)
{
    // 守卫1: dq NULL 或 新容量不够容纳现有元素 → 拒绝
    if (dq == NULL || new_capacity < dq->size)
    {
        return 0;
    }

    // 守卫2: 容量不变 → 无事可做
    if (new_capacity == dq->capacity)
    {
        return 1;
    }

    // 分支1: 要求缩到 0
    if (new_capacity == 0)
    {
        free(dq->data);
        dq->data = NULL;
        dq->capacity = 0;
        dq->front = 0;
        dq->rear = 0;

        return 1;
    }

    // 分支2: 正常扩/缩容
    // step 1.申请新一块内存
    DS_DEQUE_TYPE *new_data = (DS_DEQUE_TYPE *)malloc(new_capacity * sizeof(DS_DEQUE_TYPE));
    if (new_data == NULL)
    {
        return 0;
    }

    // step 2.浅拷贝元素至新内存处
    int i;
    for (i = 0; i < dq->size; i++)
    {
        new_data[i] = dq->data[(dq->front + i) % dq->capacity];
    }

    // step 3.释放旧内存
    free(dq->data);

    dq->data = new_data;
    dq->capacity = new_capacity;
    dq->front = 0;
    dq->rear = dq->size;

    return 1;
}

int ds_deque_shrink_to_fit(DS_Deque *dq)
{
    if (dq == NULL)
    {
        return 0;
    }

    return ds_deque_reserve(dq, dq->size);
}
