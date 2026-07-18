#ifndef DS_PRIORITYQUEUE_MAX_TYPE_H
#define DS_PRIORITYQUEUE_MAX_TYPE_H

typedef struct
{
    int data;
} ds_priorityqueue_max_type;

// 以下宏接收的参数 a,b,e 类型为 DS_PRIORITYQUEUE_MAX_TYPE

#define DS_PRIORITYQUEUE_MAX_LT(a, b) ((a).data < (b).data)
#define DS_PRIORITYQUEUE_MAX_GT(a, b) ((a).data > (b).data)
#define DS_PRIORITYQUEUE_MAX_EQ(a, b) ((a).data == (b).data)
#define DS_PRIORITYQUEUE_MAX_LE(a, b) ((a).data <= (b).data)
#define DS_PRIORITYQUEUE_MAX_GE(a, b) ((a).data >= (b).data)

static inline void destroy_element(const ds_priorityqueue_max_type *e)
{
    (void)e;
}
#define DS_PRIORITYQUEUE_MAX_DESTROY_ELEMENT(e) destroy_element(&(e))

static inline ds_priorityqueue_max_type clone_element(const ds_priorityqueue_max_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_PRIORITYQUEUE_MAX_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_priorityqueue_max_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element(const ds_priorityqueue_max_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_PRIORITYQUEUE_MAX_DESTROY_ELEMENT(e) destroy_element(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_priorityqueue_max_type clone_element(const ds_priorityqueue_max_type *src, int *judge)
{
    ds_priorityqueue_max_type copy = {.data = src->data, .name = NULL};
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
#define DS_PRIORITYQUEUE_MAX_CLONE_ELEMENT(e, judge) clone_element(&(e), judge)

// 比较宏（按 data 比较）。若 data 改为 char *，需同步修改为 strcmp 版本：
// #define DS_PRIORITYQUEUE_MAX_LT(a, b) (strcmp((a).data, (b).data) < 0)
// #define DS_PRIORITYQUEUE_MAX_GT(a, b) (strcmp((a).data, (b).data) > 0)
// #define DS_PRIORITYQUEUE_MAX_EQ(a, b) (strcmp((a).data, (b).data) == 0)
// #define DS_PRIORITYQUEUE_MAX_LE(a, b) (strcmp((a).data, (b).data) <= 0)
// #define DS_PRIORITYQUEUE_MAX_GE(a, b) (strcmp((a).data, (b).data) >= 0)
*/