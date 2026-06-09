#include <stdio.h>
#include "ds_priorityqueue_max.h"
#include "ds_priorityqueue_min.h"

static int failures = 0;

static void check(const char *name, int ok)
{
    if (!ok)
    {
        printf("  FAIL: %s\n", name);
        failures++;
    }
}

/* ── Max‑heap tests ── */
static void test_max_heap(void)
{
    printf("=== Max-Heap Tests ===\n");

    DS_PriorityQueue_max *pq = ds_priorityqueue_max_create();
    check("create", pq != NULL);

    /* size / capacity / is_empty on empty queue */
    check("size 0", ds_priorityqueue_max_size(pq) == 0);
    check("capacity 0", ds_priorityqueue_max_capacity(pq) == 0);
    check("is_empty", ds_priorityqueue_max_is_empty(pq) == 1);

    /* peek / pop on empty queue should fail */
    DS_PRIORITYQUEUE_MAX_TYPE *x;
    check("peek empty", ds_priorityqueue_max_peek(pq, NULL) == 0);
    check("peek empty2", ds_priorityqueue_max_peek(pq, &x) == 0);
    check("pop empty", ds_priorityqueue_max_pop(pq, &x) == 0);

    /* push elements: 3, 7, 1, 9, 5 */
    check("push 3", ds_priorityqueue_max_push(pq, (ds_priorityqueue_max_type){3}) == 1);
    check("push 7", ds_priorityqueue_max_push(pq, (ds_priorityqueue_max_type){7}) == 1);
    check("push 1", ds_priorityqueue_max_push(pq, (ds_priorityqueue_max_type){1}) == 1);
    check("push 9", ds_priorityqueue_max_push(pq, (ds_priorityqueue_max_type){9}) == 1);
    check("push 5", ds_priorityqueue_max_push(pq, (ds_priorityqueue_max_type){5}) == 1);

    check("size 5", ds_priorityqueue_max_size(pq) == 5);
    check("not empty", ds_priorityqueue_max_is_empty(pq) == 0);

    /* peek should return 9 (max) */
    check("peek 9", ds_priorityqueue_max_peek(pq, &x) == 1 && x->data == 9);

    /* pop: 9, 7, 5, 3, 1 */
    check("pop 9", ds_priorityqueue_max_pop(pq, &x) == 1 && x->data == 9);
    check("pop 7", ds_priorityqueue_max_pop(pq, &x) == 1 && x->data == 7);
    check("size 3", ds_priorityqueue_max_size(pq) == 3);

    /* reserve */
    check("reserve 10", ds_priorityqueue_max_reserve(pq, 10) == 1);
    check("capacity 10", ds_priorityqueue_max_capacity(pq) == 10);

    /* pop_and_destroy */
    check("pop_and_destroy", ds_priorityqueue_max_pop_and_destroy(pq) == 1);
    check("size 2", ds_priorityqueue_max_size(pq) == 2);

    /* shrink_to_fit */
    check("shrink_to_fit", ds_priorityqueue_max_shrink_to_fit(pq) == 1);
    check("capacity 2", ds_priorityqueue_max_capacity(pq) == 2);

    /* clone */
    int judge = -1;
    DS_PriorityQueue_max *clone = ds_priorityqueue_max_clone(pq, &judge);
    check("clone ok", clone != NULL && judge == 1);
    check("clone size", ds_priorityqueue_max_size(clone) == 2);
    /* clones should have same elements; peek should give the max (3) */
    DS_PRIORITYQUEUE_MAX_TYPE *cx;
    check("clone peek", ds_priorityqueue_max_peek(clone, &cx) == 1 && cx->data == 3);

    /* clone NULL → returns NULL, judge=1 */
    int judge_null;
    DS_PriorityQueue_max *null_clone = ds_priorityqueue_max_clone(NULL, &judge_null);
    check("clone NULL", null_clone == NULL && judge_null == 1);

    /* clone with judge=NULL → returns NULL */
    check("clone judge NULL", ds_priorityqueue_max_clone(pq, NULL) == NULL);

    /* NULL guard tests */
    check("size NULL", ds_priorityqueue_max_size(NULL) == -1);
    check("capacity NULL", ds_priorityqueue_max_capacity(NULL) == -1);
    check("is_empty NULL", ds_priorityqueue_max_is_empty(NULL) == -1);
    check("peek NULL", ds_priorityqueue_max_peek(NULL, &x) == 0);
    check("push NULL", ds_priorityqueue_max_push(NULL, (ds_priorityqueue_max_type){0}) == 0);
    check("pop NULL", ds_priorityqueue_max_pop(NULL, &x) == 0);
    check("reserve NULL", ds_priorityqueue_max_reserve(NULL, 8) == 0);
    check("shrink NULL", ds_priorityqueue_max_shrink_to_fit(NULL) == 0);

    ds_priorityqueue_max_destroy(clone);
    ds_priorityqueue_max_destroy(pq);
    ds_priorityqueue_max_destroy(NULL); /* should not crash */
}

