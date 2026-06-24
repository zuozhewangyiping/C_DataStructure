#ifndef DS_SKIPLIST_H
#define DS_SKIPLIST_H

#include "ds_skiplist_type.h"
#define DS_SKIPLIST_TYPE ds_skiplist_type

typedef struct DS_SkipList DS_SkipList;
typedef struct SkipListNode SkipListNode;

/*-----------------------------------------------------------------------------
 * MENU
 * Part 1. _create / _destroy
 * Part 2. _size / _level / _is_empty
 * Part 3. _traverse
 * Part 4. _clone
 * Part 5. _node_get_data
 * Part 6. _search
 * Part 7. _find_max / _find_min
 * Part 8. _predecessor / _successor
 * Part 9. _range_query
 * Part 10. _insert / _delete
 *---------------------------------------------------------------------------*/

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_SkipList *ds_skiplist_create(void); /* return pointer if success, NULL if malloc failed */
void ds_skiplist_destroy(DS_SkipList *skl);
/*
    DS_SkipList *skl = ds_skiplist_create();
    ds_skiplist_destroy(skl); // 深度销毁整个表
    skl = NULL;
*/

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_skiplist_size(const DS_SkipList *skl);     /* return size, or 0 if skl is NULL */
int ds_skiplist_level(const DS_SkipList *skl);    /* return level, or 0 if skl is NULL */
int ds_skiplist_is_empty(const DS_SkipList *skl); /* return 1 if empty, 0 if not empty, -1 if skl is NULL */
/*
    int s = ds_skiplist_size(skl);
    int lv = ds_skiplist_level(skl);
    int e = ds_skiplist_is_empty(skl); // 1=empty, 0=not empty, -1=skl is NULL
*/

/* Part 3. Traversals（回调可读写 data）---------------------------------------*/

int ds_skiplist_traverse(DS_SkipList *skl, void *user_data, void (*visit)(DS_SKIPLIST_TYPE *value, void *callback_data));
/* return 1 if success, 0 if skl or visit is NULL */

/*
    void visit(DS_SKIPLIST_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d, value=%d\n", value->key, value->value); // 只读
        value->value *= 2;                                      // 也可以修改
    }

    ds_skiplist_traverse(skl, NULL, visit);
*/

/* Part 4. Clone ------------------------------------------------------------*/

DS_SkipList *ds_skiplist_clone(const DS_SkipList *skl, int *judge);
/* return pointer if success, NULL if judge is NULL or malloc failed; *judge will be 1 if clone successful, 0 if malloc failed */

/*
    int judge;
    DS_SkipList *copy = ds_skiplist_clone(skl, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败
    }
*/

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_skiplist_node_get_data(SkipListNode *cursor, DS_SKIPLIST_TYPE **x); /* return 1 if success, 0 if cursor or x is NULL */
/*
    DS_SKIPLIST_TYPE *data;
    ds_skiplist_node_get_data(cursor, &data);
    printf("key=%d\n", data->key);
    *data = (DS_SKIPLIST_TYPE){.key = 99, .value = 0}; // 可以修改，但不要改 key
*/

/* Part 6. Search -----------------------------------------------------------*/

SkipListNode *ds_skiplist_search(DS_SkipList *skl, DS_SKIPLIST_TYPE value); /* return cursor if found, NULL if skl is NULL, empty, or not found */
/*
    SkipListNode *cursor = ds_skiplist_search(skl, (DS_SKIPLIST_TYPE){.key = 50});
*/

/* Part 7. Cursor Acquisition -----------------------------------------------*/

SkipListNode *ds_skiplist_find_max(DS_SkipList *skl);
SkipListNode *ds_skiplist_find_min(DS_SkipList *skl);
/* return cursor, or NULL if skl is NULL or empty */

/*
    SkipListNode *cursor = ds_skiplist_find_min(skl);
*/

/* Part 8. Cursor Navigation ------------------------------------------------*/

SkipListNode *ds_skiplist_predecessor(DS_SkipList *skl, SkipListNode *cursor);
SkipListNode *ds_skiplist_successor(DS_SkipList *skl, SkipListNode *cursor);
/* return cursor, or NULL if no predecessor / successor exists */

/*
    // 正向遍历整个表
    SkipListNode *cursor = ds_skiplist_find_min(skl);
    while (cursor != NULL)
    {
        DS_SKIPLIST_TYPE *data;
        ds_skiplist_node_get_data(cursor, &data);
        cursor = ds_skiplist_successor(skl, cursor);
    }
*/

/* Part 9. Range Query（回调可读写 data）-------------------------------------*/

int ds_skiplist_range_query(DS_SkipList *skl,
                            DS_SKIPLIST_TYPE low,
                            DS_SKIPLIST_TYPE high,
                            void *user_data,
                            void (*visit)(DS_SKIPLIST_TYPE *value, void *callback_data));
/* return 1 if success, 0 if skl or visit NULL */

/*
    void visit(DS_SKIPLIST_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d\n", value->key);
    }

    ds_skiplist_range_query(skl,
                           (DS_SKIPLIST_TYPE){.key = 30},
                           (DS_SKIPLIST_TYPE){.key = 70},
                           NULL,
                           visit);
*/

/* Part 10. Insert / Delete -------------------------------------------------*/

int ds_skiplist_insert(DS_SkipList *skl, DS_SKIPLIST_TYPE value); /* return 1 if success, 0 if skl NULL, malloc failed, or duplicate key */
int ds_skiplist_delete(DS_SkipList *skl, DS_SKIPLIST_TYPE value); /* return 1 if success, 0 if skl NULL, empty, or value not found */
/*
    ds_skiplist_insert(skl, (DS_SKIPLIST_TYPE){.key = 50, .value = 100});
    ds_skiplist_delete(skl, (DS_SKIPLIST_TYPE){.key = 30});
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 跳表存储泛型元素，通过 _type.h 中的宏管理元素生命周期：
 *
 *   - insert 将 value 深拷贝（CLONE_ELEMENT）到新节点，之后节点拥有该副本。
 *     CLONE 失败时新节点内存立即释放，跳表状态不变。
 *
 *   - delete / destroy 删除节点时，调用 DESTROY_ELEMENT 清理其 data，
 *     然后 free 节点。
 *
 *   - node_get_data 返回的指针指向节点内部 data，可直接读取和修改。
 *     **警告：修改 key 字段会破坏有序性。**
 *     此指针在节点被 delete / destroy 后失效。
 *
 *   - 遍历回调及 range_query 回调接收 DS_SKIPLIST_TYPE *value，可读写 data。
 *     同样，修改 key 会破坏有序性。
 *
 *   - clone 返回完全独立的深拷贝跳表，使用完毕需 ds_skiplist_destroy 释放。
 *
 *   - search / find_max / find_min / predecessor / successor 返回的 cursor
 *     在下一次 insert / delete / destroy 后可能失效（节点被释放）。
 *
 *   - Cursor（游标）使用说明
 *     --------------------
 *     SkipListNode * 是 opaque 类型，用户无法直接访问其成员。
 *     通过以下三个函数即可完成所有操作：
 *
 *     find_min / find_max     → 获取起始游标
 *     predecessor / successor → 移动游标
 *     node_get_data           → 读写游标所指元素的 data
 *
 *     cursor == NULL 表示"遍历结束"（类比 std::map::end()）。
 *
 *     cursor 在所在节点被 delete 或整个跳表被 destroy 后立即失效，
 *     在下一次 insert / delete 后也可能因节点释放而失效。
 *
 *   - predecessor / successor 要求 cursor 必须是 skl 中的节点，传入非法指针
 *     会导致未定义行为。
 */
