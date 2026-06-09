#include <stdlib.h>
#include "ds_doublelinkedlist.h"

struct DoubleLinkedListNode
{
    DS_DOUBLELINKEDLIST_TYPE data;
    struct DoubleLinkedListNode *next;
    struct DoubleLinkedListNode *prev;
};

struct DS_DoubleLinkedList
{
    DoubleLinkedListNode *head;
    DoubleLinkedListNode *tail;
};

DS_DoubleLinkedList *ds_doublelinkedlist_create(void)
{
    DS_DoubleLinkedList *dl = (DS_DoubleLinkedList *)malloc(sizeof(DS_DoubleLinkedList));
    if (dl == NULL)
    {
        return NULL;
    }

    dl->head = NULL;
    dl->tail = NULL;

    return dl;
}

void ds_doublelinkedlist_destroy(DS_DoubleLinkedList *dl)
{
    if (dl == NULL)
    {
        return;
    }

    DoubleLinkedListNode *cur = dl->head;

    while (cur != NULL)
    {
        DoubleLinkedListNode *temp = cur;
        cur = cur->next;

        DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(temp->data);
        free(temp);
    }

    free(dl);
}

int ds_doublelinkedlist_traverse(DS_DoubleLinkedList *dl, void *user_data, void (*visit)(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back))
{
    if (dl == NULL || visit == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *cur = dl->head;

    while (cur != NULL)
    {
        visit(&(cur->data), user_data);

        cur = cur->next;
    }

    return 1;
}

int ds_doublelinkedlist_rtraverse(DS_DoubleLinkedList *dl, void *user_data, void (*visit)(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back))
{
    if (dl == NULL || visit == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *cur = dl->tail;

    while (cur != NULL)
    {
        visit(&(cur->data), user_data);

        cur = cur->prev;
    }

    return 1;
}

static void inner_count(DS_DOUBLELINKEDLIST_TYPE *value, void *call_back)
{
    (void)value;
    (*(int *)call_back)++;
}

int ds_doublelinkedlist_size(DS_DoubleLinkedList *dl)
{
    int ans = 0;

    if (!ds_doublelinkedlist_traverse(dl, &ans, inner_count))
    {
        return -1;
    }

    return ans;
}

int ds_doublelinkedlist_is_empty(const DS_DoubleLinkedList *dl)
{
    if (dl == NULL)
    {
        return -1;
    }

    return (dl->head == NULL ? 1 : 0);
}

int ds_doublelinkedlist_get(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || x == NULL || index < 0)
    {
        return 0;
    }

    int count = 0;
    DoubleLinkedListNode *cur = dl->head;

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

int ds_doublelinkedlist_set(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL || index < 0)
    {
        return 0;
    }

    int count = 0;
    DoubleLinkedListNode *cur = dl->head;

    while (cur != NULL)
    {
        if (count == index)
        {
            // step 1.深度CLONE到临时变量temp
            int judge = 1;
            DS_DOUBLELINKEDLIST_TYPE temp = DS_DOUBLELINKEDLIST_CLONE_ELEMENT(value, &judge);
            if (judge == 0)
            {
                DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(temp);
                return 0;
            }

            // step 2.摧毁原来链表节点里的旧data
            DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(cur->data);

            // step 3.新data放入该节点
            cur->data = temp;

            return 1;
        }

        cur = cur->next;
        count++;
    }

    return 0;
}

int ds_doublelinkedlist_find(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_MATCH_TYPE target, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || x == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *cur = dl->head;

    while (cur != NULL)
    {
        if (DS_DOUBLELINKEDLIST_MATCH(cur->data, target))
        {
            *x = &(cur->data);

            return 1;
        }

        cur = cur->next;
    }

    return 0;
}

DoubleLinkedListNode *ds_doublelinkedlist_search(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_MATCH_TYPE target)
{
    if (dl == NULL)
    {
        return NULL;
    }

    DoubleLinkedListNode *cur = dl->head;

    while (cur != NULL)
    {
        if (DS_DOUBLELINKEDLIST_MATCH(cur->data, target))
        {
            return cur;
        }

        cur = cur->next;
    }

    return NULL;
}

static DoubleLinkedListNode *create_Node(DS_DOUBLELINKEDLIST_TYPE value)
{
    DoubleLinkedListNode *new_node = (DoubleLinkedListNode *)malloc(sizeof(DoubleLinkedListNode));
    if (new_node == NULL)
    {
        return NULL;
    }

    // step 1.深度CLONE到临时变量temp
    int judge = 1;
    DS_DOUBLELINKEDLIST_TYPE temp = DS_DOUBLELINKEDLIST_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(temp);
        free(new_node);
        return NULL;
    }

    // step 2.新data放入该节点
    new_node->data = temp;

    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

int ds_doublelinkedlist_push_front(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    if (dl->head == NULL)
    {
        new_node->next = NULL;
        new_node->prev = NULL;

        dl->head = new_node;
        dl->tail = new_node;

        return 1;
    }

    new_node->next = dl->head;
    new_node->prev = NULL;

    dl->head->prev = new_node;

    dl->head = new_node;

    return 1;
}

int ds_doublelinkedlist_push_back(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    if (dl->head == NULL)
    {
        new_node->next = NULL;
        new_node->prev = NULL;

        dl->head = new_node;
        dl->tail = new_node;

        return 1;
    }

    new_node->prev = dl->tail;
    new_node->next = NULL;

    dl->tail->next = new_node;

    dl->tail = new_node;

    return 1;
}

int ds_doublelinkedlist_insert(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL || index < 0)
    {
        return 0;
    }

    if (index == 0)
    {
        return ds_doublelinkedlist_push_front(dl, value);
    }

    if (dl->head == NULL) // 此时链表为空，且索引>0，不合法
    {
        return 0;
    }

    DoubleLinkedListNode *cur_1 = dl->head;
    DoubleLinkedListNode *cur_2 = NULL;
    int i = 0;

    while (cur_1 != NULL)
    {
        if (i == index)
        {
            // cur_2 不可能为 NULL：index == 0 已在前面被 push_front 分流，
            // 进入此处时循环至少执行过一轮，cur_2 已被赋值
            if (cur_2 == NULL)
            {
                return 0; // 逻辑防御
            }

            DoubleLinkedListNode *new_node = create_Node(value);
            if (new_node == NULL)
            {
                return 0;
            }

            cur_2->next = new_node;
            new_node->prev = cur_2;
            new_node->next = cur_1;
            cur_1->prev = new_node;

            return 1;
        }

        cur_2 = cur_1;
        cur_1 = cur_1->next;
        i++;
    }

    if (i == index)
    {
        DoubleLinkedListNode *new_node = create_Node(value);
        if (new_node == NULL)
        {
            return 0;
        }

        cur_2->next = new_node;
        new_node->prev = cur_2;

        dl->tail = new_node;

        return 1;
    }

    return 0;
}

