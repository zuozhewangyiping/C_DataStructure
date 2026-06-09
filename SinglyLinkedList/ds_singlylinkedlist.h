#ifndef DS_SINGLYLINKEDLIST_H
#define DS_SINGLYLINKEDLIST_H

#include "ds_singlylinkedlist_type.h"
#define DS_SINGLYLINKEDLIST_TYPE ds_singlylinkedlist_type

typedef struct DS_SinglyLinkedList DS_SinglyLinkedList;
typedef struct SinglyLinkedListNode SinglyLinkedListNode;

DS_SinglyLinkedList *ds_singlylinkedlist_create(void); /* return pointer if success, NULL if malloc failed */
void ds_singlylinkedlist_destroy(DS_SinglyLinkedList *sl);

int ds_singlylinkedlist_traverse(DS_SinglyLinkedList *sl, void *user_data, void (*visit)(DS_SINGLYLINKEDLIST_TYPE *value, void *call_back)); /* return 1 if success, 0 if (sl or visit) NULL */
/*
    void inner_add(DS_SINGLYLINKEDLIST_TYPE *value, void *call_back)
    {
        value->key += *(int *)call_back;
    }

    int x = 5;
    ds_singlylinkedlist_traverse(sl, &x, inner_add); // 每个元素的key都加x
*/

int ds_singlylinkedlist_size(DS_SinglyLinkedList *sl);           /* return number of elements, -1 if sl is NULL */
int ds_singlylinkedlist_is_empty(const DS_SinglyLinkedList *sl); /* return 1 if empty, 0 if not empty, -1 if sl is NULL */

int ds_singlylinkedlist_get(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE **x); /* return 1 if success, 0 if (sl or x) NULL, or index out of range */
/*
    DS_SINGLYLINKEDLIST_TYPE *data_ptr;

    if (ds_singlylinkedlist_get(sl, 0, &data_ptr))
    {
        printf("key=%d, value=%d\n", data_ptr->key, data_ptr->value); // 可以读取
        data_ptr->key = 999;                                          // 可以修改
    }
*/

int ds_singlylinkedlist_set(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE value); /* return 1 if success, 0 if sl NULL or index out of range */
/*
    ds_singlylinkedlist_set(sl, 1, (DS_SINGLYLINKEDLIST_TYPE){999, 888}); // 将索引 1 的节点的 key 改为 999，value 改为 888
*/

int ds_singlylinkedlist_find(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_MATCH_TYPE target, DS_SINGLYLINKEDLIST_TYPE **x); /* return 1 if success, 0 if (sl or x) NULL or failed to find */
/*
    DS_SINGLYLINKEDLIST_TYPE *found;

    if (ds_singlylinkedlist_find(sl, 100, &found))
    {
        found->key = 999;  // 可以修改
    }
*/

SinglyLinkedListNode *ds_singlylinkedlist_search(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_MATCH_TYPE target); /* return cursor if found, NULL if sl is NULL, empty, or not found */
/*
    SinglyLinkedListNode *cursor = ds_singlylinkedlist_search(sl, 100);
    if (cursor != NULL)
    {
        DS_SINGLYLINKEDLIST_TYPE *data;
        ds_singlylinkedlist_node_get_data(cursor, &data);
        // 可继续 next 遍历或读写 data
    }
*/

int ds_singlylinkedlist_push_front(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE value);        /* return 1 if success, 0 if malloc failed or sl NULL */
int ds_singlylinkedlist_push_back(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE value);         /* return 1 if success, 0 if malloc failed or sl NULL */
int ds_singlylinkedlist_insert(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE value); /* return 1 if success, 0 if sl NULL, index out of range, or malloc failed */
/*
    ds_singlylinkedlist_push_XXX(sl, (DS_SINGLYLINKEDLIST_TYPE){1, 100});
*/

int ds_singlylinkedlist_pop_front(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE **x);        /* return 1 if success, 0 if (sl or x) NULL, or sl empty */
int ds_singlylinkedlist_pop_front_and_destroy(DS_SinglyLinkedList *sl);                          /* return 1 if success, 0 if sl NULL, or sl empty */
int ds_singlylinkedlist_pop_back(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE **x);         /* return 1 if success, 0 if (sl or x) NULL, or sl empty */
int ds_singlylinkedlist_pop_back_and_destroy(DS_SinglyLinkedList *sl);                           /* return 1 if success, 0 if sl NULL, or sl empty */
int ds_singlylinkedlist_erase(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE **x); /* return 1 if success, 0 if (sl or x) NULL, or index out of range */
int ds_singlylinkedlist_erase_and_destroy(DS_SinglyLinkedList *sl, int index);                   /* return 1 if success, 0 if sl NULL, or index out of range */
/*
    DS_SINGLYLINKEDLIST_TYPE *data;

    if (ds_singlylinkedlist_pop_XXX(sl, &data))
    {
        printf("key=%d, value=%d\n", data->key, data->value);  // 访问

        // 用完后要记得释放 data（使用了_and_destroy版本则不用）
        DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*data);
        free(data);
    }

    - pop_front / pop_back / erase 会将被移除元素的数据拷贝至新堆块后返回，
      调用者使用完毕后需 DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*data) 清理内部资源，
      再 free(data) 释放该堆块。不想手动管理请用 _and_destroy 变体。
*/

