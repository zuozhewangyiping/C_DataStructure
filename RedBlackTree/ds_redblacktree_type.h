#ifndef DS_REDBLACKTREE_TYPE_H
#define DS_REDBLACKTREE_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_redblacktree_type;

// 以下宏接收的参数 e 类型为 DS_REDBLACKTREE_TYPE

#define DS_REDBLACKTREE_LT(a, b) ((a).key < (b).key)
#define DS_REDBLACKTREE_GT(a, b) ((a).key > (b).key)
#define DS_REDBLACKTREE_EQ(a, b) ((a).key == (b).key)
#define DS_REDBLACKTREE_LE(a, b) ((a).key <= (b).key)
#define DS_REDBLACKTREE_GE(a, b) ((a).key >= (b).key)

#define DS_REDBLACKTREE_DESTROY_ELEMENT(e) \
    do                                     \
    {                                      \
    } while (0)

#define DS_REDBLACKTREE_CLONE_ELEMENT(e, judge) \
    ((ds_redblacktree_type){.key = (e).key,     \
                            .value = (e).value})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_redblacktree_type;

// 需要修改 DESTROY_ELEMENT
#define DS_REDBLACKTREE_DESTROY_ELEMENT(e) \
    do                                \
    {                                 \
        free((e).name);               \
        (e).name = NULL;              \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_redblacktree_type clone_element(const ds_redblacktree_type *src, int *judge)
{
    ds_redblacktree_type copy = {.key = src->key, .value = src->value, .name = NULL};
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
#define DS_REDBLACKTREE_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 比较宏不变（按 key 比较）
*/
