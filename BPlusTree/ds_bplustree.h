#ifndef DS_BPLUSTREE_H
#define DS_BPLUSTREE_H

#include "ds_bplustree_type.h"
#define DS_BPLUSTREE_KEY_TYPE ds_bplustree_key_type
#define DS_BPLUSTREE_VALUE_TYPE ds_bplustree_value_type

typedef struct DS_BPlusTree DS_BPlusTree;
typedef struct BPlusTreeNode BPlusTreeNode;

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

DS_BPlusTree *ds_bplustree_create(const char *filename); /* return pointer if success, NULL if file open failed, alloc failed, or I/O error */
void ds_bplustree_destroy(DS_BPlusTree *bpt);
/*
    DS_BPlusTree *bpt = ds_bplustree_create("students.db");
    ds_bplustree_destroy(bpt); // 深度销毁整个树
    bpt = NULL;
*/

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_bplustree_size(const DS_BPlusTree *bpt);     /* return size, or 0 if bpt is NULL */
int ds_bplustree_level(const DS_BPlusTree *bpt);    /* return level, or 0 if bpt is NULL */
int ds_bplustree_is_empty(const DS_BPlusTree *bpt); /* return 1 if empty, 0 if not empty, -1 if bpt is NULL */
/*
    int s = ds_bplustree_size(bpt);
    int lv = ds_bplustree_level(bpt);
    int e = ds_bplustree_is_empty(bpt); // 1=empty, 0=not empty, -1=bpt is NULL
*/

/* Part 3. Traversals（回调只读 data）---------------------------------------*/

int ds_bplustree_traverse(DS_BPlusTree *bpt, void *user_data, void (*visit)(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *callback_data));
/* return 1 if success, 0 if bpt/visit NULL or I/O error */

/*
    void visit(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d, value=%d\n", key->key, value->value);
    }

    ds_bplustree_traverse(bpt, NULL, visit);
*/

/* Part 4. Clone ------------------------------------------------------------*/

DS_BPlusTree *ds_bplustree_clone(const DS_BPlusTree *bpt, const char *filename, int *judge);
/* return pointer if success, NULL if judge NULL, alloc failed, or I/O error; *judge will be 1 if clone successful, 0 if failed */

/*
    int judge;
    DS_BPlusTree *copy = ds_bplustree_clone(bpt, "copy.db", &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败
    }
*/

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_bplustree_node_get_data(BPlusTreeNode *cursor, DS_BPLUSTREE_KEY_TYPE **x_key, DS_BPLUSTREE_VALUE_TYPE **x_value); /* return 1 if success, 0 if cursor or x is NULL */
/*
    DS_BPLUSTREE_KEY_TYPE *data_key;
    DS_BPLUSTREE_VALUE_TYPE *data_value;

    ds_bplustree_node_get_data(cursor, &data_key, &data_value);

    printf("key=%d, value=%d\n", data_key->key, data_value->value); // 可读
    *data_value = (DS_BPLUSTREE_VALUE_TYPE){.value = 0}; // 可以修改，但不要改 key
*/

/* Part 6. Search -----------------------------------------------------------*/

BPlusTreeNode *ds_bplustree_search(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key); /* return cursor if found, NULL if bpt NULL, empty, not found, or I/O error */
/*
    BPlusTreeNode *cursor = ds_bplustree_search(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 50});
*/

/* Part 7. Cursor Acquisition -----------------------------------------------*/

BPlusTreeNode *ds_bplustree_find_max(DS_BPlusTree *bpt);
BPlusTreeNode *ds_bplustree_find_min(DS_BPlusTree *bpt);
/* return cursor, or NULL if bpt is NULL, empty, or I/O error */

/*
    BPlusTreeNode *cursor = ds_bplustree_find_min(bpt);
*/

/* Part 8. Cursor Navigation ------------------------------------------------*/

BPlusTreeNode *ds_bplustree_predecessor(DS_BPlusTree *bpt, BPlusTreeNode *cursor);
BPlusTreeNode *ds_bplustree_successor(DS_BPlusTree *bpt, BPlusTreeNode *cursor);
/* return cursor, or NULL if bpt/cursor NULL, no predecessor/successor, or I/O error */

/*
    // 正向遍历整个树（注意：successor/predecessor 会 malloc 新游标，旧游标需 free）
    BPlusTreeNode *cursor = ds_bplustree_find_min(bpt);
    while (cursor != NULL)
    {
        DS_BPLUSTREE_KEY_TYPE *data_key;
        DS_BPLUSTREE_VALUE_TYPE *data_value;

        ds_bplustree_node_get_data(cursor, &data_key, &data_value);

        BPlusTreeNode *next = ds_bplustree_successor(bpt, cursor);
        free(cursor);
        cursor = next;
    }
*/

