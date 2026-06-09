#ifndef DS_DOUBLELINKEDLIST_H
#define DS_DOUBLELINKEDLIST_H

#include "ds_doublelinkedlist_type.h"
#define DS_DOUBLELINKEDLIST_TYPE ds_doublelinkedlist_type

typedef struct DS_DoubleLinkedList DS_DoubleLinkedList;
typedef struct DoubleLinkedListNode DoubleLinkedListNode;

DS_DoubleLinkedList *ds_doublelinkedlist_create(void); /* return pointer if success, NULL if malloc failed */
void ds_doublelinkedlist_destroy(DS_DoubleLinkedList *dl);

int ds_doublelinkedlist_traverse(DS_DoubleLinkedList *dl, void *user_data, void (*visit)(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back));  /* return 1 if success, 0 if (dl or visit) NULL */
int ds_doublelinkedlist_rtraverse(DS_DoubleLinkedList *dl, void *user_data, void (*visit)(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back)); /* return 1 if success, 0 if (dl or visit) NULL */
/*
    void inner_add(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back)
    {
        value->key += *(int *)call_back;
    }

    int x = 5;
    ds_doublelinkedlist_traverse(dl, &x, inner_add); // 每个元素的key都加x
*/

int ds_doublelinkedlist_size(DS_DoubleLinkedList *dl);           /* return number of elements, -1 if dl is NULL */
int ds_doublelinkedlist_is_empty(const DS_DoubleLinkedList *dl); /* return 1 if empty, 0 if not empty, -1 if dl is NULL */

int ds_doublelinkedlist_get(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE **x); /* return 1 if success, 0 if (dl or x) NULL, or index out of range */
/*
    DS_DOUBLELINKEDLIST_TYPE *data_ptr;

    if (ds_doublelinkedlist_get(dl, 0, &data_ptr))
    {
        printf("key=%d, value=%d\n", data_ptr->key, data_ptr->value); // 可以读取
        data_ptr->key = 999;                                          // 可以修改
    }
*/

int ds_doublelinkedlist_set(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE value); /* return 1 if success, 0 if dl NULL or index out of range */
/*
    ds_doublelinkedlist_set(dl, 1, (DS_DOUBLELINKEDLIST_TYPE){999, 888}); // 将索引 1 的节点的 key 改为 999，value 改为 888
*/

int ds_doublelinkedlist_find(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_MATCH_TYPE target, DS_DOUBLELINKEDLIST_TYPE **x); /* return 1 if success, 0 if (dl or x) NULL or failed to find */
/*
    DS_DOUBLELINKEDLIST_TYPE *found;

    if (ds_doublelinkedlist_find(dl, 100, &found))
    {
        found->key = 999;  // 可以修改
    }
*/

DoubleLinkedListNode *ds_doublelinkedlist_search(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_MATCH_TYPE target); /* return cursor if found, NULL if dl is NULL, empty, or not found */
/*
    DoubleLinkedListNode *cursor = ds_doublelinkedlist_search(dl, 100);
    if (cursor != NULL)
    {
        DS_DOUBLELINKEDLIST_TYPE *data;
        ds_doublelinkedlist_node_get_data(cursor, &data);
        // 可继续 next 遍历或读写 data
    }
*/

int ds_doublelinkedlist_push_front(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE value);        /* return 1 if success, 0 if malloc failed or dl NULL */
int ds_doublelinkedlist_push_back(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE value);         /* return 1 if success, 0 if malloc failed or dl NULL */
int ds_doublelinkedlist_insert(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE value); /* return 1 if success, 0 if dl NULL, index out of range, or malloc failed */
/*
    ds_doublelinkedlist_push_XXX(dl, (DS_DOUBLELINKEDLIST_TYPE){1, 100});
*/

int ds_doublelinkedlist_pop_front(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE **x);        /* return 1 if success, 0 if (dl or x) NULL, or dl empty */
int ds_doublelinkedlist_pop_front_and_destroy(DS_DoubleLinkedList *dl);                          /* return 1 if success, 0 if dl NULL, or dl empty */
int ds_doublelinkedlist_pop_back(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE **x);         /* return 1 if success, 0 if (dl or x) NULL, or dl empty */
int ds_doublelinkedlist_pop_back_and_destroy(DS_DoubleLinkedList *dl);                           /* return 1 if success, 0 if dl NULL, or dl empty */
int ds_doublelinkedlist_erase(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE **x); /* return 1 if success, 0 if (dl or x) NULL, or index out of range */
int ds_doublelinkedlist_erase_and_destroy(DS_DoubleLinkedList *dl, int index);                   /* return 1 if success, 0 if dl NULL, or index out of range */
/*
    DS_DOUBLELINKEDLIST_TYPE *data;

    if (ds_doublelinkedlist_pop_XXX(dl, &data))
    {
        printf("key=%d, value=%d\n", data->key, data->value);  // 访问

        // 用完后要记得释放 data（使用了_and_destroy版本则不用）
        DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*data);
        free(data);
    }

    - pop_front / pop_back / erase 会将被移除元素的数据拷贝至新堆块后返回，
      调用者使用完毕后需 DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*data) 清理内部资源，
      再 free(data) 释放该堆块。不想手动管理请用 _and_destroy 变体。
*/

