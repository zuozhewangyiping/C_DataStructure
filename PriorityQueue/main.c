#include <stdio.h>
#include "ds_priorityqueue_max.h"
#include "ds_priorityqueue_min.h"

int main(void)
{
    /* max-heap */
    printf("=== Max-Heap ===\n");
    DS_PriorityQueue_max *pq_max = ds_priorityqueue_max_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_priorityqueue_max_is_empty(pq_max), ds_priorityqueue_max_size(pq_max),
           ds_priorityqueue_max_capacity(pq_max));

    ds_priorityqueue_max_push(pq_max, (ds_priorityqueue_max_type){3});
    ds_priorityqueue_max_push(pq_max, (ds_priorityqueue_max_type){9});
    ds_priorityqueue_max_push(pq_max, (ds_priorityqueue_max_type){1});
    ds_priorityqueue_max_push(pq_max, (ds_priorityqueue_max_type){7});
    printf("after 4 pushes: size=%d\n", ds_priorityqueue_max_size(pq_max));

    DS_PRIORITYQUEUE_MAX_TYPE *top;
    ds_priorityqueue_max_peek(pq_max, &top);
    printf("peek: %d\n", top->data);

    ds_priorityqueue_max_reserve(pq_max, 10);
    printf("after reserve(10): capacity=%d\n", ds_priorityqueue_max_capacity(pq_max));

    ds_priorityqueue_max_pop(pq_max, &top);
    printf("pop: %d\n", top->data);
    DS_PRIORITYQUEUE_MAX_DESTROY_ELEMENT(*top);

    ds_priorityqueue_max_pop_and_destroy(pq_max);
    printf("after pop_and_destroy: size=%d\n", ds_priorityqueue_max_size(pq_max));

    ds_priorityqueue_max_shrink_to_fit(pq_max);
    printf("after shrink_to_fit: capacity=%d\n", ds_priorityqueue_max_capacity(pq_max));

    int judge;
    DS_PriorityQueue_max *clone_max = ds_priorityqueue_max_clone(pq_max, &judge);
    ds_priorityqueue_max_peek(clone_max, &top);
    printf("clone top: %d\n", top->data);

    ds_priorityqueue_max_destroy(pq_max);
    ds_priorityqueue_max_destroy(clone_max);

    /* min-heap */
    printf("\n=== Min-Heap ===\n");
    DS_PriorityQueue_min *pq_min = ds_priorityqueue_min_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_priorityqueue_min_is_empty(pq_min), ds_priorityqueue_min_size(pq_min),
           ds_priorityqueue_min_capacity(pq_min));

    ds_priorityqueue_min_push(pq_min, (ds_priorityqueue_min_type){5});
    ds_priorityqueue_min_push(pq_min, (ds_priorityqueue_min_type){1});
    ds_priorityqueue_min_push(pq_min, (ds_priorityqueue_min_type){8});
    ds_priorityqueue_min_push(pq_min, (ds_priorityqueue_min_type){3});
    printf("after 4 pushes: size=%d\n", ds_priorityqueue_min_size(pq_min));

    DS_PRIORITYQUEUE_MIN_TYPE *top_min;
    ds_priorityqueue_min_peek(pq_min, &top_min);
    printf("peek: %d\n", top_min->data);

    ds_priorityqueue_min_reserve(pq_min, 10);
    printf("after reserve(10): capacity=%d\n", ds_priorityqueue_min_capacity(pq_min));

    ds_priorityqueue_min_pop(pq_min, &top_min);
    printf("pop: %d\n", top_min->data);
    DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(*top_min);

    ds_priorityqueue_min_pop_and_destroy(pq_min);
    printf("after pop_and_destroy: size=%d\n", ds_priorityqueue_min_size(pq_min));

    ds_priorityqueue_min_shrink_to_fit(pq_min);
    printf("after shrink_to_fit: capacity=%d\n", ds_priorityqueue_min_capacity(pq_min));

    DS_PriorityQueue_min *clone_min = ds_priorityqueue_min_clone(pq_min, &judge);
    ds_priorityqueue_min_peek(clone_min, &top_min);
    printf("clone top: %d\n", top_min->data);

    ds_priorityqueue_min_destroy(pq_min);
    ds_priorityqueue_min_destroy(clone_min);

    return 0;
}
