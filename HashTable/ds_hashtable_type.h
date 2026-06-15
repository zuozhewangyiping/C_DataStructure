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
 * 关于 HASH 宏
 * ------------
 * 默认的 HASH 宏使用 FNV-1a 算法，按 int 类型的字节计算哈希值。
 * 如果你的键类型是 int / long 等整数，直接用即可。
 *
 * 但如果你把键类型改成了别的，就要注意：
 *
 *   1. 字符串键（char *）
 *      默认宏是对指针地址算哈希，而不是对字符串内容。
 *      两个内容相同的 "abc" 会得到不同的 hash，查找会失败。
 *      此时必须重写 HASH 宏，改为对字符串内容（strlen + 逐字节）计算。
 *
 *   2. 浮点数键（float / double）
 *      -0.0 和 0.0 在数值上相等，但二进制不同，会得到不同的 hash。
 *      如果这对你来说是 bug，需要特殊处理。
 *
 *   3. 自定义结构体键
 *      如果结构体里只有你要用来比较的字段，且没有指针，一般直接能用。
 *      如果有指针、或者包含 float，参考上面两条。
 *
 * 无论怎么改，确保：两个键在 MATCH / MATCH_KEY 下相等时，HASH 结果必须相同。
 */

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
