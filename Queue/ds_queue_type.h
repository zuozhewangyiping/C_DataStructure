#ifndef DS_QUEUE_TYPE_H
#define DS_QUEUE_TYPE_H

typedef struct
{
    int data;
} ds_queue_type;

// 以下宏接收的参数 e 类型为 DS_QUEUE_TYPE

static inline void destroy_element(const ds_queue_type *e)
{
    (void)e;
}
#define DS_QUEUE_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_queue_type clone_element(const ds_queue_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_QUEUE_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_queue_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(const ds_queue_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_QUEUE_DESTROY_ELEMENT(e) destroy_element(&(e))

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
*/
