#include <stdio.h>
#include "ds_stack.h"

int main(void)
{
    DS_Stack *s = ds_stack_create();

    ds_stack_push(s, (DS_STACK_TYPE){10});
    ds_stack_push(s, (DS_STACK_TYPE){20});
    ds_stack_push(s, (DS_STACK_TYPE){30});

    DS_STACK_TYPE *data;
    while (ds_stack_pop(s, &data))
        printf("pop: %d\n", data->value);

    ds_stack_destroy(s);
    return 0;
}
