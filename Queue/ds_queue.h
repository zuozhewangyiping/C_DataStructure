#ifndef DS_QUEUE_H
#define DS_QUEUE_H

#include "ds_queue_type.h"
#define DS_QUEUE_TYPE ds_queue_type

typedef struct DS_Queue DS_Queue;

DS_Queue *ds_queue_create(void); /* return NULL if malloc failed */
void ds_queue_destroy(DS_Queue *q);

int ds_queue_size(const DS_Queue *q);     /* return number of elements, -1 if q is NULL */
int ds_queue_capacity(const DS_Queue *q); /* return capacity, -1 if q is NULL */
int ds_queue_is_empty(const DS_Queue *q); /* return 1 if empty, 0 if not empty, -1 if q is NULL */

int ds_queue_peek_front(DS_Queue *q, DS_QUEUE_TYPE **x); /* return 1 if success, 0 if q or x NULL, or q empty */
int ds_queue_peek_back(DS_Queue *q, DS_QUEUE_TYPE **x);  /* return 1 if success, 0 if q or x NULL, or q empty */
/*
    DS_QUEUE_TYPE *look;

    if (ds_queue_peek_XXX(q, &look))
    {
        printf("look=%d\n", look->data); // 可以读取
        look->data = 999;                 // 可以修改
    }

    // WARNING: 指针在下次 push / pop / reserve 后可能失效
*/

int ds_queue_enqueue(DS_Queue *q, DS_QUEUE_TYPE value); /* return 1 if success, 0 if malloc failed or q NULL */
/*
    ds_queue_enqueue(q, (DS_QUEUE_TYPE){100});
*/

int ds_queue_dequeue(DS_Queue *q, DS_QUEUE_TYPE **x); /* return 1 if success, 0 if q NULL, x NULL, or q empty */
int ds_queue_dequeue_and_destroy(DS_Queue *q);        /* return 1 if success, 0 if q NULL, or q empty */
/*
    DS_QUEUE_TYPE *user_data;

    if (ds_queue_dequeue(q, &user_data))
    {
        printf("dequeue=%d\n", user_data->data);
    }

    // 用完后要记得释放 user_data（使用了_and_destroy版本则不用）
    DS_QUEUE_DESTROY_ELEMENT(*user_data);

    // WARNING_1: user_data 指向 queue 内部已移出的位置，不能 free(user_data)；
    // WARNING_2: user_data 在下次 push / reserve 后可能失效；
    // WARNING_3: user_data 在下次 push 后，会因其指向的位置被覆盖而失效。
*/

DS_Queue *ds_queue_clone(const DS_Queue *q, int *judge); /* return pointer if success, NULL if malloc failed or judge is NULL; *judge will be 1 if clone successfully (even if q is NULL), 0 if malloc failed */
/*
    int judge;
    DS_Queue *copy = ds_queue_clone(q, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_queue_reserve(DS_Queue *q, int new_capacity); /* return 1 if success, 0 if q NULL, malloc failed, or new_capacity smaller than current size */
int ds_queue_shrink_to_fit(DS_Queue *q);             /* return 1 if success, 0 if q NULL or malloc failed */
/*
    ds_queue_reserve(q, 100);   // 预留 100 个槽位，避免多次扩容
    ds_queue_shrink_to_fit(q);  // 回收多余槽位，capacity 缩减至 size
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本 queue 的 enqueue / clone 操作均通过
 * DS_QUEUE_CLONE_ELEMENT 宏对数据做深拷贝，queue 内部持有独立副本。
 * 用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，queue 里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_QUEUE_DESTROY_ELEMENT 释放。
 *
 * 关于 peek_front / peek_back 返回的指针：
 *
 *   - 返回的指针指向 queue 内部数据，可直接读取和修改。此指针在下一次导致扩容的
 *     操作（push / reserve）后可能失效。
 *
 * 关于 dequeue 返回的指针：
 *
 *   - 返回的指针指向 queue 内部"边界外"位置（已移出的元素），不能对此指针调用
 *     free()。此指针在下一次 enqueue 时会被覆盖。
 *   - 使用 _and_destroy 变体则无需手动处理元素内部资源。
 */
