#include <stdlib.h>
#include "ds_priorityqueue_min.h"

struct DS_PriorityQueue_min
{
    DS_PRIORITYQUEUE_MIN_TYPE *data;
    int size;
    int capacity;
};

DS_PriorityQueue_min *ds_priorityqueue_min_create(void)
{
    DS_PriorityQueue_min *pq = (DS_PriorityQueue_min *)malloc(sizeof(DS_PriorityQueue_min));
    if (pq == NULL)
    {
        return NULL;
    }

    pq->data = NULL;
    pq->capacity = 0;
    pq->size = 0;

    return pq;
}

void ds_priorityqueue_min_destroy(DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < pq->size; i++)
    {
        DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(pq->data[i]);
    }

    free(pq->data);
    free(pq);
}

int ds_priorityqueue_min_size(const DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return -1;
    }

    return pq->size;
}

int ds_priorityqueue_min_capacity(const DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return -1;
    }

    return pq->capacity;
}

int ds_priorityqueue_min_is_empty(const DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return -1;
    }

    return (pq->size == 0 ? 1 : 0);
}

int ds_priorityqueue_min_peek(DS_PriorityQueue_min *pq, DS_PRIORITYQUEUE_MIN_TYPE **x)
{
    if (pq == NULL || pq->size == 0 || x == NULL)
    {
        return 0;
    }

    *x = &(pq->data[0]);

    return 1;
}

static int capacity_expansion(DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (pq->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * pq->capacity;
    }

    DS_PRIORITYQUEUE_MIN_TYPE *temp = (DS_PRIORITYQUEUE_MIN_TYPE *)realloc(pq->data, new_capacity * sizeof(DS_PRIORITYQUEUE_MIN_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    pq->data = temp;
    pq->capacity = new_capacity;

    return 1;
}

int ds_priorityqueue_min_push(DS_PriorityQueue_min *pq, DS_PRIORITYQUEUE_MIN_TYPE value)
{
    if (pq == NULL)
    {
        return 0;
    }

    if (pq->size >= pq->capacity)
    {
        if (!capacity_expansion(pq))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_PRIORITYQUEUE_MIN_TYPE temp = DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    pq->data[pq->size] = temp;

    pq->size++;

    int i = pq->size - 1;
    while (i > 0 && DS_PRIORITYQUEUE_MIN_GT(pq->data[(i - 1) / 2], pq->data[i])) // 父>子 时继续循环
    {
        DS_PRIORITYQUEUE_MIN_TYPE temp = pq->data[i];
        pq->data[i] = pq->data[(i - 1) / 2];
        pq->data[(i - 1) / 2] = temp;

        i = (i - 1) / 2;
    }

    return 1;
}

int ds_priorityqueue_min_pop(DS_PriorityQueue_min *pq, DS_PRIORITYQUEUE_MIN_TYPE **x)
{
    if (pq == NULL || x == NULL || pq->size == 0)
    {
        return 0;
    }

    DS_PRIORITYQUEUE_MIN_TYPE temp = pq->data[0];

    pq->data[0] = pq->data[pq->size - 1];

    pq->data[pq->size - 1] = temp;

    *x = &(pq->data[pq->size - 1]);

    pq->size--;

    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left >= pq->size)
        {
            break;
        }

        int child;
        if (right >= pq->size)
        {
            child = left;
        }
        else
        {
            if (DS_PRIORITYQUEUE_MIN_LT(pq->data[right], pq->data[left])) // 若 右孩子<左孩子
            {
                child = right;
            }
            else
            {
                child = left;
            }
        }

        if (DS_PRIORITYQUEUE_MIN_LE(pq->data[i], pq->data[child]))
        {
            break;
        }

        DS_PRIORITYQUEUE_MIN_TYPE temp_ = pq->data[i];
        pq->data[i] = pq->data[child];
        pq->data[child] = temp_;

        i = child;
    }

    return 1;
}

int ds_priorityqueue_min_pop_and_destroy(DS_PriorityQueue_min *pq)
{
    DS_PRIORITYQUEUE_MIN_TYPE *x;

    if (!ds_priorityqueue_min_pop(pq, &x))
    {
        return 0;
    }

    DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(*x);

    return 1;
}

DS_PriorityQueue_min *ds_priorityqueue_min_clone(const DS_PriorityQueue_min *pq, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (pq == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新的最小堆
    DS_PriorityQueue_min *new_pq = ds_priorityqueue_min_create();
    if (new_pq == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (pq->capacity == 0)
    {
        new_pq->data = NULL;
        new_pq->capacity = 0;
        new_pq->size = 0;

        *judge = 1;
        return new_pq;
    }

    // step 2.为新的最小堆分配内存
    if (!ds_priorityqueue_min_reserve(new_pq, pq->capacity))
    {
        ds_priorityqueue_min_destroy(new_pq);
        *judge = 0;
        return NULL;
    }

    // step 3.依此深拷贝每一个元素
    int i;
    for (i = 0; i < pq->size; i++)
    {
        new_pq->data[i] = DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(pq->data[i], judge);
        new_pq->size = i + 1;
        if (*judge == 0)
        {
            ds_priorityqueue_min_destroy(new_pq);
            return NULL;
        }
    }

    return new_pq;
}

int ds_priorityqueue_min_reserve(DS_PriorityQueue_min *pq, int new_capacity)
{
    // 守卫1: pq NULL 或 新容量不够容纳现有元素 → 拒绝
    if (pq == NULL || new_capacity < pq->size)
    {
        return 0;
    }

    // 守卫2: 容量不变 → 无事可做
    if (new_capacity == pq->capacity)
    {
        return 1;
    }

    // 分支1: 要求缩到 0
    if (new_capacity == 0)
    {
        free(pq->data);
        pq->data = NULL;
        pq->capacity = 0;

        return 1;
    }

    // 分支2: 正常扩/缩容
    DS_PRIORITYQUEUE_MIN_TYPE *new_data = (DS_PRIORITYQUEUE_MIN_TYPE *)realloc(pq->data, new_capacity * sizeof(DS_PRIORITYQUEUE_MIN_TYPE));
    if (new_data == NULL)
    {
        return 0;
    }

    pq->data = new_data;
    pq->capacity = new_capacity;

    return 1;
}

int ds_priorityqueue_min_shrink_to_fit(DS_PriorityQueue_min *pq)
{
    if (pq == NULL)
    {
        return 0;
    }

    return ds_priorityqueue_min_reserve(pq, pq->size);
}
