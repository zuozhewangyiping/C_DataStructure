#ifndef DS_AVLTREE_H
#define DS_AVLTREE_H

#include "ds_avltree_type.h"
#define DS_AVLTREE_TYPE ds_avltree_type

typedef struct DS_AVLTree DS_AVLTree;
typedef struct AVLTreeNode AVLTreeNode; /* opaque cursor */

/*-----------------------------------------------------------------------------
 * MENU
 * Part 1. _create / _destroy
 * Part 2. _height / _count / _is_empty
 * Part 3. _traverse_preorder_value / _traverse_inorder_value / _traverse_postorder_value / _traverse_levelorder_value
 * Part 4. _clone
 * Part 5. _node_get_data
 * Part 6. _search
 * Part 7. _find_max / _find_min
 * Part 8. _predecessor / _successor
 * Part 9. _range_query
 * Part 10. _insert / _delete
 *---------------------------------------------------------------------------*/

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_AVLTree *ds_avltree_create(void); /* return pointer if success, NULL if malloc failed */
void ds_avltree_destroy(DS_AVLTree *tree);
/*
    DS_AVLTree *tree = ds_avltree_create();
    ds_avltree_destroy(tree); // 深度销毁整棵树
    tree = NULL;
*/

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_avltree_height(const DS_AVLTree *tree);   /* return height, or 0 if tree is NULL */
int ds_avltree_count(const DS_AVLTree *tree);    /* return node count, or 0 if tree is NULL, or -1 if traversal failed */
int ds_avltree_is_empty(const DS_AVLTree *tree); /* return 1 if empty, 0 if not empty, -1 if tree is NULL */
/*
    int h = ds_avltree_height(tree);
    int n = ds_avltree_count(tree);
    int e = ds_avltree_is_empty(tree); // 1=empty, 0=not empty, -1=tree is NULL
*/

/* Part 3. Traversals（回调可读写 data）---------------------------------------*/

int ds_avltree_traverse_preorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data));
int ds_avltree_traverse_inorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data));
int ds_avltree_traverse_postorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data));
int ds_avltree_traverse_levelorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data));
/* return 1 if success, 0 if visit is NULL or internal stack allocation failed */

/*
    void visit(DS_AVLTREE_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d, value=%d\n", value->key, value->value); // 只读
        value->value *= 2;                                      // 也可以修改
    }

    ds_avltree_traverse_inorder_value(tree, NULL, visit);
*/

/* Part 4. Clone ------------------------------------------------------------*/

DS_AVLTree *ds_avltree_clone(const DS_AVLTree *tree, int *judge);
/* return pointer if success, NULL if judge is NULL or malloc failed; *judge will be 1 if clone successful, 0 if malloc failed */

/*
    int judge;
    DS_AVLTree *copy = ds_avltree_clone(tree, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败
    }
*/

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_avltree_node_get_data(AVLTreeNode *cursor, DS_AVLTREE_TYPE **x); /* return 1 if success, 0 if cursor or x is NULL */
/*
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key=%d\n", data->key);
    *data = (DS_AVLTREE_TYPE){.key = 99, .value = 0}; // 可以修改，但不要改 key
*/

/* Part 6. Search -----------------------------------------------------------*/

AVLTreeNode *ds_avltree_search(DS_AVLTree *tree, DS_AVLTREE_TYPE value); /* return cursor if found, NULL if tree is NULL, empty, or not found */
/*
    AVLTreeNode *cursor = ds_avltree_search(tree, (DS_AVLTREE_TYPE){.key = 50});
*/

/* Part 7. Cursor Acquisition -----------------------------------------------*/

AVLTreeNode *ds_avltree_find_max(DS_AVLTree *tree);
AVLTreeNode *ds_avltree_find_min(DS_AVLTree *tree);
/* return cursor, or NULL if tree is NULL or empty */

/*
    AVLTreeNode *cursor = ds_avltree_find_min(tree);
*/