int ds_singlylinkedlist_insert_after_cursor(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE value); /* return 1 if success, 0 if cursor NULL or malloc failed */
int ds_singlylinkedlist_erase_after_cursor(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE **x);    /* return 1 if success, 0 if cursor NULL, x NULL, or cursor->next is NULL */
int ds_singlylinkedlist_erase_after_cursor_and_destroy(SinglyLinkedListNode *cursor);                      /* return 1 if success, 0 if cursor NULL or cursor->next is NULL */
/*
    // 在游标后插入
    ds_singlylinkedlist_insert_after_cursor(cursor, (DS_SINGLYLINKEDLIST_TYPE){100, 200});

    // 删除游标的下一节点
    DS_SINGLYLINKEDLIST_TYPE *data;
    if (ds_singlylinkedlist_erase_after_cursor(cursor, &data))
    {
        DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*data);
        free(data);
    }
    // 或直接用 _and_destroy 版本
    ds_singlylinkedlist_erase_after_cursor_and_destroy(cursor);
*/

DS_SinglyLinkedList *ds_singlylinkedlist_clone(const DS_SinglyLinkedList *sl, int *judge); /* deep_clone, return pointer if success, NULL if malloc failed or (sl or judge) NULL; *judge will be 1 if clone successfully, *judge is unchanged (stays 1) when sl or judge is NULL , 0 if malloc failed */
/*
    int judge;
    DS_SinglyLinkedList *copy = ds_singlylinkedlist_clone(sl, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_singlylinkedlist_concat(DS_SinglyLinkedList *sl1, DS_SinglyLinkedList *sl2); /* Move all nodes from sl2 to the end of sl1. sl2 becomes empty afterwards. sl1 and sl2 must be different. return 1 if success (even if sl2 NULL), 0 if sl1 NULL, or sl1 == sl2 */
/*
    ds_singlylinkedlist_concat(sl1, sl2); // sl2 的所有节点转移至 sl1 末尾，sl2 变为空链表
*/

SinglyLinkedListNode *ds_singlylinkedlist_begin(DS_SinglyLinkedList *sl);                          /* return cursor to first node, or NULL if sl is NULL or empty */
SinglyLinkedListNode *ds_singlylinkedlist_next(SinglyLinkedListNode *cursor);                      /* return cursor to next node, or NULL if at tail */
int ds_singlylinkedlist_node_get_data(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE **x); /* return 1 if success, 0 if cursor or x is NULL */
/*
    // 游标cursor使用样例————手动遍历整条链表：
    SinglyLinkedListNode *cur = ds_singlylinkedlist_begin(sl);
    while (cur != NULL)
    {
        DS_SINGLYLINKEDLIST_TYPE *data;
        ds_singlylinkedlist_node_get_data(cur, &data);
        printf("key=%d\n", data->key);
        cur = ds_singlylinkedlist_next(cur);
    }
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本链表的 push / insert / set / clone 操作均通过 DS_SINGLYLINKEDLIST_CLONE_ELEMENT
 * 宏对数据做深拷贝，链表内部持有独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，链表里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_SINGLYLINKEDLIST_DESTROY_ELEMENT 释放：
 *
 *         char *n = strdup("Alice");
 *         ds_singlylinkedlist_push_back(sl, (type){1, 100, n});
 *         DS_SINGLYLINKEDLIST_DESTROY_ELEMENT((type){1, 100, n});  // 释放你手里的 name
 *
 *   - 若需修改链表内已有数据，请通过 get / find 拿到内部指针后直接修改，无需再次 push。
 *
 * Cursor（游标）使用说明
 * --------------------
 * SinglyLinkedListNode * 是 opaque 类型，用户无法直接访问其成员。
 * 通过三个函数完成遍历：
 *
 *   ds_singlylinkedlist_begin       → 获取起始游标
 *   ds_singlylinkedlist_next        → 移动到下一节点
 *   ds_singlylinkedlist_node_get_data → 读写游标所指元素的 data
 *
 * cursor == NULL 表示"遍历结束"。
 *
 * cursor 在所在节点被 pop / erase / destroy 后立即失效。
 *
 * // 遍历示例
 * SinglyLinkedListNode *cur = ds_singlylinkedlist_begin(sl);
 * while (cur != NULL)
 * {
 *     DS_SINGLYLINKEDLIST_TYPE *data;
 *     ds_singlylinkedlist_node_get_data(cur, &data);
 *     cur = ds_singlylinkedlist_next(cur);
 * }
 */
