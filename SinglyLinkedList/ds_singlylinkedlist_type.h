#ifndef DS_SINGLYLINKEDLIST_TYPE_H
#define DS_SINGLYLINKEDLIST_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_singlylinkedlist_type;

// 以下宏接收的参数 e 类型为 DS_SINGLYLINKEDLIST_TYPE

static inline void destroy_element(const ds_singlylinkedlist_type *e)
{
    (void)e;
}
#define DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_singlylinkedlist_type clone_element(const ds_singlylinkedlist_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_SINGLYLINKEDLIST_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#define DS_SINGLYLINKEDLIST_MATCH_TYPE int
static inline int match_element(const ds_singlylinkedlist_type *e, DS_SINGLYLINKEDLIST_MATCH_TYPE target)
{
    return (e)->key == target ? 1 : 0;
}
#define DS_SINGLYLINKEDLIST_MATCH(e, target) match_element(&(e), target)

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
static inline void destroy_element(ds_singlylinkedlist_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_SINGLYLINKEDLIST_DESTROY_ELEMENT(e) destroy_element(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_singlylinkedlist_type clone_element(const ds_singlylinkedlist_type *src, int *judge)
{
    ds_singlylinkedlist_type copy = {.key = src->key, .value = src->value, .name = NULL};
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
#define DS_SINGLYLINKEDLIST_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 如需按 name 匹配，同步修改 DS_SINGLYLINKEDLIST_MATCH_TYPE 及 match_element 函数体：
// #define DS_SINGLYLINKEDLIST_MATCH_TYPE char *
// static inline int match_element(const ds_singlylinkedlist_type *e, DS_SINGLYLINKEDLIST_MATCH_TYPE target)
// {
//     return strcmp(e->name, target) == 0 ? 1 : 0;
// }
*/