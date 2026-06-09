#include <stdio.h>
#include <stdlib.h>
#include "ds_singlylinkedlist.h"

/* 遍历回调：打印每个元素 */
static void print_element(DS_SINGLYLINKEDLIST_TYPE *value, void *user_data)
{
    (void)user_data;
    printf("  key=%d, value=%d\n", value->key, value->value);
}

/* 遍历回调：每个元素的 value 加上回调传入的数值 */
static void add_value(DS_SINGLYLINKEDLIST_TYPE *value, void *user_data)
{
    value->value += *(int *)user_data;
}

/* 辅助：打印链表全部内容及大小 */
static void print_list(DS_SinglyLinkedList *sl)
{
    int sz = ds_singlylinkedlist_size(sl);
    printf("size=%d\n", sz);
    if (sz > 0) {
        ds_singlylinkedlist_traverse(sl, NULL, print_element);
    }
    printf("\n");
}

int main(void)
{
    /* ---- 创建链表 ---- */
    DS_SinglyLinkedList *sl = ds_singlylinkedlist_create();
    printf("empty? %d\n\n", ds_singlylinkedlist_is_empty(sl));

    /* ---- push_back / push_front ---- */
    ds_singlylinkedlist_push_back(sl,  (DS_SINGLYLINKEDLIST_TYPE){1, 100});
    ds_singlylinkedlist_push_back(sl,  (DS_SINGLYLINKEDLIST_TYPE){2, 200});
    ds_singlylinkedlist_push_front(sl, (DS_SINGLYLINKEDLIST_TYPE){0, 0});
    printf("after push:\n");
    print_list(sl);    /* 预期: {0,0}, {1,100}, {2,200} */

    /* ---- insert ---- */
    ds_singlylinkedlist_insert(sl, 1, (DS_SINGLYLINKEDLIST_TYPE){99, 999});
    printf("after insert at index 1:\n");
    print_list(sl);    /* 预期: {0,0}, {99,999}, {1,100}, {2,200} */

    /* ---- get（拿到内部指针，可直接修改） ---- */
    DS_SINGLYLINKEDLIST_TYPE *p;
    if (ds_singlylinkedlist_get(sl, 2, &p)) {
        p->key = 777;
        printf("after get+modify index 2:\n");
        print_list(sl);    /* {1,100} 变为 {777,100} */
    }

    /* ---- set（深拷贝替换） ---- */
    ds_singlylinkedlist_set(sl, 0, (DS_SINGLYLINKEDLIST_TYPE){-1, -100});
    printf("after set index 0:\n");
    print_list(sl);

    /* ---- find ---- */
    if (ds_singlylinkedlist_find(sl, 777, &p)) {
        printf("found key=777, value=%d\n\n", p->value);
    }

    /* ---- traverse 批量修改 ---- */
    int delta = 10;
    ds_singlylinkedlist_traverse(sl, &delta, add_value);
    printf("after traverse add 10 to all values:\n");
    print_list(sl);

    /* ---- pop_front / pop_back ---- */
    DS_SINGLYLINKEDLIST_TYPE *popped;
    ds_singlylinkedlist_pop_front(sl, &popped);
    printf("pop front: key=%d, value=%d, then free it\n", popped->key, popped->value);
    free(popped);

    ds_singlylinkedlist_pop_back(sl, &popped);
    printf("pop back : key=%d, value=%d, then free it\n", popped->key, popped->value);
    free(popped);
    printf("after pops:\n");
    print_list(sl);

    /* ---- erase ---- */
    ds_singlylinkedlist_erase(sl, 0, &popped);
    printf("erase index 0: key=%d, value=%d, free it\n", popped->key, popped->value);
    free(popped);
    printf("after erase:\n");
    print_list(sl);

    /* ---- clone ---- */
    int judge;
    DS_SinglyLinkedList *copy = ds_singlylinkedlist_clone(sl, &judge);
    printf("clone (judge=%d):\n", judge);
    print_list(copy);

    /* ---- concat ---- */
    ds_singlylinkedlist_concat(sl, copy);
    printf("after concat (copy -> sl), copy is now empty:\n");
    printf("sl:  "); print_list(sl);
    printf("copy: "); print_list(copy);

    /* ---- _and_destroy 版本（无需手动 free） ---- */
    ds_singlylinkedlist_pop_front_and_destroy(sl);
    printf("after pop_front_and_destroy:\n");
    print_list(sl);

    /* ---- 清理 ---- */
    ds_singlylinkedlist_destroy(sl);
    ds_singlylinkedlist_destroy(copy);
    return 0;
}
