#include <stdlib.h>
#include "ds_singlylinkedlist.h"

struct SinglyLinkedListNode
{
    DS_SINGLYLINKEDLIST_TYPE data;
    struct SinglyLinkedListNode *next;
};

struct DS_SinglyLinkedList
{
    SinglyLinkedListNode *head;
};

DS_SinglyLinkedList *ds_singlylinkedlist_create(void)
{
    DS_SinglyLinkedList *sl = (DS_SinglyLinkedList *)malloc(sizeof(DS_SinglyLinkedList));
    if (sl == NULL)
    {
        return NULL;
    }

    sl->head = NULL;

    return sl;
}

void ds_singlylinkedlist_destroy(DS_SinglyLinkedList *sl)
{
    if (sl == NULL)
    {
        return;
    }

    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        SinglyLinkedListNode *temp = cur;
        cur = cur->next;

        DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(temp->data);
        free(temp);
    }

    free(sl);
}

int ds_singlylinkedlist_traverse(DS_SinglyLinkedList *sl, void *user_data, void (*visit)(DS_SINGLYLINKEDLIST_TYPE *value, void *call_back))
{
    if (sl == NULL || visit == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        visit(&(cur->data), user_data);

        cur = cur->next;
    }

    return 1;
}

static void inner_count(DS_SINGLYLINKEDLIST_TYPE *value, void *call_back)
{
    (void)value;
    (*(int *)call_back)++;
}

int ds_singlylinkedlist_size(DS_SinglyLinkedList *sl)
{
    int ans = 0;

    if (!ds_singlylinkedlist_traverse(sl, &ans, inner_count))
    {
        return -1;
    }

    return ans;
}

int ds_singlylinkedlist_is_empty(const DS_SinglyLinkedList *sl)
{
    if (sl == NULL)
    {
        return -1;
    }

    return (sl->head == NULL ? 1 : 0);
}

int ds_singlylinkedlist_get(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (sl == NULL || x == NULL || index < 0)
    {
        return 0;
    }

    int count = 0;
    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        if (count == index)
        {
            *x = &(cur->data);

            return 1;
        }

        cur = cur->next;
        count++;
    }

    return 0;
}

int ds_singlylinkedlist_set(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE value)
{
    if (sl == NULL || index < 0)
    {
        return 0;
    }

    int count = 0;
    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        if (count == index)
        {
            // step 1.深度CLONE到临时变量temp
            int judge = 1;
            DS_SINGLYLINKEDLIST_TYPE temp = DS_SINGLYLINKEDLIST_CLONE_ELEMENT(value, &judge);
            if (judge == 0)
            {
                DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(temp);
                return 0;
            }

            // step 2.摧毁原来链表节点里的旧data
            DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(cur->data);

            // step 3.新data放入该节点
            cur->data = temp;

            return 1;
        }

        cur = cur->next;
        count++;
    }

    return 0;
}

int ds_singlylinkedlist_find(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_MATCH_TYPE target, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (sl == NULL || x == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        if (DS_SINGLYLINKEDLIST_MATCH(cur->data, target))
        {
            *x = &(cur->data);

            return 1;
        }

        cur = cur->next;
    }

    return 0;
}

SinglyLinkedListNode *ds_singlylinkedlist_search(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_MATCH_TYPE target)
{
    if (sl == NULL)
    {
        return NULL;
    }

    SinglyLinkedListNode *cur = sl->head;

    while (cur != NULL)
    {
        if (DS_SINGLYLINKEDLIST_MATCH(cur->data, target))
        {
            return cur;
        }

        cur = cur->next;
    }

    return NULL;
}

static SinglyLinkedListNode *create_Node(DS_SINGLYLINKEDLIST_TYPE value)
{
    SinglyLinkedListNode *new_node = (SinglyLinkedListNode *)malloc(sizeof(SinglyLinkedListNode));
    if (new_node == NULL)
    {
        return NULL;
    }

    // step 1.深度CLONE到临时变量temp
    int judge = 1;
    DS_SINGLYLINKEDLIST_TYPE temp = DS_SINGLYLINKEDLIST_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(temp);
        free(new_node);
        return NULL;
    }

    // step 2.新data放入该节点
    new_node->data = temp;

    new_node->next = NULL;

    return new_node;
}