int ds_doublelinkedlist_pop_front(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || x == NULL || dl->head == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *cur_1 = dl->head;
    DoubleLinkedListNode *cur_2 = cur_1->next;

    DS_DOUBLELINKEDLIST_TYPE *popped = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
    if (popped == NULL)
    {
        return 0;
    }

    *popped = cur_1->data; // 内联 data 拷贝到新堆块
    *x = popped;
    free(cur_1);

    if (cur_2 == NULL)
    {
        dl->tail = NULL;
    }
    else
    {
        cur_2->prev = NULL;
    }

    dl->head = cur_2;

    return 1;
}

int ds_doublelinkedlist_pop_front_and_destroy(DS_DoubleLinkedList *dl)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_pop_front(dl, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_doublelinkedlist_pop_back(DS_DoubleLinkedList *dl, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || x == NULL || dl->head == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *cur_1 = dl->tail;
    DoubleLinkedListNode *cur_2 = cur_1->prev;

    DS_DOUBLELINKEDLIST_TYPE *popped = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
    if (popped == NULL)
    {
        return 0;
    }

    *popped = cur_1->data; // 内联 data 拷贝到新堆块
    *x = popped;
    free(cur_1);

    if (cur_2 == NULL)
    {
        dl->head = NULL;
    }
    else
    {
        cur_2->next = NULL;
    }

    dl->tail = cur_2;

    return 1;
}

int ds_doublelinkedlist_pop_back_and_destroy(DS_DoubleLinkedList *dl)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_pop_back(dl, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_doublelinkedlist_erase(DS_DoubleLinkedList *dl, int index, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || x == NULL || index < 0)
    {
        return 0;
    }

    if (index == 0)
    {
        return ds_doublelinkedlist_pop_front(dl, x);
    }

    if (dl->head == NULL) // 此时链表为空，且索引>0，不合法
    {
        return 0;
    }

    // 剩下的情况为：链表不为空，且索引>0（所以删除的不可能是头节点，那么最终找到时cur_2 != NULL）

    DoubleLinkedListNode *cur_1 = dl->head;
    DoubleLinkedListNode *cur_2 = NULL;
    int i = 0;

    while (cur_1 != NULL)
    {
        if (i == index)
        {
            DS_DOUBLELINKEDLIST_TYPE *erased = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
            if (erased == NULL)
            {
                return 0;
            }

            cur_2->next = cur_1->next;

            if (cur_1->next == NULL) // 删除的是尾节点
            {
                dl->tail = cur_2;
            }
            else
            {
                cur_1->next->prev = cur_2; // 后一节点回指 cur_2
            }

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

int ds_doublelinkedlist_erase_and_destroy(DS_DoubleLinkedList *dl, int index)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_erase(dl, index, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_doublelinkedlist_insert_after_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL || cursor == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    new_node->next = cursor->next;
    new_node->prev = cursor;

    if (cursor->next == NULL)
    {
        dl->tail = new_node;
    }
    else
    {
        cursor->next->prev = new_node;
    }

    cursor->next = new_node;

    return 1;
}

int ds_doublelinkedlist_insert_before_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE value)
{
    if (dl == NULL || cursor == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    new_node->prev = cursor->prev;
    new_node->next = cursor;

    if (cursor->prev == NULL)
    {
        dl->head = new_node;
    }
    else
    {
        cursor->prev->next = new_node;
    }

    cursor->prev = new_node;

    return 1;
}

int ds_doublelinkedlist_erase_after_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || cursor == NULL || x == NULL || cursor->next == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *victim = cursor->next;

    DS_DOUBLELINKEDLIST_TYPE *erased = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
    if (erased == NULL)
    {
        return 0;
    }

    *erased = victim->data;
    *x = erased;

    cursor->next = victim->next;

    if (victim->next == NULL)
    {
        dl->tail = cursor;
    }
    else
    {
        victim->next->prev = cursor;
    }

    free(victim);

    return 1;
}

int ds_doublelinkedlist_erase_after_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_erase_after_cursor(dl, cursor, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_doublelinkedlist_erase_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || cursor == NULL || x == NULL)
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_TYPE *erased = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
    if (erased == NULL)
    {
        return 0;
    }

    *erased = cursor->data;
    *x = erased;

    if (dl->head == cursor && dl->tail == cursor)
    {
        dl->head = NULL;
        dl->tail = NULL;
    }
    else
    {
        if (dl->head == cursor)
        {
            dl->head = cursor->next;
            cursor->next->prev = NULL;
        }
        else if (dl->tail == cursor)
        {
            dl->tail = cursor->prev;
            cursor->prev->next = NULL;
        }
        else
        {
            cursor->prev->next = cursor->next;
            cursor->next->prev = cursor->prev;
        }
    }

    free(cursor);

    return 1;
}

int ds_doublelinkedlist_erase_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_erase_cursor(dl, cursor, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

int ds_doublelinkedlist_erase_before_cursor(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (dl == NULL || cursor == NULL || x == NULL || cursor->prev == NULL)
    {
        return 0;
    }

    DoubleLinkedListNode *victim = cursor->prev;

    DS_DOUBLELINKEDLIST_TYPE *erased = malloc(sizeof(DS_DOUBLELINKEDLIST_TYPE));
    if (erased == NULL)
    {
        return 0;
    }

    *erased = victim->data;
    *x = erased;

    cursor->prev = victim->prev;

    if (victim->prev == NULL)
    {
        dl->head = cursor;
    }
    else
    {
        victim->prev->next = cursor;
    }

    free(victim);

    return 1;
}

int ds_doublelinkedlist_erase_before_cursor_and_destroy(DS_DoubleLinkedList *dl, DoubleLinkedListNode *cursor)
{
    DS_DOUBLELINKEDLIST_TYPE *x = NULL;

    if (!ds_doublelinkedlist_erase_before_cursor(dl, cursor, &x))
    {
        return 0;
    }

    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

DS_DoubleLinkedList *ds_doublelinkedlist_clone(const DS_DoubleLinkedList *dl, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (dl == NULL)
    {
        return NULL;
    }

    DS_DoubleLinkedList *new_dl = ds_doublelinkedlist_create();
    if (new_dl == NULL)
    {
        *judge = 0;
        return NULL;
    }

    DoubleLinkedListNode *cur = dl->head;
    DoubleLinkedListNode *cur_new = NULL;

    while (cur != NULL)
    {
        DoubleLinkedListNode *new_node = create_Node(cur->data);
        if (new_node == NULL)
        {
            ds_doublelinkedlist_destroy(new_dl);
            *judge = 0;
            return NULL;
        }

        if (cur_new == NULL)
        {
            new_dl->head = new_node;
        }
        else
        {
            cur_new->next = new_node;
            new_node->prev = cur_new;
        }

        cur_new = new_node;

        cur = cur->next;
    }

    new_dl->tail = cur_new;

    return new_dl;
}

int ds_doublelinkedlist_concat(DS_DoubleLinkedList *dl1, DS_DoubleLinkedList *dl2)
{
    if (dl1 == NULL || dl1 == dl2)
    {
        return 0;
    }
    if (dl2 == NULL || dl2->head == NULL)
    {
        return 1;
    }

    if (dl1->head == NULL)
    {
        dl1->head = dl2->head;
        dl1->tail = dl2->tail;

        dl2->head = NULL;
        dl2->tail = NULL;

        return 1;
    }

    dl1->tail->next = dl2->head;
    dl2->head->prev = dl1->tail;

    dl1->tail = dl2->tail;

    dl2->head = NULL;
    dl2->tail = NULL;

    return 1;
}

DoubleLinkedListNode *ds_doublelinkedlist_begin(DS_DoubleLinkedList *dl)
{
    if (dl == NULL || dl->head == NULL)
    {
        return NULL;
    }

    return dl->head;
}

DoubleLinkedListNode *ds_doublelinkedlist_rbegin(DS_DoubleLinkedList *dl)
{
    if (dl == NULL || dl->head == NULL)
    {
        return NULL;
    }

    return dl->tail;
}

DoubleLinkedListNode *ds_doublelinkedlist_next(DoubleLinkedListNode *cursor)
{
    if (cursor == NULL)
    {
        return NULL;
    }

    return cursor->next;
}

DoubleLinkedListNode *ds_doublelinkedlist_prev(DoubleLinkedListNode *cursor)
{
    if (cursor == NULL)
    {
        return NULL;
    }

    return cursor->prev;
}

int ds_doublelinkedlist_node_get_data(DoubleLinkedListNode *cursor, DS_DOUBLELINKEDLIST_TYPE **x)
{
    if (cursor == NULL || x == NULL)
    {
        return 0;
    }

    *x = &(cursor->data);

    return 1;
}
