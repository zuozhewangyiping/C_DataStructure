#ifndef DS_DYNAMICARRAY_H
#define DS_DYNAMICARRAY_H

#include "ds_dynamicarray_type.h"
#define DS_DYNAMICARRAY_TYPE ds_dynamicarray_type

typedef struct DS_DynamicArray DS_DynamicArray;

DS_DynamicArray *ds_dynamicarray_create(void); /* return pointer if success, NULL if malloc failed */
void ds_dynamicarray_destroy(DS_DynamicArray *da);

int ds_dynamicarray_size(const DS_DynamicArray *da);     /* return number of elements, -1 if da is NULL */
int ds_dynamicarray_capacity(const DS_DynamicArray *da); /* return capacity, -1 if da is NULL */
int ds_dynamicarray_is_empty(const DS_DynamicArray *da); /* return 1 if empty, 0 if not empty, -1 if da is NULL */

int ds_dynamicarray_get(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE **x); /* return 1 if success, 0 if da NULL, x NULL, or index out of range */
/*
    DS_DYNAMICARRAY_TYPE *data_ptr;

    if (ds_dynamicarray_get(da, 0, &data_ptr))
    {
        printf("data=%d\n", data_ptr->data); // 可以读取
        data_ptr->data = 999;                // 可以修改
    }

    // WARNING: data_ptr 在下次扩容操作（push_back / insert / reserve）后可能失效，使用前请确保期间未触发扩容。
*/

int ds_dynamicarray_set(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE value); /* return 1 if success, 0 if da NULL or index out of range */
/*
    ds_dynamicarray_set(da, 1, (DS_DYNAMICARRAY_TYPE){999}); // 将索引 1 的元素改为 999
*/

int ds_dynamicarray_find(DS_DynamicArray *da, DS_DYNAMICARRAY_MATCH_TYPE target, DS_DYNAMICARRAY_TYPE **x); /* return 1 if success, 0 if (da or x) NULL or failed to find */
/*
    DS_DYNAMICARRAY_TYPE *found;

    if (ds_dynamicarray_find(da, 100, &found))
    {
        found->data = 999;  // 可以修改
    }

    // WARNING: 和 get 一样，found 在下次扩容 / push_back / insert 后可能失效
*/

int ds_dynamicarray_push_back(DS_DynamicArray *da, DS_DYNAMICARRAY_TYPE value);         /* return 1 if success, 0 if realloc failed or da NULL */
int ds_dynamicarray_insert(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE value); /* return 1 if success, 0 if da NULL, index out of range, or realloc failed */
/*
    ds_dynamicarray_push_back(da, (DS_DYNAMICARRAY_TYPE){100});
*/

int ds_dynamicarray_pop_back(DS_DynamicArray *da, DS_DYNAMICARRAY_TYPE **x);         /* return 1 if success, 0 if da NULL, x NULL, or array empty */
int ds_dynamicarray_pop_back_and_destroy(DS_DynamicArray *da);                       /* return 1 if success, 0 if da NULL, or array empty */
int ds_dynamicarray_erase(DS_DynamicArray *da, int index, DS_DYNAMICARRAY_TYPE **x); /* return 1 if success, 0 if da NULL, x NULL, or index out of range */
int ds_dynamicarray_erase_and_destroy(DS_DynamicArray *da, int index);               /* return 1 if success, 0 if da NULL, or array empty */
/*
    DS_DYNAMICARRAY_TYPE *data;

    if (ds_dynamicarray_pop_back(da, &data))
    {
        printf("data=%d\n", data->data); // 可以读取
        // 如果元素类型含指针成员（如 char *name），需手动调用：
    }

    // 用完后要记得释放 data（如下，使用了_and_destroy版本则不用）
    DS_DYNAMICARRAY_DESTROY_ELEMENT(*data);

    // 以下警告对于 pop 和 erase 都适用：
    // WARNING_1: data 指向数组内部已移出的位置，不能 free(data)；
    // WARNING_2: data_ptr 在下次扩容操作（包含reserve功能的函数）后可能失效；
    // WARNING_3: data_ptr 在下次 push_back / insert 后，会因其指向的位置被覆盖而直接失效。
*/

DS_DynamicArray *ds_dynamicarray_clone(const DS_DynamicArray *da, int *judge); /* return pointer if success, NULL if malloc failed or judge is NULL; *judge will be 1 if clone successfully (even if da is NULL), 0 if malloc failed */
/*
    int judge;
    DS_DynamicArray *copy = ds_dynamicarray_clone(da, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_dynamicarray_reserve(DS_DynamicArray *da, int new_capacity); /* return 1 if success, 0 if da NULL, realloc failed, or new_capacity smaller than current size */
int ds_dynamicarray_shrink_to_fit(DS_DynamicArray *da);             /* return 1 if success, 0 if da NULL or realloc failed */
/*
    ds_dynamicarray_reserve(da, 100);   // 预留 100 个槽位，避免多次扩容
    ds_dynamicarray_shrink_to_fit(da);  // 回收多余槽位，capacity 缩减至 size
*/

int ds_dynamicarray_concat(DS_DynamicArray *da1, DS_DynamicArray *da2); /* return 1 if success, 0 if da1 NULL or realloc failed. 失败时 da1 内容不变，但 capacity 可能已扩容*/
/*
    ds_dynamicarray_concat(da1, da2); // da2 的所有元素转移至 da1 末尾，da2 变为空数组
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本数组的 push_back / insert / set / clone 操作均通过 DS_DYNAMICARRAY_CLONE_ELEMENT
 * 宏对数据做深拷贝，数组内部持有独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，数组里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_DYNAMICARRAY_DESTROY_ELEMENT 释放。
 *
 * 关于 get / find / pop_back / erase 返回的指针：
 *
 *   - get / find 返回的指针指向数组内部数据，可直接读取和修改。此指针在下一次导致
 *     扩容的操作（push_back / insert / reserve）后可能失效。
 *   - pop_back / erase 返回的指针指向数组"边界外"位置（已移出的元素），同样会在
 *     下一次 push_back / insert 时被覆盖。不能对此指针调用 free()。
 *   - 使用 _and_destroy 版本（pop_back_and_destroy / erase_and_destroy）则无需
 *     手动处理元素内部资源。
 */
