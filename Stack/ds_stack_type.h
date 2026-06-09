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
#define DS_STACK_CLONE_ELEMENT(e, judge)    \
    ({                                      \
        char *name_copy = NULL;             \
        if ((e).name != NULL)               \
        {                                   \
            name_copy = strdup((e).name);   \
            if (name_copy == NULL)          \
                *(judge) = 0;               \
        }                                   \
        (ds_stack_type){.value = (e).value, \
                        .name = name_copy}; \
    })

*/
