#include <stdio.h>
#include "ds_stack.h"

int main(void)
{
    DS_Stack *s = ds_stack_create();

    printf("size: %d, capacity: %d, is_empty: %d\n",
           ds_stack_size(s), ds_stack_capacity(s), ds_stack_is_empty(s));

    ds_stack_push(s, (DS_STACK_TYPE){10});
    ds_stack_push(s, (DS_STACK_TYPE){20});
    ds_stack_push(s, (DS_STACK_TYPE){30});
    printf("after 3 pushes: size=%d\n", ds_stack_size(s));

    ds_stack_reserve(s, 8);
    printf("after reserve(8): capacity=%d\n", ds_stack_capacity(s));

    DS_STACK_TYPE *top;
    ds_stack_peek(s, &top);
    printf("peek: %d\n", top->value);

    ds_stack_pop(s, &top);
    printf("pop: %d\n", top->value);
    DS_STACK_DESTROY_ELEMENT(*top);

    ds_stack_pop_and_destroy(s);
    printf("after pop_and_destroy: size=%d\n", ds_stack_size(s));

    ds_stack_shrink_to_fit(s);
    printf("after shrink_to_fit: capacity=%d\n", ds_stack_capacity(s));

    int judge;
    DS_Stack *clone = ds_stack_clone(s, &judge);
    ds_stack_peek(clone, &top);
    printf("clone top: %d\n", top->value);

    ds_stack_destroy(s);
    ds_stack_destroy(clone);
    return 0;
}
