#ifndef DS_PRIORITYQUEUE_MIN_TYPE_H
#define DS_PRIORITYQUEUE_MIN_TYPE_H

typedef struct
{
    int data;
} ds_priorityqueue_min_type;

// 以下宏及函数均操作 DS_PRIORITYQUEUE_MIN_TYPE 类型

#define DS_PRIORITYQUEUE_MIN_LT(a, b) ((a).data < (b).data)
#define DS_PRIORITYQUEUE_MIN_GT(a, b) ((a).data > (b).data)
#define DS_PRIORITYQUEUE_MIN_EQ(a, b) ((a).data == (b).data)
#define DS_PRIORITYQUEUE_MIN_LE(a, b) ((a).data <= (b).data)
#define DS_PRIORITYQUEUE_MIN_GE(a, b) ((a).data >= (b).data)

static inline void destroy_element_min(ds_priorityqueue_min_type *e)
{
    (void)e;
}
#define DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(e) destroy_element_min(&(e))

static inline ds_priorityqueue_min_type clone_element_min(const ds_priorityqueue_min_type *src, int *judge)
{
    (void)judge;
    return *src;
}
#define DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(e, judge) clone_element_min(&(e), judge)

#endif

/*
// 宏修改示例：添加 char *name 成员后
typedef struct
{
    int data;
    char *name;
} ds_priorityqueue_min_type;

// 需要修改 DESTROY_ELEMENT
static inline void destroy_element_min(ds_priorityqueue_min_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_PRIORITYQUEUE_MIN_DESTROY_ELEMENT(e) destroy_element_min(&(e))

// 需要修改 CLONE_ELEMENT（深拷贝）
// 编写适配函数，再由宏调用
static inline ds_priorityqueue_min_type clone_element_min(const ds_priorityqueue_min_type *src, int *judge)
{
    ds_priorityqueue_min_type copy = {.data = src->data, .name = NULL};
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
#define DS_PRIORITYQUEUE_MIN_CLONE_ELEMENT(e, judge) clone_element_min(&(e), judge)

// 比较宏（按 data 比较）。若 data 改为 char *，需同步修改为 strcmp 版本：
// #define DS_PRIORITYQUEUE_MIN_LT(a, b) (strcmp((a).data, (b).data) < 0)
// #define DS_PRIORITYQUEUE_MIN_GT(a, b) (strcmp((a).data, (b).data) > 0)
// #define DS_PRIORITYQUEUE_MIN_EQ(a, b) (strcmp((a).data, (b).data) == 0)
// #define DS_PRIORITYQUEUE_MIN_LE(a, b) (strcmp((a).data, (b).data) <= 0)
// #define DS_PRIORITYQUEUE_MIN_GE(a, b) (strcmp((a).data, (b).data) >= 0)
*/