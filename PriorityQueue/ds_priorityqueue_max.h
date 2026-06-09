#ifndef DS_PRIORITYQUEUE_MAX_H
#define DS_PRIORITYQUEUE_MAX_H

#include "ds_priorityqueue_max_type.h"
#define DS_PRIORITYQUEUE_MAX_TYPE ds_priorityqueue_max_type

typedef struct DS_PriorityQueue_max DS_PriorityQueue_max;

DS_PriorityQueue_max *ds_priorityqueue_max_create(void); /* return pointer if success, NULL if malloc failed */
void ds_priorityqueue_max_destroy(DS_PriorityQueue_max *pq);

int ds_priorityqueue_max_size(const DS_PriorityQueue_max *pq);     /* return number of elements, -1 if pq is NULL */
int ds_priorityqueue_max_capacity(const DS_PriorityQueue_max *pq); /* return capacity, -1 if pq is NULL */
int ds_priorityqueue_max_is_empty(const DS_PriorityQueue_max *pq); /* return 1 if empty, 0 if not empty, -1 if pq is NULL */

int ds_priorityqueue_max_peek(DS_PriorityQueue_max *pq, DS_PRIORITYQUEUE_MAX_TYPE **x); /* return 1 if success, 0 if pq is NULL or empty or x NULL */
/*
    DS_PRIORITYQUEUE_MAX_TYPE *top;

    if (ds_priorityqueue_max_peek(pq, &top))
    {
        printf("max=%d\n", top->data); // 可以读取
        top->data = 999;               // 可以修改（可能破坏堆性质，需谨慎）
    }

    // WARNING: top 在下次 push / pop / reserve 后可能失效
*/

int ds_priorityqueue_max_push(DS_PriorityQueue_max *pq, DS_PRIORITYQUEUE_MAX_TYPE value); /* return 1 if success, 0 if realloc failed or pq NULL */
/*
    ds_priorityqueue_max_push(pq, (DS_PRIORITYQUEUE_MAX_TYPE){100});
*/

int ds_priorityqueue_max_pop(DS_PriorityQueue_max *pq, DS_PRIORITYQUEUE_MAX_TYPE **x); /* return 1 if success, 0 if pq NULL, x NULL, or pq empty */
int ds_priorityqueue_max_pop_and_destroy(DS_PriorityQueue_max *pq);                    /* return 1 if success, 0 if pq NULL, or pq empty */
/*
    DS_PRIORITYQUEUE_MAX_TYPE *data;

    if (ds_priorityqueue_max_pop(pq, &data))
    {
        printf("max=%d\n", data->data); // 可以读取

        // 如果元素类型含指针成员（如 char *name），需手动调用：
    }

    // 用完后要记得释放 data（如下，使用了_and_destroy版本则不用）
    DS_PRIORITYQUEUE_MAX_DESTROY_ELEMENT(*data);

    // WARNING_1: data 指向堆内部已移出的位置，不能 free(data)；
    // WARNING_2: data 在下次 push / reserve 后可能失效；
    // WARNING_3: data 在下次 push 后，会因其指向的位置被覆盖而失效。
*/

/* return pointer if success, NULL if malloc failed or judge NULL; *judge will be 1 if clone successfully (even if pq NULL), 0 if malloc failed */
DS_PriorityQueue_max *ds_priorityqueue_max_clone(const DS_PriorityQueue_max *pq, int *judge);
/*
    int judge;
    DS_PriorityQueue_max *copy = ds_priorityqueue_max_clone(pq, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

/* return 1 if success, 0 if pq NULL, realloc failed, or new_capacity smaller than current size */
int ds_priorityqueue_max_reserve(DS_PriorityQueue_max *pq, int new_capacity);
int ds_priorityqueue_max_shrink_to_fit(DS_PriorityQueue_max *pq); /* return 1 if success, 0 if pq NULL or realloc failed */
/*
    ds_priorityqueue_max_reserve(pq, 100);   // 预留 100 个元素空间，避免多次扩容
    ds_priorityqueue_max_shrink_to_fit(pq);  // 回收多余空间，capacity 缩减至 size
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本堆的 push / clone 操作均通过 DS_PRIORITYQUEUE_MAX_CLONE_ELEMENT
 * 宏对数据做深拷贝，堆内部持有独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，堆里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_PRIORITYQUEUE_MAX_DESTROY_ELEMENT 释放。
 *
 * 关于 peek / pop 返回的指针：
 *
 *   - peek 返回的指针指向堆顶元素，可直接读取和修改。修改堆顶可能破坏堆性质，需谨慎。
 *     此指针在下一次 push / pop / reserve 后可能失效。
 *   - pop 返回的指针指向堆内部"边界外"位置（已移出的元素），不能对此指针调用
 *     free()。此指针在下一次 push 时会被覆盖。
 *   - 使用 pop_and_destroy 则无需手动处理元素内部资源。
 */
