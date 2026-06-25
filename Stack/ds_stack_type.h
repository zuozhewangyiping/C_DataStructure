#ifndef DS_STACK_TYPE_H
#define DS_STACK_TYPE_H

typedef struct
{
    int value;
} ds_stack_type;

#define DS_STACK_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
    } while (0)

#define DS_STACK_CLONE_ELEMENT(e, judge) \
    ((ds_stack_type){.value = (e).value})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int value;
    char *name;
} ds_stack_type;

// 需要修改 DESTROY_ELEMENT
#define DS_STACK_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
        free((e).name);             \
        (e).name = NULL;            \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_stack_type clone_element(const ds_stack_type *src, int *judge)
{
    ds_stack_type copy = {.value = src->value, .name = NULL};
    if (src->name != NULL)
    {
        copy.name = strdup(src->name);
        if (copy.name == NULL)
        {
            *judge = 0;
        }
    }
    return copy;
}
#define DS_STACK_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

*/
