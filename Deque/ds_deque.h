#ifndef DS_DEQUE_H
#define DS_DEQUE_H

#include "ds_deque_type.h"
#define DS_DEQUE_TYPE ds_deque_type

typedef struct DS_Deque DS_Deque;

DS_Deque *ds_deque_create(void); /* return NULL if malloc failed */
void ds_deque_destroy(DS_Deque *dq);

int ds_deque_size(const DS_Deque *dq);     /* return number of elements, -1 if dq is NULL */
int ds_deque_capacity(const DS_Deque *dq); /* return capacity, -1 if dq is NULL */
int ds_deque_is_empty(const DS_Deque *dq); /* return 1 if empty, 0 if not empty, -1 if dq is NULL */

int ds_deque_get(DS_Deque *dq, int index, DS_DEQUE_TYPE **x); /* return 1 if success, 0 if dq or x NULL, or index out of range */
/*
    DS_DEQUE_TYPE *data_ptr;

    if (ds_deque_get(dq, 0, &data_ptr))
    {
        printf("data=%d\n", data_ptr->data); // 可以读取
        data_ptr->data = 999;                // 可以修改
    }

    // WARNING: data_ptr 在下次 push_back / push_front / insert / reserve 后可能失效
*/

int ds_deque_set(DS_Deque *dq, int index, DS_DEQUE_TYPE value); /* return 1 if success, 0 if dq NULL or index out of range */
/*
    ds_deque_set(dq, 1, (DS_DEQUE_TYPE){999}); // 将索引 1 的元素改为 999
*/

int ds_deque_find(DS_Deque *dq, DS_DEQUE_MATCH_TYPE target, DS_DEQUE_TYPE **x); /* return 1 if success, 0 if (dq or x) NULL or failed to find */
/*
    DS_DEQUE_TYPE *found;

    if (ds_deque_find(dq, 100, &found))
    {
        found->data = 999;  // 可以修改
    }

    // WARNING: 和 get 一样，found 在下次 push / insert / reserve 后可能失效
*/

int ds_deque_peek_front(DS_Deque *dq, DS_DEQUE_TYPE **x); /* return 1 if success, 0 if dq or x NULL, or dq empty */
int ds_deque_peek_back(DS_Deque *dq, DS_DEQUE_TYPE **x);  /* return 1 if success, 0 if dq or x NULL, or dq empty */
/*
    DS_DEQUE_TYPE *look;

    if (ds_deque_peek_XXX(dq, &look))
    {
        printf("look=%d\n", look->data); // 可以读取
        look->data = 999;                 // 可以修改
    }

    // WARNING: 指针在下次 push / pop / reserve 后可能失效
*/

int ds_deque_push_front(DS_Deque *dq, DS_DEQUE_TYPE value);        /* return 1 if success, 0 if realloc failed or dq NULL */
int ds_deque_push_back(DS_Deque *dq, DS_DEQUE_TYPE value);         /* return 1 if success, 0 if realloc failed or dq NULL */
int ds_deque_insert(DS_Deque *dq, int index, DS_DEQUE_TYPE value); /* return 1 if success, 0 if dq NULL, index out of range, or realloc failed */
/*
    ds_deque_push_XXX(dq, (DS_DEQUE_TYPE){100});
*/

int ds_deque_pop_front(DS_Deque *dq, DS_DEQUE_TYPE **x);        /* return 1 if success, 0 if dq NULL, x NULL, or dq empty */
int ds_deque_pop_front_and_destroy(DS_Deque *dq);               /* return 1 if success, 0 if dq NULL, or dq empty */
int ds_deque_pop_back(DS_Deque *dq, DS_DEQUE_TYPE **x);         /* return 1 if success, 0 if dq NULL, x NULL, or dq empty */
int ds_deque_pop_back_and_destroy(DS_Deque *dq);                /* return 1 if success, 0 if dq NULL, or dq empty */
int ds_deque_erase(DS_Deque *dq, int index, DS_DEQUE_TYPE **x); /* return 1 if success, 0 if dq NULL, x NULL, or index out of range */
int ds_deque_erase_and_destroy(DS_Deque *dq, int index);        /* return 1 if success, 0 if dq NULL, or index out of range */
/*
    DS_DEQUE_TYPE *user_data;

    if (ds_deque_pop_XXX(dq, &user_data))
    {
        printf("pop_XXX=%d\n", user_data->data);
    }

    // 用完后要记得释放 user_data（使用了_and_destroy版本则不用）
    DS_DEQUE_DESTROY_ELEMENT(*user_data);

    // 以下警告对于 pop 和 erase 都适用：
    // WARNING_1: user_data 指向 deque 内部已移出的位置，不能 free(user_data)；
    // WARNING_2: user_data 在下次 push / reserve 后可能失效；
    // WARNING_3: user_data 在下次 push 后，会因其指向的位置被覆盖而失效。
*/

DS_Deque *ds_deque_clone(const DS_Deque *dq, int *judge); /* return pointer if success, NULL if malloc failed or judge is NULL; *judge will be 1 if clone successfully (even if dq is NULL), 0 if malloc failed */
/*
    int judge;
    DS_Deque *copy = ds_deque_clone(dq, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_deque_reserve(DS_Deque *dq, int new_capacity); /* return 1 if success, 0 if dq NULL, realloc failed, or new_capacity smaller than current size */
int ds_deque_shrink_to_fit(DS_Deque *dq);             /* return 1 if success, 0 if dq NULL or realloc failed */
/*
    ds_deque_reserve(dq, 100);   // 预留 100 个槽位，避免多次扩容
    ds_deque_shrink_to_fit(dq);  // 回收多余槽位，capacity 缩减至 size
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本 deque 的 push_front / push_back / insert / set / clone 操作均通过
 * DS_DEQUE_CLONE_ELEMENT 宏对数据做深拷贝，deque 内部持有独立副本。
 * 用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，deque 里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_DEQUE_DESTROY_ELEMENT 释放。
 *
 * 关于 get / find / peek_front / peek_back 返回的指针：
 *
 *   - 返回的指针指向 deque 内部数据，可直接读取和修改。此指针在下一次导致扩容的
 *     操作（push / insert / reserve）后可能失效。
 *
 * 关于 pop_front / pop_back / erase 返回的指针：
 *
 *   - 返回的指针指向 deque 内部"边界外"位置（已移出的元素），不能对此指针调用
 *     free()。此指针在下一次 push / insert 时会被覆盖。
 *   - 使用 _and_destroy 变体则无需手动处理元素内部资源。
 */
