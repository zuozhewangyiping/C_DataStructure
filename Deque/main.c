#include <stdio.h>
#include "ds_deque.h"

int main(void)
{
    DS_Deque *dq = ds_deque_create();

    ds_deque_push_back(dq, (DS_DEQUE_TYPE){10});
    ds_deque_push_front(dq, (DS_DEQUE_TYPE){20});
    ds_deque_push_back(dq, (DS_DEQUE_TYPE){30});

    DS_DEQUE_TYPE *val;
    while (ds_deque_pop_back(dq, &val))
    {
        printf("pop_back: %d\n", val->data);

        DS_DEQUE_DESTROY_ELEMENT(*val);
    }

    ds_deque_destroy(dq);
    return 0;
}
