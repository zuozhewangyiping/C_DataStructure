#ifndef DS_DOUBLELINKEDLIST_TYPE_H
#define DS_DOUBLELINKEDLIST_TYPE_H

typedef struct
{
    int key;
    int value;
} ds_doublelinkedlist_type;

// 以下宏接收的参数 e 类型为 DS_DOUBLELINKEDLIST_TYPE

// DESTROY宏用于ds_doublelinkedlist_destroy & XXX_and_destroy & ds_doublelinkedlist_set函数
#define DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(e) \
    do                                         \
    {                                          \
        /* nothing to free */                  \
    } while (0)

// CLONE宏用于ds_doublelinkedlist_clone函数
#define DS_DOUBLELINKEDLIST_CLONE_ELEMENT(e, judge) \
    ((ds_doublelinkedlist_type){.key = (e).key,     \
                                .value = (e).value})

// 两个MATCH宏用于ds_doublelinkedlist_find函数
#define DS_DOUBLELINKEDLIST_MATCH_TYPE int
#define DS_DOUBLELINKEDLIST_MATCH(e, target) ((e).key == target ? 1 : 0)

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
#define DS_DOUBLELINKEDLIST_DESTROY_ELEMENT(e) \
    do                                         \
    {                                          \
        free((e).name);                        \
        (e).name = NULL;                       \
    } while (0)

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

// MATCH 宏不变（按 key 匹配），如需按 name 匹配则修改：
// #define DS_DOUBLELINKEDLIST_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/
