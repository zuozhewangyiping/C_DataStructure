#ifndef DS_DOUBLELINKEDLIST_TYPE_H
#define DS_DOUBLELINKEDLIST_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_doublelinkedlist_type;

// 以下宏及函数均操作 DS_DOUBLELINKEDLIST_TYPE 类型

static inline void destroy_element(ds_doublelinkedlist_type *e)
{
    (void)e;
}
#define DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_doublelinkedlist_type clone_element(const ds_doublelinkedlist_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_DOUBLELINKEDLIST_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#define DS_DOUBLELINKEDLIST_MATCH_TYPE int
static inline int match_element(const ds_doublelinkedlist_type *e, DS_DOUBLELINKEDLIST_MATCH_TYPE target)
{
    return e->key == target ? 1 : 0;
}
#define DS_DOUBLELINKEDLIST_MATCH(e, target) match_element(&(e), target)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int key;
    int value;
    char *name;
} ds_doublelinkedlist_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(ds_doublelinkedlist_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(e) destroy_element(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_doublelinkedlist_type clone_element(const ds_doublelinkedlist_type *src, int *judge)
{
    ds_doublelinkedlist_type copy = {.key = src->key, .value = src->value, .name = NULL};
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
#define DS_DOUBLELINKEDLIST_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 如需按 name 匹配，同步修改 DS_DOUBLELINKEDLIST_MATCH_TYPE 及 match_element 函数体：
// #define DS_DOUBLELINKEDLIST_MATCH_TYPE char *
// static inline int match_element(const ds_doublelinkedlist_type *e, DS_DOUBLELINKEDLIST_MATCH_TYPE target)
// {
//     return strcmp(e->name, target) == 0 ? 1 : 0;
// }
*/
