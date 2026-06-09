#ifndef DS_STACK_H
#define DS_STACK_H

#include "ds_stack_type.h"
#define DS_STACK_TYPE ds_stack_type

typedef struct DS_Stack DS_Stack;

DS_Stack *ds_stack_create(void); /* return NULL if malloc failed */
void ds_stack_destroy(DS_Stack *s);

int ds_stack_size(const DS_Stack *s);     /* return size, or -1 if s is NULL */
int ds_stack_capacity(const DS_Stack *s); /* return capacity, or -1 if s is NULL */
int ds_stack_is_empty(const DS_Stack *s); /* return 1 if empty, 0 if not empty, -1 if s is NULL */

int ds_stack_push(DS_Stack *s, DS_STACK_TYPE value); /* return 1 if success, 0 if s is NULL or realloc failed */
/*
    ds_stack_push(s, (DS_STACK_TYPE){10});
*/

int ds_stack_peek(DS_Stack *s, DS_STACK_TYPE **x); /* return 1 if success, 0 if s is NULL or empty or x is NULL */
/*
    DS_STACK_TYPE *data;

    if (ds_stack_peek(s, &data))
    {
        printf("top=%d\n", data->value); // 可以读取
        data->value = 999;               // 可以修改
    }
*/

int ds_stack_pop(DS_Stack *s, DS_STACK_TYPE **x); /* return 1 if success, 0 if s is NULL or empty or x is NULL */
int ds_stack_pop_and_destroy(DS_Stack *s);        /* return 1 if success, 0 if s NULL, or stack empty */
/*
    DS_STACK_TYPE *data;

    if (ds_stack_pop(s, &data))
    {
        printf("pop=%d\n", data->value); // 可以读取
    }

    // 用完后要记得释放 data（如下，使用了_and_destroy版本则不用）
    DS_STACK_DESTROY_ELEMENT(*data);

    // 以下警告对于 pop 和 pop_and_destroy 都适用：
    // WARNING_1: data 指向栈内部已移出的位置，不能 free(data)；
    // WARNING_2: data 在下次 push / reserve 后可能失效；
    // WARNING_3: data 在下次 push 后，会因其指向的位置被覆盖而直接失效。
*/

DS_Stack *ds_stack_clone(const DS_Stack *s, int *judge); /* return pointer if success, NULL if malloc failed or judge is NULL; *judge will be 1 if clone successfully (even if s is NULL), 0 if malloc failed */
/*
    int judge;
    DS_Stack *copy = ds_stack_clone(s, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_stack_reserve(DS_Stack *s, int new_capacity); /* return 1 if success, 0 if s NULL, realloc failed, or new_capacity smaller than current size */
int ds_stack_shrink_to_fit(DS_Stack *s);             /* return 1 if success, 0 if s NULL or realloc failed */
/*
    ds_stack_reserve(s, 100);   // 预留 100 个槽位，避免多次扩容
    ds_stack_shrink_to_fit(s);  // 回收多余槽位，capacity 缩减至 size
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本栈的 push / clone 操作均通过 DS_STACK_CLONE_ELEMENT 宏对数据做深拷贝，栈内部持有
 * 独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name 仍
 *     指向原内存，栈里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_STACK_DESTROY_ELEMENT 释放。
 *
 * 关于 peek / pop 返回的指针：
 *
 *   - peek 返回的指针指向栈顶元素，可直接读取和修改。此指针在下一次 push / pop / reserve
 *     后可能失效。
 *   - pop 返回的指针指向栈内部"边界外"位置（已移出的元素），不能对此指针调用 free()。
 *     此指针在下一次 push 时会被覆盖。
 *   - 使用 pop_and_destroy 则无需手动处理元素内部资源。
 */
