#include <stdio.h>
#include "ds_deque.h"

int main(void)
{
    DS_Deque *dq = ds_deque_create();

    printf("size: %d, capacity: %d, is_empty: %d\n",
           ds_deque_size(dq), ds_deque_capacity(dq), ds_deque_is_empty(dq));

    ds_deque_push_back(dq, (DS_DEQUE_TYPE){10});
    ds_deque_push_front(dq, (DS_DEQUE_TYPE){20});
    ds_deque_push_back(dq, (DS_DEQUE_TYPE){30});
    ds_deque_insert(dq, 1, (DS_DEQUE_TYPE){99});
    printf("after push/insert: size=%d\n", ds_deque_size(dq));

    ds_deque_reserve(dq, 10);
    printf("after reserve(10): capacity=%d\n", ds_deque_capacity(dq));

    DS_DEQUE_TYPE *val;
    ds_deque_get(dq, 0, &val);
    printf("get[0]=%d\n", val->data);

    ds_deque_set(dq, 0, (DS_DEQUE_TYPE){77});
    printf("after set[0]=77\n");

    ds_deque_peek_front(dq, &val);
    printf("peek_front: %d\n", val->data);
    ds_deque_peek_back(dq, &val);
    printf("peek_back: %d\n", val->data);

    ds_deque_find(dq, 99, &val);
    printf("find(99): data=%d\n", val->data);

    ds_deque_pop_front(dq, &val);
    printf("pop_front: %d\n", val->data);
    DS_DEQUE_DESTROY_ELEMENT(*val);

    ds_deque_pop_back(dq, &val);
    printf("pop_back: %d\n", val->data);
    DS_DEQUE_DESTROY_ELEMENT(*val);

    ds_deque_erase(dq, 0, &val);
    printf("erase[0]: %d\n", val->data);
    DS_DEQUE_DESTROY_ELEMENT(*val);

    ds_deque_push_front(dq, (DS_DEQUE_TYPE){1});
    ds_deque_pop_front_and_destroy(dq);
    ds_deque_push_back(dq, (DS_DEQUE_TYPE){2});
    ds_deque_pop_back_and_destroy(dq);
    ds_deque_push_back(dq, (DS_DEQUE_TYPE){3});
    ds_deque_erase_and_destroy(dq, 0);

    ds_deque_shrink_to_fit(dq);
    printf("after shrink_to_fit: capacity=%d\n", ds_deque_capacity(dq));

    int judge;
    DS_Deque *clone = ds_deque_clone(dq, &judge);
    printf("clone: size=%d\n", ds_deque_size(clone));

    ds_deque_destroy(dq);
    ds_deque_destroy(clone);
    return 0;
}