int ds_singlylinkedlist_push_front(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE value)
{
    if (sl == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur = sl->head;

    new_node->next = cur;

    sl->head = new_node;

    return 1;
}

int ds_singlylinkedlist_push_back(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE value)
{
    if (sl == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur = sl->head;

    if (cur == NULL)
    {
        sl->head = new_node;
    }
    else
    {
        while (cur->next != NULL)
        {
            cur = cur->next;
        }

        cur->next = new_node;
    }

    return 1;
}

int ds_singlylinkedlist_insert(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE value)
{
    if (sl == NULL || index < 0)
    {
        return 0;
    }

    if (index == 0)
    {
        return ds_singlylinkedlist_push_front(sl, value);
    }

    if (sl->head == NULL) // 此时链表为空，且索引>0，不合法
    {
        return 0;
    }

    SinglyLinkedListNode *cur_1 = sl->head;
    SinglyLinkedListNode *cur_2 = NULL;
    int i = 0;

    while (cur_1 != NULL)
    {
        if (i == index)
        {
            SinglyLinkedListNode *new_node = create_Node(value);
            if (new_node == NULL)
            {
                return 0;
            }

            cur_2->next = new_node;
            new_node->next = cur_1;

            return 1;
        }

        cur_2 = cur_1;
        cur_1 = cur_1->next;
        i++;
    }

    if (i == index)
    {
        SinglyLinkedListNode *new_node = create_Node(value);
        if (new_node == NULL)
        {
            return 0;
        }

        cur_2->next = new_node;

        return 1;
    }

    return 0;
}

int ds_singlylinkedlist_pop_front(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (sl == NULL || x == NULL || sl->head == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur_1 = sl->head;
    SinglyLinkedListNode *cur_2 = cur_1->next;

    DS_SINGLYLINKEDLIST_TYPE *popped = malloc(sizeof(DS_SINGLYLINKEDLIST_TYPE));
    if (popped == NULL)
    {
        return 0;
    }

    *popped = cur_1->data; // 内联 data 拷贝到新堆块
    *x = popped;
    free(cur_1);

    sl->head = cur_2;

    return 1;
}

int ds_singlylinkedlist_pop_front_and_destroy(DS_SinglyLinkedList *sl)
{
    DS_SINGLYLINKEDLIST_TYPE *x = NULL;

    if (!ds_singlylinkedlist_pop_front(sl, &x))
    {
        return 0;
    }

    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_singlylinkedlist_pop_back(DS_SinglyLinkedList *sl, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (sl == NULL || x == NULL || sl->head == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *cur_1 = sl->head;
    SinglyLinkedListNode *cur_2 = NULL;

    while (cur_1->next != NULL)
    {
        cur_2 = cur_1;
        cur_1 = cur_1->next;
    }

    DS_SINGLYLINKEDLIST_TYPE *popped = malloc(sizeof(DS_SINGLYLINKEDLIST_TYPE));
    if (popped == NULL)
    {
        return 0;
    }

    *popped = cur_1->data; // 内联 data 拷贝到新堆块
    *x = popped;
    free(cur_1);

    if (cur_2 == NULL)
    {
        sl->head = NULL;
    }
    else
    {
        cur_2->next = NULL;
    }

    return 1;
}

int ds_singlylinkedlist_pop_back_and_destroy(DS_SinglyLinkedList *sl)
{
    DS_SINGLYLINKEDLIST_TYPE *x = NULL;

    if (!ds_singlylinkedlist_pop_back(sl, &x))
    {
        return 0;
    }

    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_singlylinkedlist_erase(DS_SinglyLinkedList *sl, int index, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (sl == NULL || x == NULL || index < 0)
    {
        return 0;
    }

    if (index == 0)
    {
        return ds_singlylinkedlist_pop_front(sl, x);
    }

    if (sl->head == NULL) // 此时链表为空，且索引>0，不合法
    {
        return 0;
    }

    // 剩下的情况为：链表不为空，且索引>0（所以删除的不可能是头节点，那么最终找到时cur_2 != NULL）

    SinglyLinkedListNode *cur_1 = sl->head;
    SinglyLinkedListNode *cur_2 = NULL;
    int i = 0;

    while (cur_1 != NULL)
    {
        if (i == index)
        {
            DS_SINGLYLINKEDLIST_TYPE *erased = malloc(sizeof(DS_SINGLYLINKEDLIST_TYPE));
            if (erased == NULL)
            {
                return 0;
            }

            cur_2->next = cur_1->next;

            *erased = cur_1->data; // 内联 data 拷贝到新堆块
            *x = erased;
            free(cur_1);

            return 1;
        }

        cur_2 = cur_1;
        cur_1 = cur_1->next;
        i++;
    }

    return 0;
}

int ds_singlylinkedlist_erase_and_destroy(DS_SinglyLinkedList *sl, int index)
{
    DS_SINGLYLINKEDLIST_TYPE *x = NULL;

    if (!ds_singlylinkedlist_erase(sl, index, &x))
    {
        return 0;
    }

    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_singlylinkedlist_insert_after_cursor(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE value)
{
    if (cursor == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    new_node->next = cursor->next;
    cursor->next = new_node;

    return 1;
}

int ds_singlylinkedlist_erase_after_cursor(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (cursor == NULL || x == NULL || cursor->next == NULL)
    {
        return 0;
    }

    SinglyLinkedListNode *victim = cursor->next;

    DS_SINGLYLINKEDLIST_TYPE *erased = malloc(sizeof(DS_SINGLYLINKEDLIST_TYPE));
    if (erased == NULL)
    {
        return 0;
    }

    *erased = victim->data;
    *x = erased;

    cursor->next = victim->next;

    free(victim);

    return 1;
}

int ds_singlylinkedlist_erase_after_cursor_and_destroy(SinglyLinkedListNode *cursor)
{
    DS_SINGLYLINKEDLIST_TYPE *x = NULL;

    if (!ds_singlylinkedlist_erase_after_cursor(cursor, &x))
    {
        return 0;
    }

    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

DS_SinglyLinkedList *ds_singlylinkedlist_clone(const DS_SinglyLinkedList *sl, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (sl == NULL)
    {
        return NULL;
    }

    DS_SinglyLinkedList *new_sl = ds_singlylinkedlist_create();
    if (new_sl == NULL)
    {
        *judge = 0;
        return NULL;
    }

    SinglyLinkedListNode *cur = sl->head;
    SinglyLinkedListNode *cur_new = NULL;

    while (cur != NULL)
    {
        SinglyLinkedListNode *new_node = create_Node(cur->data);
        if (new_node == NULL)
        {
            ds_singlylinkedlist_destroy(new_sl);
            *judge = 0;
            return NULL;
        }

        if (cur_new == NULL)
        {
            new_sl->head = new_node;
        }
        else
        {
            cur_new->next = new_node;
        }

        cur_new = new_node;

        cur = cur->next;
    }

    return new_sl;
}

int ds_singlylinkedlist_concat(DS_SinglyLinkedList *sl1, DS_SinglyLinkedList *sl2)
{
    if (sl1 == NULL || sl1 == sl2)
    {
        return 0;
    }
    if (sl2 == NULL || sl2->head == NULL)
    {
        return 1;
    }

    if (sl1->head == NULL)
    {
        sl1->head = sl2->head;

        sl2->head = NULL;

        return 1;
    }

    SinglyLinkedListNode *cur = sl1->head;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    cur->next = sl2->head;

    sl2->head = NULL;

    return 1;
}

SinglyLinkedListNode *ds_singlylinkedlist_begin(DS_SinglyLinkedList *sl)
{
    if (sl == NULL || sl->head == NULL)
    {
        return NULL;
    }

    return sl->head;
}

SinglyLinkedListNode *ds_singlylinkedlist_next(SinglyLinkedListNode *cursor)
{
    if (cursor == NULL)
    {
        return NULL;
    }

    return cursor->next;
}

int ds_singlylinkedlist_node_get_data(SinglyLinkedListNode *cursor, DS_SINGLYLINKEDLIST_TYPE **x)
{
    if (cursor == NULL || x == NULL)
    {
        return 0;
    }

    *x = &(cursor->data);

    return 1;
}
