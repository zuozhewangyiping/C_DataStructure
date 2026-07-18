#ifndef DS_DYNAMICARRAY_TYPE_H
#define DS_DYNAMICARRAY_TYPE_H

typedef struct
{
    int data;
} ds_dynamicarray_type;

// 以下宏及函数均操作 DS_DYNAMICARRAY_TYPE 类型

static inline void destroy_element(ds_dynamicarray_type *e)
{
    (void)e;
}
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_dynamicarray_type clone_element(const ds_dynamicarray_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#define DS_DYNAMICARRAY_MATCH_TYPE int
static inline int match_element(const ds_dynamicarray_type *e, DS_DYNAMICARRAY_MATCH_TYPE target)
{
    return e->data == target ? 1 : 0;
}
#define DS_DYNAMICARRAY_MATCH(e, target) match_element(&(e), target)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_dynamicarray_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(ds_dynamicarray_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) destroy_element(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_dynamicarray_type clone_element(const ds_dynamicarray_type *src, int *judge)
{
    ds_dynamicarray_type copy = {.data = src->data, .name = NULL};
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
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 如需按 name 匹配，同步修改 DS_DYNAMICARRAY_MATCH_TYPE 及 match_element 函数体：
// #define DS_DYNAMICARRAY_MATCH_TYPE char *
// static inline int match_element(const ds_dynamicarray_type *e, DS_DYNAMICARRAY_MATCH_TYPE target)
// {
//     return strcmp(e->name, target) == 0 ? 1 : 0;
// }
*/
