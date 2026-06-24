#include <stdlib.h>
#include "ds_skiplist.h"

#define MAX_LEVEL 32

struct SkipListNode
{
    DS_SKIPLIST_TYPE data;
    int level;
    struct SkipListNode *next[];
};

struct DS_SkipList
{
    SkipListNode *header;
    int size;
    int level;
};

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_SkipList *ds_skiplist_create(void)
{
    DS_SkipList *skl = (DS_SkipList *)malloc(sizeof(DS_SkipList));
    if (skl == NULL)
    {
        return NULL;
    }

    skl->header = (SkipListNode *)malloc(sizeof(SkipListNode) + MAX_LEVEL * sizeof(SkipListNode *));
    if (skl->header == NULL)
    {
        free(skl);
        return NULL;
    }

    int i = 0;
    for (i = 0; i < MAX_LEVEL; i++)
    {
        skl->header->next[i] = NULL;
    }

    skl->level = 0;
    skl->size = 0;

    return skl;
}

void ds_skiplist_destroy(DS_SkipList *skl)
{
    if (skl == NULL)
    {
        return;
    }

    SkipListNode *cur = skl->header->next[0];

    while (cur != NULL)
    {
        SkipListNode *temp = cur;
        cur = cur->next[0];

        DS_SKIPLIST_DESTROY_ELEMENT(temp->data);
        free(temp);
    }

    free(skl->header);
    free(skl);
}

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_skiplist_size(const DS_SkipList *skl)
{
    if (skl == NULL)
    {
        return 0;
    }

    return skl->size;
}

int ds_skiplist_level(const DS_SkipList *skl)
{
    if (skl == NULL)
    {
        return 0;
    }

    return skl->level;
}

int ds_skiplist_is_empty(const DS_SkipList *skl)
{
    if (skl == NULL)
    {
        return -1;
    }

    return (skl->size == 0 ? 1 : 0);
}

/* Part 3. Traversals（回调可读写 data）---------------------------------------*/

int ds_skiplist_traverse(DS_SkipList *skl, void *user_data, void (*visit)(DS_SKIPLIST_TYPE *value, void *callback_data))
{
    if (skl == NULL || visit == NULL)
    {
        return 0;
    }

    SkipListNode *cur = skl->header->next[0];

    while (cur != NULL)
    {
        visit(&(cur->data), user_data);

        cur = cur->next[0];
    }

    return 1;
}

/* Part 4. Clone ------------------------------------------------------------*/

static SkipListNode *create_node(DS_SKIPLIST_TYPE data, int level)
{
    SkipListNode *new_node = (SkipListNode *)malloc(sizeof(SkipListNode) + level * sizeof(SkipListNode *));
    if (new_node == NULL)
    {
        return NULL;
    }

    // step 1.深度CLONE到临时变量temp
    int judge = 1;
    DS_SKIPLIST_TYPE temp = DS_SKIPLIST_CLONE_ELEMENT(data, &judge);
    if (judge == 0)
    {
        DS_SKIPLIST_DESTROY_ELEMENT(temp);
        free(new_node);
        return NULL;
    }

    // step 2.新data放入该节点
    new_node->data = temp;

    // step 3.补充其他字段
    new_node->level = level;
    int i = 0;
    for (i = 0; i < level; i++)
    {
        new_node->next[i] = NULL;
    }

    return new_node;
}

DS_SkipList *ds_skiplist_clone(const DS_SkipList *skl, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (skl == NULL)
    {
        return NULL;
    }

    DS_SkipList *new_skl = ds_skiplist_create();
    if (new_skl == NULL)
    {
        *judge = 0;
        return NULL;
    }

    SkipListNode *cur = skl->header->next[0];

    SkipListNode *prev[MAX_LEVEL];
    int i = 0;
    for (i = 0; i < MAX_LEVEL; i++)
    {
        prev[i] = new_skl->header;
    }

    while (cur != NULL)
    {
        SkipListNode *new_node = create_node(cur->data, cur->level);
        if (new_node == NULL)
        {
            ds_skiplist_destroy(new_skl);
            *judge = 0;
            return NULL;
        }

        for (i = 0; i < cur->level; i++)
        {
            prev[i]->next[i] = new_node;
            prev[i] = new_node;
        }

        cur = cur->next[0];
    }

    new_skl->level = skl->level;
    new_skl->size = skl->size;

    return new_skl;
}

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_skiplist_node_get_data(SkipListNode *cursor, DS_SKIPLIST_TYPE **x)
{
    if (cursor == NULL || x == NULL)
    {
        return 0;
    }

    *x = &(cursor->data);

    return 1;
}

/* Part 6. Search -----------------------------------------------------------*/

