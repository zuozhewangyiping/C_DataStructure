#include <stdio.h>
#include <stdlib.h>
#include "ds_doublelinkedlist.h"

static void print_entry(DS_DOUBLELINKEDLIST_TYPE *v, void *ud)
{
    (void)ud;
    printf("  [%d]=%d\n", v->key, v->value);
}

static void add_ten(DS_DOUBLELINKEDLIST_TYPE *v, void *ud) { (void)ud; v->value += 10; }

int main(void)
{
    DS_DoubleLinkedList *dl = ds_doublelinkedlist_create();

    printf("is_empty: %d, size: %d\n",
           ds_doublelinkedlist_is_empty(dl), ds_doublelinkedlist_size(dl));

    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){1, 100});
    ds_doublelinkedlist_push_front(dl, (DS_DOUBLELINKEDLIST_TYPE){0, 0});
    ds_doublelinkedlist_insert(dl, 1, (DS_DOUBLELINKEDLIST_TYPE){99, 999});
    printf("after push/insert: size=%d\n", ds_doublelinkedlist_size(dl));

    DS_DOUBLELINKEDLIST_TYPE *p;
    ds_doublelinkedlist_get(dl, 1, &p);
    printf("get[1]: key=%d, value=%d\n", p->key, p->value);

    ds_doublelinkedlist_set(dl, 0, (DS_DOUBLELINKEDLIST_TYPE){-1, -100});

    ds_doublelinkedlist_find(dl, 1, &p);
    printf("find(1): key=%d, value=%d\n", p->key, p->value);

    DoubleLinkedListNode *cur = ds_doublelinkedlist_search(dl, 99);
    ds_doublelinkedlist_node_get_data(cur, &p);
    printf("search(99): key=%d, value=%d\n", p->key, p->value);

    printf("traverse:\n");
    ds_doublelinkedlist_traverse(dl, NULL, print_entry);

    int delta = 10;
    ds_doublelinkedlist_traverse(dl, &delta, add_ten);
    printf("after traverse(add 10):\n");
    ds_doublelinkedlist_traverse(dl, NULL, print_entry);

    printf("rtraverse:\n");
    ds_doublelinkedlist_rtraverse(dl, NULL, print_entry);

    cur = ds_doublelinkedlist_begin(dl);
    ds_doublelinkedlist_node_get_data(cur, &p);
    printf("begin: key=%d\n", p->key);
    cur = ds_doublelinkedlist_rbegin(dl);
    ds_doublelinkedlist_node_get_data(cur, &p);
    printf("rbegin: key=%d\n", p->key);

    cur = ds_doublelinkedlist_next(ds_doublelinkedlist_begin(dl));
    ds_doublelinkedlist_node_get_data(cur, &p);
    printf("next(after begin): key=%d\n", p->key);

    cur = ds_doublelinkedlist_prev(ds_doublelinkedlist_rbegin(dl));
    ds_doublelinkedlist_node_get_data(cur, &p);
    printf("prev(before rbegin): key=%d\n", p->key);

    ds_doublelinkedlist_insert_after_cursor(dl, ds_doublelinkedlist_begin(dl),
                                            (DS_DOUBLELINKEDLIST_TYPE){50, 500});
    ds_doublelinkedlist_insert_before_cursor(dl, ds_doublelinkedlist_rbegin(dl),
                                             (DS_DOUBLELINKEDLIST_TYPE){250, 2500});
    printf("after cursor inserts: size=%d\n", ds_doublelinkedlist_size(dl));

    ds_doublelinkedlist_erase_after_cursor_and_destroy(dl, ds_doublelinkedlist_begin(dl));
    ds_doublelinkedlist_erase_before_cursor_and_destroy(dl, ds_doublelinkedlist_rbegin(dl));

    ds_doublelinkedlist_erase_after_cursor(dl, ds_doublelinkedlist_begin(dl), &p);
    printf("erase_after_cursor: key=%d\n", p->key);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_doublelinkedlist_erase_before_cursor(dl, ds_doublelinkedlist_rbegin(dl), &p);
    printf("erase_before_cursor: key=%d\n", p->key);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    cur = ds_doublelinkedlist_search(dl, 1);
    ds_doublelinkedlist_erase_cursor(dl, cur, &p);
    printf("erase_cursor: key=%d, value=%d\n", p->key, p->value);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){99, 99});
    cur = ds_doublelinkedlist_search(dl, 99);
    ds_doublelinkedlist_erase_cursor_and_destroy(dl, cur);
    printf("after erase_cursor_and_destroy: size=%d\n", ds_doublelinkedlist_size(dl));

    ds_doublelinkedlist_pop_front(dl, &p);
    printf("pop_front: key=%d, value=%d\n", p->key, p->value);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_doublelinkedlist_pop_back(dl, &p);
    printf("pop_back: key=%d, value=%d\n", p->key, p->value);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_doublelinkedlist_erase(dl, 0, &p);
    printf("erase[0]: key=%d, value=%d\n", p->key, p->value);
    DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(*p);
    free(p);

    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){7, 700});
    ds_doublelinkedlist_pop_front_and_destroy(dl);
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){8, 800});
    ds_doublelinkedlist_pop_back_and_destroy(dl);
    ds_doublelinkedlist_push_back(dl, (DS_DOUBLELINKEDLIST_TYPE){9, 900});
    ds_doublelinkedlist_erase_and_destroy(dl, 0);

    int judge;
    DS_DoubleLinkedList *clone = ds_doublelinkedlist_clone(dl, &judge);
    printf("clone: size=%d\n", ds_doublelinkedlist_size(clone));

    DS_DoubleLinkedList *dl2 = ds_doublelinkedlist_create();
    ds_doublelinkedlist_push_back(dl2, (DS_DOUBLELINKEDLIST_TYPE){100, 1000});
    ds_doublelinkedlist_concat(dl, dl2);
    printf("after concat: dl size=%d, dl2 is_empty=%d\n",
           ds_doublelinkedlist_size(dl), ds_doublelinkedlist_is_empty(dl2));

    ds_doublelinkedlist_destroy(dl);
    ds_doublelinkedlist_destroy(dl2);
    ds_doublelinkedlist_destroy(clone);
    return 0;
}
