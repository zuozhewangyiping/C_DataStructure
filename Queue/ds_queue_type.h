#ifndef DS_QUEUE_TYPE_H
#define DS_QUEUE_TYPE_H

typedef struct
{
    int data;
} ds_queue_type;

// 以下宏接收的参数 e 类型为 DS_QUEUE_TYPE

#define DS_QUEUE_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
    } while (0)

#define DS_QUEUE_CLONE_ELEMENT(e, judge) \
    ((ds_queue_type){.data = (e).data})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_queue_type;

// 需要修改 DESTROY_ELEMENT
#define DS_QUEUE_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
        free((e).name);             \
        (e).name = NULL;            \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
#define DS_QUEUE_CLONE_ELEMENT(e, judge)    \
    ({                                      \
        char *name_copy = NULL;             \
        if ((e).name != NULL)               \
        {                                   \
            name_copy = strdup((e).name);   \
            if (name_copy == NULL)          \
                *(judge) = 0;               \
        }                                   \
        (ds_queue_type){.data = (e).data,   \
                        .name = name_copy}; \
    })

// MATCH 宏不变（按 data 匹配），如需按 name 匹配则修改：
// #define DS_QUEUE_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/
