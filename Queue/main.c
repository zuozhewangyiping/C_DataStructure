#include <stdio.h>
#include "ds_queue.h"

int main(void)
{
    DS_Queue *q = ds_queue_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_queue_is_empty(q), ds_queue_size(q), ds_queue_capacity(q));

    ds_queue_enqueue(q, (DS_QUEUE_TYPE){10});
    ds_queue_enqueue(q, (DS_QUEUE_TYPE){20});
    ds_queue_enqueue(q, (DS_QUEUE_TYPE){30});
    printf("after 3 enqueues: size=%d\n", ds_queue_size(q));

    ds_queue_reserve(q, 10);
    printf("after reserve(10): capacity=%d\n", ds_queue_capacity(q));

    DS_QUEUE_TYPE *val;
    ds_queue_peek_front(q, &val);
    printf("peek_front: %d\n", val->data);

    ds_queue_peek_back(q, &val);
    printf("peek_back: %d\n", val->data);

    ds_queue_dequeue(q, &val);
    printf("dequeue: %d\n", val->data);
    DS_QUEUE_DESTROY_ELEMENT(*val);

    printf("after dequeue: size=%d\n", ds_queue_size(q));

    ds_queue_enqueue(q, (DS_QUEUE_TYPE){40});
    ds_queue_dequeue_and_destroy(q);
    printf("after dequeue_and_destroy: size=%d\n", ds_queue_size(q));

    ds_queue_shrink_to_fit(q);
    printf("after shrink_to_fit: capacity=%d\n", ds_queue_capacity(q));

    int judge;
    DS_Queue *clone = ds_queue_clone(q, &judge);
    ds_queue_peek_front(clone, &val);
    printf("clone front: %d\n", val->data);

    ds_queue_destroy(q);
    ds_queue_destroy(clone);
    return 0;
}