/* Part 9. Range Query（回调只读 data）-------------------------------------*/

int ds_bplustree_range_query(DS_BPlusTree *bpt,
                             DS_BPLUSTREE_KEY_TYPE low,
                             DS_BPLUSTREE_KEY_TYPE high,
                             void *user_data,
                             void (*visit)(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *callback_data));
/* return 1 if success, 0 if bpt/visit NULL or I/O error */

/*
    void visit(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d, value=%d\n", key->key, value->value);
    }

    ds_bplustree_range_query(bpt,
                           (DS_BPLUSTREE_KEY_TYPE){.key = 30},
                           (DS_BPLUSTREE_KEY_TYPE){.key = 70},
                           NULL,
                           visit);
*/

/* Part 10. Insert / Delete -------------------------------------------------*/

int ds_bplustree_insert(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key, DS_BPLUSTREE_VALUE_TYPE value); /* return 1 if success, 0 if bpt NULL, alloc/I/O error, or duplicate key */
int ds_bplustree_delete(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key);                                /* return 1 if success, 0 if bpt NULL, empty, key not found, or I/O error */
/*
    ds_bplustree_insert(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 50}, (DS_BPLUSTREE_VALUE_TYPE){.value = 100});
    ds_bplustree_delete(bpt, (DS_BPLUSTREE_KEY_TYPE){.key = 30});
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * B+ 树是基于磁盘页的持久化数据结构，通过 _type.h 中的宏管理 key / value 类型：
 *
 *   - key 和 value 必须是编译期定长类型（sizeof 固定、内部不含指针），
 *     序列化与反序列化均通过 memcpy 完成。
 *
 *   - insert 将 key 和 value 写入叶子页，页分裂时数据在页间搬移。
 *     若 malloc 失败（游标等临时对象），操作回滚，B+ 树状态不变。
 *
 *   - delete 从叶子页中删除 key/value，若触发下溢则自动借节点或合并页。
 *     被废弃的页进入自由链表，后续 alloc_page 时优先复用。
 *
 *   - destroy 关闭文件并释放 DS_BPlusTree 结构体，磁盘数据保留。
 *     下次 create 同一文件名可继续操作。
 *
 *   - node_get_data 返回的指针指向游标内部 key / value 副本，可直接读取和修改。
 *     **警告：修改 key 字段会破坏有序性。**
 *     此指针在游标所在节点被 delete / destroy 后失效。
 *
 *   - 遍历回调及 range_query 回调接收 const DS_BPLUSTREE_KEY_TYPE *key 和
 *     const DS_BPLUSTREE_VALUE_TYPE *value。回调操作的是页缓冲区副本，数据为只读；
 *     修改不会持久化到磁盘。如需修改数据，请使用 search + node_get_data 定位后重新 insert。
 *
 *   - clone 将数据文件逐页拷贝至新文件，返回独立的 DS_BPlusTree，使用完毕需
 *     ds_bplustree_destroy 释放。
 *
 *   - search / find_max / find_min / predecessor / successor 返回的 cursor
 *     在下一次 insert / delete / destroy 后可能失效（页分裂、合并、释放导致）。
 *
 *   - Cursor（游标）使用说明
 *     --------------------
 *     BPlusTreeNode * 是 opaque 类型，用户无法直接访问其成员。
 *     通过以下三个函数即可完成所有操作：
 *
 *     find_min / find_max     → 获取起始游标
 *     predecessor / successor → 移动游标
 *     node_get_data           → 读写游标所指元素的 data
 *
 *     cursor == NULL 表示"遍历结束"（类比 std::map::end()）。
 *
 *     **search / find_min / find_max / predecessor / successor 返回的 cursor
 *     是 malloc 分配的堆内存，使用完毕后必须 free(cursor)。遍历时请用临时变量
 *     保存下一个游标后再释放当前游标（参见头文件示例代码）。
 *
 *     cursor 在所在节点被 delete 或整个 B+ 树被 destroy 后立即失效，
 *     在下一次 insert / delete 后也可能因页分裂/合并而失效。
 *
 *   - predecessor / successor 要求 cursor 必须是 bpt 中的有效游标，传入非法指针
 *     会导致未定义行为。
 *
 * 已知局限性
 * --------------
 *   - insert / delete 操作不是原子的：写盘步骤逐页进行，中途 I/O 失败会导致文件
 *     处于部分更新的不一致状态（已写页生效，未写页丢失）。目前无 WAL / 回滚机制，
 *     请勿在可能发生 I/O 错误的场景（磁盘满、意外断电等）依赖本库的数据完整性。
 */
