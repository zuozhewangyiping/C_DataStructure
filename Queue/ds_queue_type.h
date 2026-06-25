#ifndef DS_QUEUE_TYPE_H
#define DS_QUEUE_TYPE_H

typedef struct
{
    int data;
} ds_queue_type;

// 以下宏接收的参数 e 类型为 DS_QUEUE_TYPE

#define DS_QUEUE_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
    } while (0)

#define DS_QUEUE_CLONE_ELEMENT(e, judge) \
    ((ds_queue_type){.data = (e).data})

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_queue_type;

// 需要修改 DESTROY_ELEMENT
#define DS_QUEUE_DESTROY_ELEMENT(e) \
    do                              \
    {                               \
        free((e).name);             \
        (e).name = NULL;            \
    } while (0)

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_queue_type clone_element(const ds_queue_type *src, int *judge)
{
    ds_queue_type copy = {.data = src->data, .name = NULL};
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
#define DS_QUEUE_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// MATCH 宏不变（按 data 匹配），如需按 name 匹配则修改：
// #define DS_QUEUE_MATCH(e, target) (strcmp((e).name, target) == 0 ? 1 : 0)
*/