SkipListNode *ds_skiplist_search(DS_SkipList *skl, DS_SKIPLIST_TYPE value)
{
    if (skl == NULL || skl->size == 0)
    {
        return NULL;
    }

    SkipListNode *cur = skl->header;

    int i = skl->level - 1; // 从最高层开始查找
    while (1)
    {
        if (cur->next[i] == NULL)
        {
            if (i > 0)
            {
                i--;
            }
            else
            {
                return NULL;
            }
        }
        else if (DS_SKIPLIST_LT(cur->next[i]->data, value)) // 如果cur的下一个 < 目标，那么cur在本层后移
        {
            cur = cur->next[i];
        }
        else if (DS_SKIPLIST_GT(cur->next[i]->data, value)) // 如果cur的下一个 > 目标，那么降层/返回
        {
            if (i > 0)
            {
                i--;
            }
            else
            {
                return NULL;
            }
        }
        else // if (DS_SKIPLIST_EQ(cur->next[i]->data, value)) // 如果cur的下一个 == 目标，那么返回cur的下一个
        {
            return cur->next[i];
        }
    }
}

/* Part 7. Cursor Acquisition -----------------------------------------------*/

SkipListNode *ds_skiplist_find_max(DS_SkipList *skl)
{
    if (skl == NULL || skl->size == 0)
    {
        return NULL;
    }

    SkipListNode *cur = skl->header;
    SkipListNode *temp = NULL;

    int i = skl->level - 1; // 从最高层开始查找
    while (1)
    {
        while (cur->next[i] == NULL)
        {
            if (i > 0)
            {
                i--;
            }
            else
            {
                return temp;
            }
        }

        temp = cur->next[i];
        cur = cur->next[i];
    }
}

SkipListNode *ds_skiplist_find_min(DS_SkipList *skl)
{
    if (skl == NULL || skl->header->next[0] == NULL)
    {
        return NULL;
    }

    return skl->header->next[0];
}

/* Part 8. Cursor Navigation ------------------------------------------------*/

SkipListNode *ds_skiplist_predecessor(DS_SkipList *skl, SkipListNode *cursor)
{
    if (skl == NULL || skl->size == 0 || cursor == NULL || cursor == skl->header->next[0])
    {
        return NULL;
    }

    // 上面已经排除了“没有前驱”的情况，所以一定不会返回NULL

    SkipListNode *cur = skl->header;

    int i = skl->level - 1; // 从最高层开始查找
    while (1)
    {
        if (cur->next[i] == NULL)
        {
            if (i > 0)
            {
                i--;
            }
            else
            {
                return cur;
            }
        }
        else if (DS_SKIPLIST_LT(cur->next[i]->data, cursor->data)) // 如果cur的下一个 < 目标，那么cur在本层后移
        {
            cur = cur->next[i];
        }
        else // if (DS_SKIPLIST_GE(cur->next[i]->data, cursor->data)) // 如果cur的下一个 >= 目标，那么分类讨论（理论上，如果“cur的下一个 > 目标”，那么只能降层；但是与 == 的情况合并起来更方便，而 == 情况需要分类讨论。所以干脆直接统一分类讨论）
        {
            if (i > 0) // 如果还没到最底层，那么降层
            {
                i--;
            }
            else // 如果已经到了最底层，那么cur肯定是目标的前驱
            {
                return cur;
            }
        }
    }
}

SkipListNode *ds_skiplist_successor(DS_SkipList *skl, SkipListNode *cursor)
{
    if (skl == NULL || skl->size == 0 || cursor == NULL)
    {
        return NULL;
    }

    return cursor->next[0];
}

/* Part 9. Range Query（回调可读写 data）-------------------------------------*/

int ds_skiplist_range_query(DS_SkipList *skl,
                            DS_SKIPLIST_TYPE low,
                            DS_SKIPLIST_TYPE high,
                            void *user_data,
                            void (*visit)(DS_SKIPLIST_TYPE *value, void *callback_data))
{
    if (skl == NULL)
    {
        return 0;
    }

    if (skl->size == 0)
    {
        return 1;
    }

    if (visit == NULL)
    {
        return 0;
    }

    if (DS_SKIPLIST_GT(low, high))
    {
        return 1;
    }

    SkipListNode *begin = NULL; // 设定begin为大于等于low的最小元素

    // 找begin
    SkipListNode *cur = skl->header;

    int i = skl->level - 1; // 从最高层开始查找
    while (1)
    {
        if (cur->next[i] == NULL) // 若后移之后cur的下一个不存在，那么分类讨论
        {
            if (i > 0) // 若未降到最低层，就往下降
            {
                i--;
            }
            else // 若已经是最底层，那么说明本表中不存在大于等于low的元素
            {
                return 1;
            }
        }
        else if (DS_SKIPLIST_LT(cur->next[i]->data, low)) // 如果cur的下一个 < 目标，那么cur在本层后移
        {
            cur = cur->next[i];
        }
        else if (DS_SKIPLIST_GT(cur->next[i]->data, low)) // 如果cur的下一个 > 目标，那么分类讨论
        {
            if (i > 0) // 若未降到最低层，就往下降
            {
                i--;
            }
            else // 若已经是最底层，那么“cur的下一个”就是我们要找的begin
            {
                begin = cur->next[i];

                break;
            }
        }
        else // if (DS_SKIPLIST_EQ(cur->next[i]->data, low)) // 如果cur的下一个 == 目标，那么“cur的下一个”就是我们要找的begin
        {
            begin = cur->next[i];

            break;
        }
    }

    // 找到begin之后开始遍历
    SkipListNode *cur_ = begin;
    while (cur_ != NULL && DS_SKIPLIST_LE(cur_->data, high))
    {
        visit(&(cur_->data), user_data);
        cur_ = cur_->next[0];
    }

    return 1;
}

