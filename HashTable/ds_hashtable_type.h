#ifndef DS_HASHTABLE_TYPE_H
#define DS_HASHTABLE_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_hashtable_type;

// 以下宏接收的参数 e 类型为 DS_HASHTABLE_TYPE

#define DS_HASHTABLE_DESTROY_ELEMENT(e) \
    do                                  \
    {                                   \
        /* nothing to free */           \
    } while (0)

#define DS_HASHTABLE_CLONE_ELEMENT(e, judge) \
    ((ds_hashtable_type){.key = (e).key,     \
                         .value = (e).value})

#define DS_HASHTABLE_MATCH_TYPE int
#define DS_HASHTABLE_MATCH(e, target) ((e).key == target ? 1 : 0)
#define DS_HASHTABLE_MATCH_KEY(a, b) ((a).key == (b).key ? 1 : 0)

// HASH宏用于哈希运算
#define DS_HASHTABLE_HASH(e, table_capacity)                         \
    ({                                                               \
        unsigned int hash = 2166136261u;                             \
        unsigned char *bytes = (unsigned char *)&((e).key);          \
        for (size_t i = 0; i < sizeof(DS_HASHTABLE_MATCH_TYPE); i++) \
        {                                                            \
            hash ^= bytes[i];                                        \
            hash *= 16777619u;                                       \
        }                                                            \
        (int)(hash % (table_capacity));                              \
    })

#define DS_HASHTABLE_HASH_KEY(key, table_capacity)                   \
    ({                                                               \
        unsigned int hash = 2166136261u;                             \
        unsigned char *bytes = (unsigned char *)&(key);              \
        for (size_t i = 0; i < sizeof(DS_HASHTABLE_MATCH_TYPE); i++) \
        {                                                            \
            hash ^= bytes[i];                                        \
            hash *= 16777619u;                                       \
        }                                                            \
        (int)(hash % (table_capacity));                              \
    })

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_hashtable_type;

// 需要修改 DESTROY_ELEMENT
#define DS_HASHTABLE_DESTROY_ELEMENT(e) \
    do                                         \
    {                                          \
        free((e).name);                        \
        (e).name = NULL;                       \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
#define DS_HASHTABLE_CLONE_ELEMENT(e, judge)    \
    ({                                                 \
        char *name_copy = NULL;                        \
        if ((e).name != NULL)                          \
        {                                              \
            name_copy = strdup((e).name);              \
            if (name_copy == NULL)                     \
                *(judge) = 0;                          \
        }                                              \
        (ds_hashtable_type){.key = (e).key,     \
                                   .value = (e).value, \
                                   .name = name_copy}; \
    })

// MATCH 宏不变（按 key 匹配），如需按 name 匹配则修改：
// #define DS_HASHTABLE_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/
