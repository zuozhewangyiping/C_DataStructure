#ifndef DS_DYNAMICARRAY_TYPE_H
#define DS_DYNAMICARRAY_TYPE_H

#include "ds_string.h"

typedef struct
{
    DS_String *name;
    int id;
    int score;
} ds_dynamicarray_type;

// 以下宏接收的参数 e 类型为 DS_DYNAMICARRAY_TYPE

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) \
    do                                     \
    {                                      \
        ds_string_destroy((e).name);       \
        (e).name = NULL;                   \
    } while (0)

#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge) \
    ((ds_dynamicarray_type){.score = (e).score, \
                            .id = (e).id,       \
                            .name = ds_string_clone((e).name, judge)})

#define DS_DYNAMICARRAY_MATCH_TYPE int
#define DS_DYNAMICARRAY_MATCH(e, target) ((e).id == target ? 1 : 0)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_dynamicarray_type;

// 需要修改 DESTROY_ELEMENT
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) \
    do                                     \
    {                                      \
        free((e).name);                    \
        (e).name = NULL;                   \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge)    \
    ({                                             \
        char *name_copy = NULL;                    \
        if ((e).name != NULL)                      \
        {                                          \
            name_copy = strdup((e).name);          \
            if (name_copy == NULL)                 \
                *(judge) = 0;                      \
        }                                          \
        (ds_dynamicarray_type){.data = (e).data,   \
                               .name = name_copy}; \
    })

// MATCH 宏不变（按 data 匹配），如需按 name 匹配则修改：
// #define DS_DYNAMICARRAY_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/
