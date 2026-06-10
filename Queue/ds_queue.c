#include <stdlib.h>
#include "ds_queue.h"

struct DS_Queue
{
    DS_QUEUE_TYPE *data;
    int front;
    int rear;
    int size;
    int capacity;
};

DS_Queue *ds_queue_create(void)
{
    DS_Queue *q = (DS_Queue *)malloc(sizeof(DS_Queue));
    if (q == NULL)
    {
        return NULL;
    }

    q->data = NULL;
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->capacity = 0;

    return q;
}

void ds_queue_destroy(DS_Queue *q)
{
    if (q == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < q->size; i++)
    {
        DS_QUEUE_DESTROY_ELEMENT(q->data[(q->front + i) % q->capacity]);
    }

    free(q->data);
    free(q);
}

int ds_queue_size(const DS_Queue *q)
{
    if (q == NULL)
    {
        return -1;
    }

    return q->size;
}

int ds_queue_capacity(const DS_Queue *q)
{
    if (q == NULL)
    {
        return -1;
    }

    return q->capacity;
}

int ds_queue_is_empty(const DS_Queue *q)
{
    if (q == NULL)
    {
        return -1;
    }

    return (q->size == 0 ? 1 : 0);
}

int ds_queue_peek_front(DS_Queue *q, DS_QUEUE_TYPE **x)
{
    if (q == NULL || x == NULL || q->size == 0)
    {
        return 0;
    }

    *x = &(q->data[q->front]);

    return 1;
}

int ds_queue_peek_back(DS_Queue *q, DS_QUEUE_TYPE **x)
{
    if (q == NULL || x == NULL || q->size == 0)
    {
        return 0;
    }

    *x = &(q->data[(q->capacity + q->rear - 1) % q->capacity]);

    return 1;
}

static int capacity_expansion(DS_Queue *q)
{
    if (q == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (q->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * q->capacity;
    }

    DS_QUEUE_TYPE *temp = (DS_QUEUE_TYPE *)malloc(new_capacity * sizeof(DS_QUEUE_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    int i;
    for (i = 0; i < q->size; i++)
    {
        temp[i] = q->data[(q->front + i) % q->capacity];
    }

    free(q->data);

    q->data = temp;
    q->front = 0;
    q->rear = q->size;
    q->capacity = new_capacity;

    return 1;
}

int ds_queue_enqueue(DS_Queue *q, DS_QUEUE_TYPE value)
{
    if (q == NULL)
    {
        return 0;
    }

    if (q->size >= q->capacity)
    {
        if (!capacity_expansion(q))
        {
            return 0;
        }
    }

    // step 1.将用户数据value深拷贝至temp（直接创建副本）
    int judge = 1;
    DS_QUEUE_TYPE temp = DS_QUEUE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_QUEUE_DESTROY_ELEMENT(temp); // 清理半成品资源
        /* 扩容可能已成功：capacity 已增大但 size 与 data 均未改变，对外无影响 */
        return 0;
    }

    // step 2.新temp放入新位置
    q->data[q->rear] = temp;

    // step 3.更新rear的位置
    q->rear = (q->rear + 1) % q->capacity;

    q->size++;

    return 1;
}

int ds_queue_dequeue(DS_Queue *q, DS_QUEUE_TYPE **x)
{
    if (q == NULL || x == NULL || q->size == 0)
    {
        return 0;
    }

    *x = &(q->data[q->front]);

    q->front = (q->front + 1) % q->capacity;

    q->size--;

    return 1;
}

int ds_queue_dequeue_and_destroy(DS_Queue *q)
{
    DS_QUEUE_TYPE *x = NULL;

    if (!ds_queue_dequeue(q, &x))
    {
        return 0;
    }

    DS_QUEUE_DESTROY_ELEMENT(*x);
    // 不 free(x)，因为它指向数组内部

    return 1;
}

DS_Queue *ds_queue_clone(const DS_Queue *q, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (q == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新队列
    DS_Queue *new_q = ds_queue_create();
    if (new_q == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (q->capacity == 0)
    {
        new_q->data = NULL;
        new_q->capacity = 0;
        new_q->size = 0;

        *judge = 1;
        return new_q;
    }

    // step 2.为新队列分配内存
    if (!ds_queue_reserve(new_q, q->capacity))
    {
        ds_queue_destroy(new_q);
        *judge = 0;
        return NULL;
    }

    // step 3.依此深拷贝每一个元素
    int i;
    for (i = 0; i < q->size; i++)
    {
        new_q->data[i] = DS_QUEUE_CLONE_ELEMENT(q->data[(q->front + i) % q->capacity], judge);
        new_q->size++;

        if (*judge == 0)
        {
            ds_queue_destroy(new_q);

            return NULL;
        }
    }

    new_q->front = 0;
    new_q->rear = new_q->size;

    return new_q;
}

int ds_queue_reserve(DS_Queue *q, int new_capacity)
{
    // 守卫1: q NULL 或 新容量不够容纳现有元素 → 拒绝
    if (q == NULL || new_capacity < q->size)
    {
        return 0;
    }

    // 守卫2: 容量不变 → 无事可做
    if (new_capacity == q->capacity)
    {
        return 1;
    }

    // 分支1: 要求缩到 0
    if (new_capacity == 0)
    {
        free(q->data);
        q->data = NULL;
        q->capacity = 0;
        q->front = 0;
        q->rear = 0;

        return 1;
    }

    // 分支2: 正常扩/缩容
    // step 1.申请新一块内存
    DS_QUEUE_TYPE *new_data = (DS_QUEUE_TYPE *)malloc(new_capacity * sizeof(DS_QUEUE_TYPE));
    if (new_data == NULL)
    {
        return 0;
    }

    // step 2.浅拷贝元素至新内存处
    int i;
    for (i = 0; i < q->size; i++)
    {
        new_data[i] = q->data[(q->front + i) % q->capacity];
    }

    // step 3.释放旧内存
    free(q->data);

    q->data = new_data;
    q->capacity = new_capacity;
    q->front = 0;
    q->rear = q->size;

    return 1;
}

int ds_queue_shrink_to_fit(DS_Queue *q)
{
    if (q == NULL)
    {
        return 0;
    }

    return ds_queue_reserve(q, q->size);
}
