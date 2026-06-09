#ifndef DS_PRIORITYQUEUE_MIN_TYPE_H
#define DS_PRIORITYQUEUE_MIN_TYPE_H

typedef struct
{
    int data;
} ds_priorityqueue_min_type;

// 以下宏接收的参数 a,b,e 类型为 DS_PRIORITYQUEUE_MIN_TYPE

#define DS_PRIORITYQUEUE_MIN_LT(a, b) ((a).data < (b).data)
#define DS_PRIORITYQUEUE_MIN_GT(a, b) ((a).data > (b).data)
#define DS_PRIORITYQUEUE_MIN_EQ(a, b) ((a).data == (b).data)
#define DS_PRIORITYQUEUE_MIN_LE(a, b) ((a).data <= (b).data)
#define DS_PRIORITYQUEUE_MIN_GE(a, b) ((a).data >= (b).data)

#define DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(e) \
    do                                          \
    {                                           \
    } while (0)

#define DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(e, judge) \
    ((ds_priorityqueue_min_type){.data = (e).data})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_priorityqueue_min_type;

// 需要修改 DESTROY_ELEMENT
#define DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(e) \
    do                                          \
    {                                           \
        free((e).name);                         \
        (e).name = NULL;                        \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
#define DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(e, judge)    \
    ({                                                  \
        char *name_copy = NULL;                         \
        if ((e).name != NULL)                           \
        {                                               \
            name_copy = strdup((e).name);               \
            if (name_copy == NULL)                      \
                *(judge) = 0;                           \
        }                                               \
        (ds_priorityqueue_min_type){.data = (e).data,   \
                                    .name = name_copy}; \
    })

// 比较宏不变（按 data 比较），如需按 name 比较则修改对应宏
*/