/* Part 8. Cursor Navigation ------------------------------------------------*/

AVLTreeNode *ds_avltree_predecessor(DS_AVLTree *tree, AVLTreeNode *cursor);
AVLTreeNode *ds_avltree_successor(DS_AVLTree *tree, AVLTreeNode *cursor);
/* return cursor, or NULL if no predecessor / successor exists */

/*
    // 正向遍历整棵树
    AVLTreeNode *cursor = ds_avltree_find_min(tree);
    while (cursor != NULL)
    {
        DS_AVLTREE_TYPE *data;
        ds_avltree_node_get_data(cursor, &data);
        cursor = ds_avltree_successor(tree, cursor);
    }
*/

/* Part 9. Range Query（回调可读写 data）-------------------------------------*/

int ds_avltree_range_query(DS_AVLTree *tree,
                           DS_AVLTREE_TYPE low,
                           DS_AVLTREE_TYPE high,
                           void *user_data,
                           void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data));
/* return 1 if success, 0 if tree NULL, visit NULL, or stack allocation failed */

/*
    void visit(DS_AVLTREE_TYPE *value, void *user_data)
    {
        (void)user_data;
        printf("key=%d\n", value->key);
    }

    ds_avltree_range_query(tree,
                           (DS_AVLTREE_TYPE){.key = 30},
                           (DS_AVLTREE_TYPE){.key = 70},
                           NULL,
                           visit);
*/

/* Part 10. Insert / Delete -------------------------------------------------*/

int ds_avltree_insert(DS_AVLTree *tree, DS_AVLTREE_TYPE value); /* return 1 if success, 0 if tree NULL, malloc failed, or duplicate key */
int ds_avltree_delete(DS_AVLTree *tree, DS_AVLTREE_TYPE value); /* return 1 if success, 0 if tree NULL, empty, or value not found */
/*
    ds_avltree_insert(tree, (DS_AVLTREE_TYPE){.key = 50, .value = 100});
    ds_avltree_delete(tree, (DS_AVLTREE_TYPE){.key = 30});
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * AVL 树存储泛型元素，通过 _type.h 中的宏管理元素生命周期：
 *
 *   - insert 将 value 深拷贝（CLONE_ELEMENT）到新节点，之后节点拥有该副本。
 *     CLONE 失败时新节点内存立即释放，树状态不变。
 *
 *   - delete / destroy 删除节点时，调用 DESTROY_ELEMENT 清理其 data，
 *     然后 free 节点。
 *
 *   - node_get_data 返回的指针指向节点内部 data，可直接读取和修改。
 *     **警告：修改 key 字段会破坏 BST 性质。**
 *     此指针在节点被 delete / destroy 后失效。
 *
 *   - 遍历回调及 range_query 回调接收 DS_AVLTREE_TYPE *value，可读写 data。
 *     同样，修改 key 会破坏 BST 性质。
 *
 *   - clone 返回完全独立的深拷贝树，使用完毕需 ds_avltree_destroy 释放。
 *
 *   - search / find_max / find_min / predecessor / successor 返回的 cursor
 *     在下一次 insert / delete / clear / destroy 后可能失效（旋转或释放）。
 *
 *   - Cursor（游标）使用说明
 *     --------------------
 *     AVLTreeNode * 是 opaque 类型，用户无法直接访问其成员。
 *     通过以下三个函数即可完成所有操作：
 *
 *     find_min / find_max   → 获取起始游标
 *     predecessor / successor → 移动游标
 *     node_get_data         → 读写游标所指元素的 data
 *
 *     cursor == NULL 表示"遍历结束"（类比 std::map::end()）。
 *
 *     cursor 在所在节点被 delete 或整棵树被 destroy 后立即失效，
 *     在下一次 insert / delete 后也可能因旋转而失效。
 *
 *   - predecessor / successor 要求 cursor 必须是 tree 中的节点，传入非法指针
 *     会导致未定义行为。
 */
