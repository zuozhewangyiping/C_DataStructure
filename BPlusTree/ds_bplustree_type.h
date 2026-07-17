#ifndef DS_BPLUSTREE_TYPE_H
#define DS_BPLUSTREE_TYPE_H

typedef struct
{
    int key;
} ds_bplustree_key_type;

typedef struct
{
    int value;
} ds_bplustree_value_type;

// 以下参数 a,b 为 DS_BPLUSTREE_KEY_TYPE

#define DS_BPLUSTREE_LT(a, b) ((a).key < (b).key)
#define DS_BPLUSTREE_GT(a, b) ((a).key > (b).key)
#define DS_BPLUSTREE_EQ(a, b) ((a).key == (b).key)
#define DS_BPLUSTREE_LE(a, b) ((a).key <= (b).key)
#define DS_BPLUSTREE_GE(a, b) ((a).key >= (b).key)

#endif

/*
// 宏修改示例：将 value 改为包含字符串的结构体
#include <string.h>

typedef struct
{
    int value;
    char name[64];   // 固定长度，不能使用 char *（指针无法持久化到磁盘）
} ds_bplustree_value_type;

// 比较宏不变（按 key 比较）
// 无需 DESTROY / CLONE 宏 —— 磁盘版通过 fread/fwrite 整页读写，sizeof 固定即可
*/
