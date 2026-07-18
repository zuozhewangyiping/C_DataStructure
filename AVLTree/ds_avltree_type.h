#ifndef DS_AVLTREE_TYPE_H
#define DS_AVLTREE_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_avltree_type;

// 以下宏接收的参数 e 类型为 DS_AVLTREE_TYPE

#define DS_AVLTREE_LT(a, b) ((a).key < (b).key)
#define DS_AVLTREE_GT(a, b) ((a).key > (b).key)
#define DS_AVLTREE_EQ(a, b) ((a).key == (b).key)
#define DS_AVLTREE_LE(a, b) ((a).key <= (b).key)
#define DS_AVLTREE_GE(a, b) ((a).key >= (b).key)

static inline void destroy_element(const ds_avltree_type *e)
{
    (void)e;
}
#define DS_AVLTREE_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_avltree_type clone_element(const ds_avltree_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_AVLTREE_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_avltree_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(ds_avltree_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_AVLTREE_DESTROY_ELEMENT(e) destroy_element(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_avltree_type clone_element(const ds_avltree_type *src, int *judge)
{
    ds_avltree_type copy = {.key = src->key, .value = src->value, .name = NULL};
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
#define DS_AVLTREE_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 比较宏（按 key 比较）。若 key 改为 char *，需同步修改为 strcmp 版本：
// #define DS_AVLTREE_LT(a, b) (strcmp((a).key, (b).key) < 0)
// #define DS_AVLTREE_GT(a, b) (strcmp((a).key, (b).key) > 0)
// #define DS_AVLTREE_EQ(a, b) (strcmp((a).key, (b).key) == 0)
// #define DS_AVLTREE_LE(a, b) (strcmp((a).key, (b).key) <= 0)
// #define DS_AVLTREE_GE(a, b) (strcmp((a).key, (b).key) >= 0)
*/