/* ── Min‑heap tests ── */
static void test_min_heap(void)
{
    printf("\n=== Min-Heap Tests ===\n");

    DS_PriorityQueue_min *pq = ds_priorityqueue_min_create();
    check("create", pq != NULL);

    check("size 0", ds_priorityqueue_min_size(pq) == 0);
    check("is_empty", ds_priorityqueue_min_is_empty(pq) == 1);

    /* push: 5, 3, 8, 1 */
    check("push 5", ds_priorityqueue_min_push(pq, (ds_priorityqueue_min_type){5}) == 1);
    check("push 3", ds_priorityqueue_min_push(pq, (ds_priorityqueue_min_type){3}) == 1);
    check("push 8", ds_priorityqueue_min_push(pq, (ds_priorityqueue_min_type){8}) == 1);
    check("push 1", ds_priorityqueue_min_push(pq, (ds_priorityqueue_min_type){1}) == 1);

    check("size 4", ds_priorityqueue_min_size(pq) == 4);

    DS_PRIORITYQUEUE_MIN_TYPE *x;
    check("peek 1", ds_priorityqueue_min_peek(pq, &x) == 1 && x->data == 1);

    /* pop: 1, 3, 5, 8 */
    check("pop 1", ds_priorityqueue_min_pop(pq, &x) == 1 && x->data == 1);
    check("pop 3", ds_priorityqueue_min_pop(pq, &x) == 1 && x->data == 3);

    /* pop_and_destroy */
    check("pop_and_destroy", ds_priorityqueue_min_pop_and_destroy(pq) == 1);
    check("size 1", ds_priorityqueue_min_size(pq) == 1);

    /* reserve & shrink_to_fit */
    check("reserve 8", ds_priorityqueue_min_reserve(pq, 8) == 1);
    check("capacity 8", ds_priorityqueue_min_capacity(pq) == 8);
    check("shrink_to_fit", ds_priorityqueue_min_shrink_to_fit(pq) == 1);
    check("capacity 1", ds_priorityqueue_min_capacity(pq) == 1);

    /* pop last element, then reserve(0) to free internal array */
    check("pop 8", ds_priorityqueue_min_pop(pq, &x) == 1 && x->data == 8);
    check("size 0", ds_priorityqueue_min_size(pq) == 0);
    check("reserve 0", ds_priorityqueue_min_reserve(pq, 0) == 1);
    check("capacity 0", ds_priorityqueue_min_capacity(pq) == 0);

    /* push again after reserve(0) — tests realloc(NULL, ...) path */
    check("push after 0", ds_priorityqueue_min_push(pq, (ds_priorityqueue_min_type){42}) == 1);
    check("peek 42", ds_priorityqueue_min_peek(pq, &x) == 1 && x->data == 42);

    /* clone */
    int judge = -1;
    DS_PriorityQueue_min *clone = ds_priorityqueue_min_clone(pq, &judge);
    check("clone ok", clone != NULL && judge == 1);
    DS_PRIORITYQUEUE_MIN_TYPE *cx;
    check("clone peek", ds_priorityqueue_min_peek(clone, &cx) == 1 && cx->data == 42);

    /* NULL guards (brief) */
    check("pop NULL", ds_priorityqueue_min_pop(NULL, &x) == 0);
    check("pop_and_destroy NULL", ds_priorityqueue_min_pop_and_destroy(NULL) == 0);
    check("reserve size>cap", ds_priorityqueue_min_reserve(pq, 0) == 0); /* 0 < size(1) */

    ds_priorityqueue_min_destroy(clone);
    ds_priorityqueue_min_destroy(pq);
}

int main(void)
{
    test_max_heap();
    test_min_heap();

    printf("\n%s (%d failure(s))\n", failures ? "SOME TESTS FAILED" : "All tests passed", failures);
    return failures;
}
