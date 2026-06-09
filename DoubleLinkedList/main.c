#include <stdio.h>
#include <stdlib.h>
#include "ds_doublelinkedlist.h"

/* 遍历回调：打印每个元素 */
static void print_element(DS_DOUBLELINKEDLIST_TYPE *value, void *user_data)
{
    (void)user_data;
    printf("  key=%d, value=%d\n", value->key, value->value);
}

/* 遍历回调：每个元素的 value 加上传入的数值 */
static void add_value(DS_DOUBLELINKEDLIST_TYPE *value, void *user_data)
{
    value->value += *(int *)user_data;
}

/* 辅助：打印链表正序 + 逆序内容及大小 */
static void print_list(DS_DoubleLinkedList *dl)
{
    int sz = ds_doublelinkedlist_size(dl);
    printf("  size=%d\n", sz);
    if (sz > 0)
    {
        printf("  正序:");
        DoubleLinkedListNode *cur = ds_doublelinkedlist_begin(dl);
        while (cur != NULL)
        {
            DS_DOUBLELINKEDLIST_TYPE *data;
            ds_doublelinkedlist_node_get_data(cur, &data);
            printf(" [k=%d,v=%d]", data->key, data->value);
            cur = ds_doublelinkedlist_next(cur);
        }
        printf("\n  逆序:");
        cur = ds_doublelinkedlist_rbegin(dl);
        while (cur != NULL)
        {
            DS_DOUBLELINKEDLIST_TYPE *data;
            ds_doublelinkedlist_node_get_data(cur, &data);
            printf(" [k=%d,v=%d]", data->key, data->value);
            cur = ds_doublelinkedlist_prev(cur);
        }
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    /* ========================== 1. create & is_empty ========================== */
    printf("===== create & is_empty =====\n");
    DS_DoubleLinkedList *dl = ds_doublelinkedlist_create();
    printf("is_empty? %d (expect 1)\n\n", ds_doublelinkedlist_is_empty(dl));

    /* ======================== 2. push_back / push_front ======================= */
    printf("===== push_back / push_front =====\n");
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){1, 100});
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){2, 200});
    ds_doublelinkedlist_push_front(dl, (DS_DOUBLELINKEDLIST_TYPE){0, 0});
    print_list(dl); /* 预期: {0,0}, {1,100}, {2,200} */

    /* ============================ 3. insert ============================= */
    printf("===== insert =====\n");
    ds_doublelinkedlist_insert(dl, 1, (DS_DOUBLELINKEDLIST_TYPE){99, 999});
    printf("after insert at index 1:\n");
    print_list(dl); /* 预期: {0,0}, {99,999}, {1,100}, {2,200} */

    /* 尾部插入 */
    ds_doublelinkedlist_insert(dl, 4, (DS_DOUBLELINKEDLIST_TYPE){3, 300});
    printf("after insert at tail (index 4):\n");
    print_list(dl);

    /* ========================= 4. get（拿到内部指针，可直接修改） ========================= */
    printf("===== get =====\n");
    DS_DOUBLELINKEDLIST_TYPE *p;
    if (ds_doublelinkedlist_get(dl, 2, &p))
    {
        printf("get index 2: key=%d, value=%d\n", p->key, p->value);
        p->key = 777;
        printf("after in-place modify:\n");
        print_list(dl);
    }

    /* ============================ 5. set（深拷贝替换） ============================= */
    printf("===== set =====\n");
    ds_doublelinkedlist_set(dl, 0, (DS_DOUBLELINKEDLIST_TYPE){-1, -100});
    printf("after set index 0:\n");
    print_list(dl);

    /* ============================ 6. find ============================= */
    printf("===== find =====\n");
    if (ds_doublelinkedlist_find(dl, 777, &p))
    {
        printf("found key=777, value=%d\n\n", p->value);
    }

    /* ============================ 7. search ============================ */
    printf("===== search =====\n");
    DoubleLinkedListNode *cursor = ds_doublelinkedlist_search(dl, 99);
    if (cursor != NULL)
    {
        DS_DOUBLELINKEDLIST_TYPE *data;
        ds_doublelinkedlist_node_get_data(cursor, &data);
        printf("search key=99: found, value=%d\n\n", data->value);
    }

    /* ==================== 8. traverse / rtraverse 批量修改 ===================== */
    printf("===== traverse / rtraverse =====\n");
    int delta = 10;
    ds_doublelinkedlist_traverse(dl, &delta, add_value);
    printf("after traverse add 10 to all values:\n");
    print_list(dl);

    int delta2 = -10;
    ds_doublelinkedlist_rtraverse(dl, &delta2, add_value);
    printf("after rtraverse subtract 10 from all values:\n");
    print_list(dl);

    /* ================= 9. pop_front / pop_back ================== */
    printf("===== pop_front / pop_back =====\n");
    DS_DOUBLELINKEDLIST_TYPE *popped;

    ds_doublelinkedlist_pop_front(dl, &popped);
    printf("pop_front: key=%d, value=%d\n", popped->key, popped->value);
    free(popped);

    ds_doublelinkedlist_pop_back(dl, &popped);
    printf("pop_back : key=%d, value=%d\n", popped->key, popped->value);
    free(popped);
    printf("after pops:\n");
    print_list(dl);

    /* ============================ 10. erase ============================ */
    printf("===== erase =====\n");
    ds_doublelinkedlist_erase(dl, 0, &popped);
    printf("erase index 0: key=%d, value=%d\n", popped->key, popped->value);
    free(popped);
    printf("after erase:\n");
    print_list(dl);

    /* ============================ 11. clone ============================ */
    printf("===== clone =====\n");
    int judge;
    DS_DoubleLinkedList *copy = ds_doublelinkedlist_clone(dl, &judge);
    printf("clone judge=%d:\n", judge);
    print_list(copy);
    /* 验证独立性：修改 copy 不影响原链表 */
    ds_doublelinkedlist_set(copy, 0, (DS_DOUBLELINKEDLIST_TYPE){888, 888});
    printf("after modify copy, original dl unchanged:\n");
    printf("dl:   ");
    print_list(dl);
    printf("copy: ");
    print_list(copy);

    /* ============================ 12. concat ============================ */
    printf("===== concat =====\n");
    DS_DoubleLinkedList *dl2 = ds_doublelinkedlist_create();
    ds_doublelinkedlist_push_back(dl2, (DS_DOUBLELINKEDLIST_TYPE){100, 1000});
    ds_doublelinkedlist_push_back(dl2, (DS_DOUBLELINKEDLIST_TYPE){200, 2000});
    printf("dl2 before concat:\n");
    print_list(dl2);

    ds_doublelinkedlist_concat(dl, dl2);
    printf("after concat dl2 -> dl:\n");
    printf("dl:  ");
    print_list(dl);
    printf("dl2: ");
    print_list(dl2); /* dl2 应为空 */

    /* ==================== 13. cursor insert / erase 游标操作 ==================== */
    printf("===== cursor insert / erase =====\n");

    /* insert_after_cursor */
    cursor = ds_doublelinkedlist_begin(dl);
    ds_doublelinkedlist_insert_after_cursor(dl, cursor, (DS_DOUBLELINKEDLIST_TYPE){50, 500});
    printf("after insert_after_cursor (after head):\n");
    print_list(dl);

    /* insert_before_cursor */
    cursor = ds_doublelinkedlist_rbegin(dl);
    ds_doublelinkedlist_insert_before_cursor(dl, cursor, (DS_DOUBLELINKEDLIST_TYPE){250, 2500});
    printf("after insert_before_cursor (before tail):\n");
    print_list(dl);

    /* erase_after_cursor */
    cursor = ds_doublelinkedlist_begin(dl);
    ds_doublelinkedlist_erase_after_cursor(dl, cursor, &popped);
    printf("erase_after_cursor (after head): removed key=%d, value=%d\n", popped->key, popped->value);
    free(popped);
    print_list(dl);

    /* erase_before_cursor */
    cursor = ds_doublelinkedlist_rbegin(dl);
    ds_doublelinkedlist_erase_before_cursor(dl, cursor, &popped);
    printf("erase_before_cursor (before tail): removed key=%d, value=%d\n", popped->key, popped->value);
    free(popped);
    print_list(dl);

    /* erase_cursor */
    cursor = ds_doublelinkedlist_search(dl, 100);
    if (cursor != NULL)
    {
        ds_doublelinkedlist_erase_cursor(dl, cursor, &popped);
        printf("erase_cursor: removed key=%d, value=%d\n", popped->key, popped->value);
        free(popped);
        print_list(dl);
    }

    /* ===================== 14. _and_destroy 变体 ===================== */
    printf("===== _and_destroy variants =====\n");
    ds_doublelinkedlist_pop_front_and_destroy(dl);
    printf("after pop_front_and_destroy:\n");
    print_list(dl);

    ds_doublelinkedlist_pop_back_and_destroy(dl);
    printf("after pop_back_and_destroy:\n");
    print_list(dl);

    /* 重建数据测试 erase_cursor_and_destroy */
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){1, 1});
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){2, 2});
    cursor = ds_doublelinkedlist_begin(dl);
    ds_doublelinkedlist_erase_cursor_and_destroy(dl, cursor);
    printf("after erase_cursor_and_destroy:\n");
    print_list(dl);

    ds_doublelinkedlist_erase_and_destroy(dl, 0);
    printf("after erase_and_destroy:\n");
    print_list(dl);

    /* ================== 15. 边界：空链表 ================== */
    printf("===== edge cases (empty list) =====\n");
    DS_DoubleLinkedList *empty = ds_doublelinkedlist_create();
    printf("empty size  = %d (expect 0)\n", ds_doublelinkedlist_size(empty));
    printf("empty is_empty = %d (expect 1)\n", ds_doublelinkedlist_is_empty(empty));
    printf("pop_front_and_destroy on empty: %d (expect 0)\n", ds_doublelinkedlist_pop_front_and_destroy(empty));
    printf("pop_back_and_destroy  on empty: %d (expect 0)\n", ds_doublelinkedlist_pop_back_and_destroy(empty));
    printf("erase_and_destroy on empty: %d (expect 0)\n", ds_doublelinkedlist_erase_and_destroy(empty, 0));
    printf("get on empty: %d (expect 0)\n", ds_doublelinkedlist_get(empty, 0, &p));
    printf("set on empty: %d (expect 0)\n", ds_doublelinkedlist_set(empty, 0, (DS_DOUBLELINKEDLIST_TYPE){0, 0}));
    printf("find on empty: %d (expect 0)\n", ds_doublelinkedlist_find(empty, 1, &p));
    printf("search on empty: %p (expect nil)\n", (void *)ds_doublelinkedlist_search(empty, 1));
    printf("begin on empty: %p (expect nil)\n", (void *)ds_doublelinkedlist_begin(empty));
    printf("rbegin on empty: %p (expect nil)\n", (void *)ds_doublelinkedlist_rbegin(empty));
    ds_doublelinkedlist_destroy(empty);

    /* ========================= 16. 清理 ========================== */
    printf("===== cleanup =====\n");
    ds_doublelinkedlist_destroy(dl);
    ds_doublelinkedlist_destroy(dl2);
    ds_doublelinkedlist_destroy(copy);
    printf("all destroyed.\n");

    return 0;
}
