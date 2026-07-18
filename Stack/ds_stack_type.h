#ifndef DS_STACK_TYPE_H
#define DS_STACK_TYPE_H

typedef struct
{
    int value;
} ds_stack_type;

// 以下宏接收的参数 e 类型为 DS_STACK_TYPE

static inline void destroy_element(const ds_stack_type *e)
{
    (void)e;
}
#define DS_STACK_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_stack_type clone_element(const ds_stack_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_STACK_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int value;
    char *name;
} ds_stack_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(const ds_stack_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_STACK_DESTROY_ELEMENT(e) destroy_element(&(e))

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
