#ifndef DS_HASHTABLE_H
#define DS_HASHTABLE_H

#include "ds_hashtable_type.h"
#define DS_HASHTABLE_TYPE ds_hashtable_type

typedef struct DS_HashTable DS_HashTable;

DS_HashTable *ds_hashtable_create(void); /* return pointer if success, NULL if malloc failed */
void ds_hashtable_destroy(DS_HashTable *ht);

int ds_hashtable_traverse(DS_HashTable *ht, void *user_data, void (*visit)(DS_HASHTABLE_TYPE *value, void *call_back)); /* return 1 if success, 0 if (ht or visit) NULL */
/*
    void print_element(DS_HASHTABLE_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d, value=%d\n", value->key, value->value);
    }

    ds_hashtable_traverse(ht, NULL, print_element);
*/

int ds_hashtable_size(DS_HashTable *ht);     /* return size, -1 if ht is NULL */
int ds_hashtable_capacity(DS_HashTable *ht); /* return capacity, -1 if ht is NULL */
int ds_hashtable_is_empty(DS_HashTable *ht); /* return 1 if empty, 0 if not empty, -1 if ht is NULL */

int ds_hashtable_find(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target, DS_HASHTABLE_TYPE **x); /* return 1 if success, 0 if (ht or x) NULL or not found */
/*
    DS_HASHTABLE_TYPE *found;

    if (ds_hashtable_find(ht, 100, &found))
    {
        found->value = 999;  // 可以修改，修改立即生效
    }
*/

// insert —— 深度克隆 + 存入，key 已存在则拒绝
int ds_hashtable_insert(DS_HashTable *ht, DS_HASHTABLE_TYPE value); /* return 1 if success, 0 if ht NULL, malloc failed, or key already exists */
/*
    ds_hashtable_insert(ht, (DS_HASHTABLE_TYPE){1, 100});
*/

// put —— key 已存在则覆盖（DESTROY 旧值 + CLONE 新值），不存在则同 insert
int ds_hashtable_put(DS_HashTable *ht, DS_HASHTABLE_TYPE value); /* return 1 if success, 0 if ht NULL or malloc failed */
/*
    ds_hashtable_put(ht, (DS_HASHTABLE_TYPE){1, 100});
*/

int ds_hashtable_erase(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target, DS_HASHTABLE_TYPE **x); /* return 1 if success, 0 if (ht or x) NULL or malloc failed or not found */
int ds_hashtable_erase_and_destroy(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target);            /* return 1 if success, 0 if ht NULL or malloc failed or not found */
/*
    DS_HASHTABLE_TYPE *data;

    if (ds_hashtable_erase(ht, 100, &data))
    {
        printf("key=%d, value=%d\n", data->key, data->value); // 可以读取

        // 用完后记得释放 data（使用了 _and_destroy 版本则不用）
        DS_HASHTABLE_DESTROY_ELEMENT(*data);
        free(data);
    }
*/

DS_HashTable *ds_hashtable_clone(const DS_HashTable *ht, int *judge); /* return pointer if success, NULL if malloc failed or judge NULL; *judge = 1 if clone successfully (even if ht NULL), 0 if malloc failed */
/*
    int judge;
    DS_HashTable *copy = ds_hashtable_clone(ht, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_hashtable_reserve(DS_HashTable *ht, int new_capacity); /* return 1 if success, 0 if ht NULL, realloc failed, or new_capacity smaller than current size */

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本哈希表的 insert / put / clone 操作均通过 DS_HASHTABLE_CLONE_ELEMENT
 * 宏对数据做深拷贝，哈希表内部持有独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，哈希表里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_HASHTABLE_DESTROY_ELEMENT 释放。
 *
 * 关于 find 返回的指针：
 *   - find 返回的指针指向桶链内部节点数据，可直接读取和修改。此指针在该节点被 erase 后失效。
 *
 * 关于 erase 返回的指针：
 *   - erase 将删除的元素数据拷贝至新堆块（malloc）后返回，调用者使用完毕后需
 *     DS_HASHTABLE_DESTROY_ELEMENT(*data) 清理内部资源，再 free(data) 释放堆块。
 *     不想手动管理请用 erase_and_destroy 变体。
 *
 * insert 与 put 的区别
 * --------------------
 *   - insert：纯插入。key 已存在时拒绝并返回 0。
 *   - put：插入或覆盖。key 已存在时销毁旧值，克隆新值覆盖。
 */