int ds_doublelinkedlist_insert_after_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE value);  /* return 1 if success, 0 if cursor NULL or malloc failed */
int ds_doublelinkedlist_insert_before_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE value); /* return 1 if success, 0 if cursor NULL or malloc failed */
int ds_doublelinkedlist_erase_after_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x);     /* return 1 if success, 0 if cursor NULL, x NULL, or cursor->next is NULL */
int ds_doublelinkedlist_erase_after_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor);                       /* return 1 if success, 0 if cursor NULL or cursor->next is NULL */
int ds_doublelinkedlist_erase_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x);           /* return 1 if success, 0 if cursor NULL or x NULL */
int ds_doublelinkedlist_erase_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor);                             /* return 1 if success, 0 if cursor NULL */
int ds_doublelinkedlist_erase_before_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x);    /* return 1 if success, 0 if cursor NULL, x NULL, or cursor->prev is NULL */
int ds_doublelinkedlist_erase_before_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor);                      /* return 1 if success, 0 if cursor NULL or cursor->prev is NULL */
/*
    // 在游标后插入
    ds_doublelinkedlist_insert_after_cursor(dl, cursor, (DS_DOUBLELINKEDLIST_TYPE){100, 200});

    // 删除游标的下一节点
    DS_DOUBLELINKEDLIST_TYPE *data;
    if (ds_doublelinkedlist_erase_after_cursor(dl, cursor, &data))
    {
        DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*data);
        free(data);
    }
    // 或直接用 _and_destroy 版本
    ds_doublelinkedlist_erase_after_cursor_and_destroy(dl, cursor);

    // 另外几个用法相同
*/

DS_DoubleLinkedList *ds_doublelinkedlist_clone(const DS_DoubleLinkedList *dl, int *judge); /* deep_clone, return pointer if success, NULL if malloc failed or (dl or judge) NULL; *judge will be 1 if clone successfully, *judge is unchanged (stays 1) when dl or judge is NULL , 0 if malloc failed */
/*
    int judge;
    DS_DoubleLinkedList *copy = ds_doublelinkedlist_clone(dl, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_doublelinkedlist_concat(DS_DoubleLinkedList *dl1, DS_DoubleLinkedList *dl2); /* Move all nodes from dl2 to the end of dl1. dl2 becomes empty afterwards. dl1 and dl2 must be different. return 1 if success (even if dl2 NULL), 0 if dl1 NULL, or dl1 == dl2 */
/*
    ds_doublelinkedlist_concat(dl1, dl2); // dl2 的所有节点转移至 dl1 末尾，dl2 变为空链表
*/

DoubleLinkedListNode *ds_doublelinkedlist_begin(DS_DoubleLinkedList *dl);                          /* return cursor to first node, or NULL if dl is NULL or empty */
DoubleLinkedListNode *ds_doublelinkedlist_rbegin(DS_DoubleLinkedList *dl);                         /* return cursor to last node, or NULL if dl is NULL or empty */
DoubleLinkedListNode *ds_doublelinkedlist_next(DoubleLinkedListNode *cursor);                      /* return cursor to next node, or NULL if at tail */
DoubleLinkedListNode *ds_doublelinkedlist_prev(DoubleLinkedListNode *cursor);                      /* return cursor to previous node, or NULL if at head */
int ds_doublelinkedlist_node_get_data(DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x); /* return 1 if success, 0 if cursor or x is NULL */
/*
    // 游标cursor使用样例————手动遍历整条链表：
    // Example 1.正序遍历
    DoubleLinkedListNode *cur = ds_doublelinkedlist_begin(dl);
    while (cur != NULL)
    {
        DS_DOUBLELINKEDLIST_TYPE *data;
        ds_doublelinkedlist_node_get_data(cur, &data);
        printf("key=%d\n", data->key);
        cur = ds_doublelinkedlist_next(cur);
    }

    // Example 2.逆序遍历
    DoubleLinkedListNode *cur = ds_doublelinkedlist_rbegin(dl);
    while (cur != NULL)
    {
        DS_DOUBLELINKEDLIST_TYPE *data;
        ds_doublelinkedlist_node_get_data(cur, &data);
        cur = ds_doublelinkedlist_prev(cur);
    }
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * 本链表的 push / insert / set / clone 操作均通过 DS_DOUBLELINKEDLIST_CLONE_ELEMENT
 * 宏对数据做深拷贝，链表内部持有独立副本。用户传入 value 后仍保留自己那份数据的所有权：
 *
 *   - 若你的元素类型仅含标量成员（如 int），value 在栈上，函数返回后自动消亡，无需处理。
 *   - 若你的元素类型含指针成员（如 char *name 指向堆内存），函数返回后你手里的 name
 *     仍指向原内存，链表里的副本已经独立。如果你不再需要自己这份，请手动调用
 *     DS_DOUBLELINKEDLIST_DESTROY_ELEMENT 释放：
 *
 *         char *n = strdup("Alice");
 *         ds_doublelinkedlist_push_back(dl, (type){1, 100, n});
 *         DS_DOUBLELINKEDLIST_DESTROY_ELEMENT((type){1, 100, n});  // 释放你手里的 name
 *
 *   - 若需修改链表内已有数据，请通过 get / find 拿到内部指针后直接修改，无需再次 push。
 *
 * Cursor（游标）使用说明
 * --------------------
 * DoubleLinkedListNode * 是 opaque 类型，用户无法直接访问其成员。
 * 通过五个函数完成遍历：
 *
 *   ds_doublelinkedlist_begin       → 获取起始游标
 *   ds_doublelinkedlist_rbegin      → 获得末尾游标
 *   ds_doublelinkedlist_next        → 移动到下一节点
 *   ds_doublelinkedlist_prev        → 移动到上一节点
 *   ds_doublelinkedlist_node_get_data → 读写游标所指元素的 data
 *
 * cursor == NULL 表示"遍历结束"。
 *
 * cursor 在所在节点被 pop / erase / destroy 后立即失效。
 *
 * // 遍历示例在五个函数声明下方注释块
 */
