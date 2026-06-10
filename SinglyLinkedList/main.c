#include <stdio.h>
#include <stdlib.h>
#include "ds_singlylinkedlist.h"

static void print_entry(DS_SINGLYLINKEDLIST_TYPE *v, void *ud)
{
    (void)ud;
    printf("  [%d]=%d\n", v->key, v->value);
}

static void add_ten(DS_SINGLYLINKEDLIST_TYPE *v, void *ud) { (void)ud; v->value += 10; }

int main(void)
{
    DS_SinglyLinkedList *sl = ds_singlylinkedlist_create();

    printf("is_empty: %d, size: %d\n", ds_singlylinkedlist_is_empty(sl), ds_singlylinkedlist_size(sl));

    ds_singlylinkedlist_push_back(sl, (DS_SINGLYLINKEDLIST_TYPE){1, 100});
    ds_singlylinkedlist_push_front(sl, (DS_SINGLYLINKEDLIST_TYPE){0, 0});
    ds_singlylinkedlist_insert(sl, 1, (DS_SINGLYLINKEDLIST_TYPE){99, 999});
    printf("after push/insert: size=%d\n", ds_singlylinkedlist_size(sl));

    DS_SINGLYLINKEDLIST_TYPE *p;
    ds_singlylinkedlist_get(sl, 1, &p);
    printf("get[1]: key=%d, value=%d\n", p->key, p->value);

    ds_singlylinkedlist_set(sl, 0, (DS_SINGLYLINKEDLIST_TYPE){-1, -100});

    ds_singlylinkedlist_find(sl, 1, &p);
    printf("find(1): key=%d, value=%d\n", p->key, p->value);

    SinglyLinkedListNode *cur = ds_singlylinkedlist_search(sl, 99);
    ds_singlylinkedlist_node_get_data(cur, &p);
    printf("search(99): key=%d, value=%d\n", p->key, p->value);

    printf("traverse:\n");
    ds_singlylinkedlist_traverse(sl, NULL, print_entry);

    int delta = 10;
    ds_singlylinkedlist_traverse(sl, &delta, add_ten);
    printf("after traverse(add 10):\n");
    ds_singlylinkedlist_traverse(sl, NULL, print_entry);

    cur = ds_singlylinkedlist_begin(sl);
    printf("begin->key=%d\n", (ds_singlylinkedlist_node_get_data(cur, &p), p->key));

    ds_singlylinkedlist_insert_after_cursor(cur, (DS_SINGLYLINKEDLIST_TYPE){50, 500});
    cur = ds_singlylinkedlist_next(cur);
    ds_singlylinkedlist_node_get_data(cur, &p);
    printf("after insert_after_cursor: key=%d\n", p->key);

    ds_singlylinkedlist_erase_after_cursor_and_destroy(cur);
    printf("after erase_after_cursor_and_destroy\n");

    ds_singlylinkedlist_erase_after_cursor(ds_singlylinkedlist_begin(sl), &p);
    printf("erase_after_cursor: key=%d, value=%d\n", p->key, p->value);
    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_singlylinkedlist_pop_front(sl, &p);
    printf("pop_front: key=%d, value=%d\n", p->key, p->value);
    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_singlylinkedlist_pop_back(sl, &p);
    printf("pop_back: key=%d, value=%d\n", p->key, p->value);
    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_singlylinkedlist_erase(sl, 0, &p);
    printf("erase[0]: key=%d, value=%d\n", p->key, p->value);
    DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_singlylinkedlist_push_back(sl, (DS_SINGLYLINKEDLIST_TYPE){7, 700});
    ds_singlylinkedlist_pop_front_and_destroy(sl);
    ds_singlylinkedlist_push_back(sl, (DS_SINGLYLINKEDLIST_TYPE){8, 800});
    ds_singlylinkedlist_pop_back_and_destroy(sl);
    ds_singlylinkedlist_push_back(sl, (DS_SINGLYLINKEDLIST_TYPE){9, 900});
    ds_singlylinkedlist_erase_and_destroy(sl, 0);

    int judge;
    DS_SinglyLinkedList *clone = ds_singlylinkedlist_clone(sl, &judge);
    printf("clone: size=%d\n", ds_singlylinkedlist_size(clone));

    DS_SinglyLinkedList *sl2 = ds_singlylinkedlist_create();
    ds_singlylinkedlist_push_back(sl2, (DS_SINGLYLINKEDLIST_TYPE){100, 1000});
    ds_singlylinkedlist_concat(sl, sl2);
    printf("after concat: sl size=%d, sl2 is_empty=%d\n",
           ds_singlylinkedlist_size(sl), ds_singlylinkedlist_is_empty(sl2));

    ds_singlylinkedlist_destroy(sl);
    ds_singlylinkedlist_destroy(sl2);
    ds_singlylinkedlist_destroy(clone);
    return 0;
}
