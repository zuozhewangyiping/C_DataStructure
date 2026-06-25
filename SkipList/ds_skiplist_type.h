#ifndef DS_SKIPLIST_TYPE_H
#define DS_SKIPLIST_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_skiplist_type;

// 以下宏接收的参数 e 类型为 DS_SKIPLIST_TYPE

#define DS_SKIPLIST_LT(a, b) ((a).key < (b).key)
#define DS_SKIPLIST_GT(a, b) ((a).key > (b).key)
#define DS_SKIPLIST_EQ(a, b) ((a).key == (b).key)
#define DS_SKIPLIST_LE(a, b) ((a).key <= (b).key)
#define DS_SKIPLIST_GE(a, b) ((a).key >= (b).key)

#define DS_SKIPLIST_DESTROY_ELEMENT(e) \
    do                                 \
    {                                  \
    } while (0)

#define DS_SKIPLIST_CLONE_ELEMENT(e, judge) \
    ((ds_skiplist_type){.key = (e).key,     \
                        .value = (e).value})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_skiplist_type;

// 需要修改 DESTROY_ELEMENT
#define DS_SKIPLIST_DESTROY_ELEMENT(e) \
    do                                \
    {                                 \
        free((e).name);               \
        (e).name = NULL;              \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_skiplist_type clone_element(const ds_skiplist_type *src, int *judge)
{
    ds_skiplist_type copy = {.key = src->key, .value = src->value, .name = NULL};
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
#define DS_SKIPLIST_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 比较宏不变（按 key 比较）
*/
