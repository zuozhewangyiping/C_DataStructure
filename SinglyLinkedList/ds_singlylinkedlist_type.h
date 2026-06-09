#ifndef DS_SINGLYLINKEDLIST_TYPE_H
#define DS_SINGLYLINKEDLIST_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_singlylinkedlist_type;

// 以下宏接收的参数 e 类型为 DS_SINGLYLINKEDLIST_TYPE

// DESTROY宏用于ds_singlylinkedlist_destroy & XXX_and_destroy & ds_singlylinkedlist_set函数
#define DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(e) \
    do                                         \
    {                                          \
        /* nothing to free */                  \
    } while (0)

// CLONE宏用于ds_singlylinkedlist_clone函数
#define DS_SINGLYLINKEDLIST_CLONE_ELEMENT(e, judge) \
    ((ds_singlylinkedlist_type){.key = (e).key,     \
                                .value = (e).value})

// 两个MATCH宏用于ds_singlylinkedlist_find函数
#define DS_SINGLYLINKEDLIST_MATCH_TYPE int
#define DS_SINGLYLINKEDLIST_MATCH(e, target) ((e).key == target ? 1 : 0)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_singlylinkedlist_type;

// 需要修改 DESTROY_ELEMENT
#define DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(e) \
    do                                         \
    {                                          \
        free((e).name);                        \
        (e).name = NULL;                       \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
#define DS_SINGLYLINKEDLIST_CLONE_ELEMENT(e, judge)    \
    ({                                                 \
        char *name_copy = NULL;                        \
        if ((e).name != NULL)                          \
        {                                              \
            name_copy = strdup((e).name);              \
            if (name_copy == NULL)                     \
                *(judge) = 0;                          \
        }                                              \
        (ds_singlylinkedlist_type){.key = (e).key,     \
                                   .value = (e).value, \
                                   .name = name_copy}; \
    })

// MATCH 宏不变（按 key 匹配），如需按 name 匹配则修改：
// #define DS_SINGLYLINKEDLIST_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/