/* Part 10. Insert / Delete -------------------------------------------------*/

static int random_level(void)
{
    int level = 1;
    while (rand() % 2 == 0 && level < MAX_LEVEL)
    {
        level++;
    }

    return level;
}

int ds_skiplist_insert(DS_SkipList *skl, DS_SKIPLIST_TYPE value)
{
    if (skl == NULL)
    {
        return 0;
    }

    SkipListNode *prev[MAX_LEVEL];
    int i = 0;
    for (i = 0; i < MAX_LEVEL; i++)
    {
        prev[i] = skl->header;
    }

    SkipListNode *cur = skl->header;

    if (skl->size > 0)
    {
        i = skl->level - 1; // 从最高层开始查找
        while (1)
        {
            if (cur->next[i] == NULL)
            {
                if (i > 0)
                {
                    prev[i] = cur;
                    i--;
                }
                else
                {
                    prev[i] = cur;
                    break;
                }
            }
            else if (DS_SKIPLIST_LT(cur->next[i]->data, value)) // 如果cur的下一个 < 目标，那么cur在本层后移
            {
                cur = cur->next[i];
            }
            else if (DS_SKIPLIST_GT(cur->next[i]->data, value)) // 如果cur的下一个 > 目标，那么分类讨论
            {
                if (i > 0) // 若未降到最低层，就往下降
                {
                    prev[i] = cur;
                    i--;
                }
                else // 若已经是最底层，那么cur就是我们要找的插入点
                {
                    prev[i] = cur;
                    break;
                }
            }
            else // if (DS_SKIPLIST_EQ(cur->next[i]->data, low)) // 如果cur的下一个 == 目标，那么拒绝插入，直接返回
            {
                return 0;
            }
        }
    }

    int level = random_level();

    SkipListNode *new_node = create_node(value, level);
    if (new_node == NULL)
    {
        return 0;
    }

    for (i = 0; i < level; i++)
    {
        SkipListNode *temp = prev[i]->next[i];
        prev[i]->next[i] = new_node;
        new_node->next[i] = temp;
    }

    if (level > skl->level)
    {
        skl->level = level;
    }

    skl->size++;

    return 1;
}

int ds_skiplist_delete(DS_SkipList *skl, DS_SKIPLIST_TYPE value)
{
    if (skl == NULL || skl->size == 0)
    {
        return 0;
    }

    SkipListNode *prev[MAX_LEVEL];
    int i = 0;
    for (i = 0; i < MAX_LEVEL; i++)
    {
        prev[i] = skl->header;
    }

    SkipListNode *victim = NULL;

    SkipListNode *cur = skl->header;

    i = skl->level - 1; // 从最高层开始查找
    while (1)
    {
        if (cur->next[i] == NULL)
        {
            if (i > 0)
            {
                prev[i] = cur;
                i--;
            }
            else
            {
                return 0;
            }
        }
        else if (DS_SKIPLIST_LT(cur->next[i]->data, value)) // 如果cur的下一个 < 目标，那么cur在本层后移
        {
            cur = cur->next[i];
        }
        else if (DS_SKIPLIST_GT(cur->next[i]->data, value)) // 如果cur的下一个 > 目标，那么降层/返回
        {
            if (i > 0)
            {
                prev[i] = cur;
                i--;
            }
            else
            {
                return 0;
            }
        }
        else // if (DS_SKIPLIST_EQ(cur->next[i]->data, value)) // 如果cur的下一个 == 目标，那么victim为cur的下一个；但是要继续向下搜索
        {
            victim = cur->next[i];

            if (i > 0)
            {
                prev[i] = cur;
                i--;
            }
            else
            {
                prev[i] = cur;
                break;
            }
        }
    }

    for (i = 0; i < victim->level; i++)
    {
        prev[i]->next[i] = victim->next[i];
    }

    DS_SKIPLIST_DESTROY_ELEMENT(victim->data);
    free(victim);

    // 删掉某些节点可能会改变skl->level，需要检查更新
    while (skl->level > 0 && skl->header->next[skl->level - 1] == NULL) // 需要防止level被减到负数而导致数组越界
    {
        skl->level--;
    }

    skl->size--;

    return 1;
}
