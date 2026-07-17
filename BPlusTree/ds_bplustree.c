#include <stdio.h>
#include <stdlib.h>
#include "ds_bplustree.h"

#define DS_BPLUSTREE_PAGE_SIZE 4096

/* 页类型标记 */
#define PAGE_TYPE_FILE_HEADER 0
#define PAGE_TYPE_INTERNAL 1
#define PAGE_TYPE_LEAF 2
#define PAGE_TYPE_DISCARDED 3

#define M (int)(4084 / (4 + sizeof(DS_BPLUSTREE_KEY_TYPE)))
#define N (int)(4080 / (sizeof(DS_BPLUSTREE_KEY_TYPE) + sizeof(DS_BPLUSTREE_VALUE_TYPE)))

struct DS_BPlusTree
{
    FILE *file; // 文件指针（纯内存）
    int root_page;
    int size;
    int level;
    int first_discarded_page;
};

struct BPlusTreeNode
{
    int page_no;                   // 页号
    int slot_index;                // 槽位
    DS_BPLUSTREE_KEY_TYPE key;     // key 副本
    DS_BPLUSTREE_VALUE_TYPE value; // value 副本
};

/* 文件头页的内容 */
typedef struct
{
    int page_type;
    int num_keys;
    int root_page;
    int size;
    int level;
    int first_discarded_page;
} FileHeaderPage;

/* 内部节点页的内容 */
typedef struct
{
    int page_type;
    int num_keys;
    int children[M + 1];
    DS_BPLUSTREE_KEY_TYPE keys[M];
} InternalPage;

/* 叶子节点页的内容 */
typedef struct
{
    int page_type;
    int num_keys;
    int prev_page;
    int next_page;
    DS_BPLUSTREE_KEY_TYPE keys[N];
    DS_BPLUSTREE_VALUE_TYPE values[N];
} LeafPage;

/* 废弃页的内容 */
typedef struct
{
    int page_type;
    int next_free; // 下一废弃页号，0 = 链表尾
} DiscardedPage;

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_BPlusTree *ds_bplustree_create(const char *filename)
{
    FILE *fp = fopen(filename, "r+b"); // 先尝试打开已有文件
    if (fp == NULL)
    {
        fp = fopen(filename, "w+b"); // 文件不存在，创建新文件
        if (fp == NULL)
        {
            return NULL;
        }
    }

    DS_BPlusTree *bpt = (DS_BPlusTree *)malloc(sizeof(DS_BPlusTree));
    if (bpt == NULL)
    {
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fclose(fp);
        free(bpt);
        return NULL;
    }
    long file_size = ftell(fp);
    if (file_size < 0)
    {
        fclose(fp);
        free(bpt);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        fclose(fp);
        free(bpt);
        return NULL;
    }

    if (file_size == 0) // 如果文件为空，创建文件头页的内容
    {
        char all[DS_BPLUSTREE_PAGE_SIZE] = {0};

        FileHeaderPage *fh = (FileHeaderPage *)all;

        fh->page_type = PAGE_TYPE_FILE_HEADER;
        fh->root_page = 0;
        fh->size = 0;
        fh->level = 0;
        fh->first_discarded_page = 0;

        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            fclose(fp);
            free(bpt);
            return NULL;
        }
        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            fclose(fp);
            free(bpt);
            return NULL;
        }
    }

    // 统一读取文件头内容并写入B+树

    char all[DS_BPLUSTREE_PAGE_SIZE];

    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        fclose(fp);
        free(bpt);
        return NULL;
    }

    FileHeaderPage *fh = (FileHeaderPage *)all;

    bpt->root_page = fh->root_page;
    bpt->size = fh->size;
    bpt->level = fh->level;
    bpt->first_discarded_page = fh->first_discarded_page;

    bpt->file = fp;

    return bpt;
}

void ds_bplustree_destroy(DS_BPlusTree *bpt)
{
    if (bpt == NULL)
    {
        return;
    }

    fclose(bpt->file);
    free(bpt);
}

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_bplustree_size(const DS_BPlusTree *bpt)
{
    if (bpt == NULL)
    {
        return 0;
    }

    return bpt->size;
}

int ds_bplustree_level(const DS_BPlusTree *bpt)
{
    if (bpt == NULL)
    {
        return 0;
    }

    return bpt->level;
}

int ds_bplustree_is_empty(const DS_BPlusTree *bpt)
{
    if (bpt == NULL)
    {
        return -1;
    }

    return (bpt->size == 0 ? 1 : 0);
}

/* Part 3. Traversals（回调只读 data）---------------------------------------*/

int ds_bplustree_traverse(DS_BPlusTree *bpt, void *user_data, void (*visit)(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *callback_data))
{
    if (bpt == NULL || visit == NULL)
    {
        return 0;
    }

    if (bpt->size == 0)
    {
        return 1;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    // step 1.找到第一个叶子页

    while (1)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        InternalPage *ip = (InternalPage *)all;

        page_num = ip->children[0];
    }

    // step 2.按顺序对所有叶子页的内容进行遍历

    while (1)
    {
        LeafPage *lp = (LeafPage *)all;

        int i = 0;
        for (i = 0; i < lp->num_keys; i++)
        {
            visit(&lp->keys[i], &lp->values[i], user_data);
        }

        page_num = lp->next_page;

        if (page_num == 0)
        {
            break;
        }

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }
    }

    return 1;
}

/* Part 4. Clone ------------------------------------------------------------*/

DS_BPlusTree *ds_bplustree_clone(const DS_BPlusTree *bpt, const char *filename, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    if (bpt == NULL)
    {
        *judge = 1;

        return NULL;
    }

    *judge = 1;

    FILE *fp_c = fopen(filename, "w+b"); // 打开文件
    if (fp_c == NULL)
    {
        *judge = 0;
        return NULL;
    }

    FILE *fp = bpt->file;

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        *judge = 0;
        fclose(fp_c);
        remove(filename);
        return NULL;
    }
    long file_size = ftell(fp);
    if (file_size < 0)
    {
        *judge = 0;
        fclose(fp_c);
        remove(filename);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        *judge = 0;
        fclose(fp_c);
        remove(filename);
        return NULL;
    }

    long i = 0;
    for (i = 0; i < (file_size + DS_BPLUSTREE_PAGE_SIZE - 1) / DS_BPLUSTREE_PAGE_SIZE; i++)
    {
        char buf[DS_BPLUSTREE_PAGE_SIZE];

        if (fread(buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            *judge = 0;
            fclose(fp_c);
            remove(filename);
            return NULL;
        }

        if (fwrite(buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp_c) != 1)
        {
            *judge = 0;
            fclose(fp_c);
            remove(filename);
            return NULL;
        }
    }

    fclose(fp_c);

    DS_BPlusTree *new_bpt = ds_bplustree_create(filename);
    if (new_bpt == NULL)
    {
        *judge = 0;
        remove(filename);
    }

    return new_bpt;
}

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_bplustree_node_get_data(BPlusTreeNode *cursor, DS_BPLUSTREE_KEY_TYPE **x_key, DS_BPLUSTREE_VALUE_TYPE **x_value)
{
    if (cursor == NULL || x_key == NULL || x_value == NULL)
    {
        return 0;
    }

    *x_key = &(cursor->key);
    *x_value = &(cursor->value);

    return 1;
}

/* Part 6. Search -----------------------------------------------------------*/

BPlusTreeNode *ds_bplustree_search(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key)
{
    if (bpt == NULL || bpt->size == 0)
    {
        return NULL;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    // step 1.从root_page开始，层层往下查找到叶子页

    while (1)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return NULL;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return NULL;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        InternalPage *ip = (InternalPage *)all;

        int is_found = 0;

        int i = 0;
        for (i = 0; i < ip->num_keys; i++)
        {
            if (DS_BPLUSTREE_LT(key, ip->keys[i]))
            {
                page_num = ip->children[i];

                is_found = 1;

                break;
            }
        }

        if (is_found == 0)
        {
            page_num = ip->children[ip->num_keys];
        }
    }

    // step 2.在叶子页中进行顺序查找

    LeafPage *lp = (LeafPage *)all;

    int i = 0;
    for (i = 0; i < lp->num_keys; i++)
    {
        if (DS_BPLUSTREE_EQ(key, lp->keys[i]))
        {
            BPlusTreeNode *cursor = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
            if (cursor == NULL)
            {
                return NULL;
            }

            cursor->page_no = page_num;
            cursor->slot_index = i;
            cursor->key = key;
            cursor->value = lp->values[i];

            return cursor;
        }
    }

    return NULL;
}

/* Part 7. Cursor Acquisition -----------------------------------------------*/

BPlusTreeNode *ds_bplustree_find_max(DS_BPlusTree *bpt)
{
    if (bpt == NULL || bpt->size == 0)
    {
        return NULL;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    while (1) // 沿最右边直达叶子层
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return NULL;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return NULL;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        InternalPage *ip = (InternalPage *)all;

        page_num = ip->children[ip->num_keys];
    }

    LeafPage *lp = (LeafPage *)all;

    BPlusTreeNode *cursor = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    if (cursor == NULL)
    {
        return NULL;
    }

    cursor->page_no = page_num;
    cursor->slot_index = lp->num_keys - 1;
    cursor->key = lp->keys[lp->num_keys - 1];
    cursor->value = lp->values[lp->num_keys - 1];

    return cursor;
}

BPlusTreeNode *ds_bplustree_find_min(DS_BPlusTree *bpt)
{
    if (bpt == NULL || bpt->size == 0)
    {
        return NULL;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    while (1) // 沿最左边直达叶子层
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return NULL;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return NULL;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        InternalPage *ip = (InternalPage *)all;

        page_num = ip->children[0];
    }

    LeafPage *lp = (LeafPage *)all;

    BPlusTreeNode *cursor = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    if (cursor == NULL)
    {
        return NULL;
    }

    cursor->page_no = page_num;
    cursor->slot_index = 0;
    cursor->key = lp->keys[0];
    cursor->value = lp->values[0];

    return cursor;
}

/* Part 8. Cursor Navigation ------------------------------------------------*/

BPlusTreeNode *ds_bplustree_predecessor(DS_BPlusTree *bpt, BPlusTreeNode *cursor)
{
    if (bpt == NULL || bpt->size == 0 || cursor == NULL)
    {
        return NULL;
    }

    FILE *fp = bpt->file;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    BPlusTreeNode *cursor_p = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    if (cursor_p == NULL)
    {
        return NULL;
    }

    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * cursor->page_no, SEEK_SET) != 0)
    {
        free(cursor_p);
        return NULL;
    }
    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        free(cursor_p);
        return NULL;
    }

    LeafPage *lp = (LeafPage *)all;

    if (cursor->slot_index > 0)
    {
        cursor_p->page_no = cursor->page_no;
        cursor_p->slot_index = cursor->slot_index - 1;
        cursor_p->key = lp->keys[cursor->slot_index - 1];
        cursor_p->value = lp->values[cursor->slot_index - 1];
    }
    else
    {
        if (lp->prev_page == 0) // 如果没有上一页
        {
            free(cursor_p);

            return NULL;
        }

        int page_num = lp->prev_page;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            free(cursor_p);
            return NULL;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            free(cursor_p);
            return NULL;
        }

        lp = (LeafPage *)all;

        cursor_p->page_no = page_num;
        cursor_p->slot_index = lp->num_keys - 1;
        cursor_p->key = lp->keys[lp->num_keys - 1];
        cursor_p->value = lp->values[lp->num_keys - 1];
    }

    return cursor_p;
}

BPlusTreeNode *ds_bplustree_successor(DS_BPlusTree *bpt, BPlusTreeNode *cursor)
{
    if (bpt == NULL || bpt->size == 0 || cursor == NULL)
    {
        return NULL;
    }

    FILE *fp = bpt->file;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    BPlusTreeNode *cursor_p = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    if (cursor_p == NULL)
    {
        return NULL;
    }

    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * cursor->page_no, SEEK_SET) != 0)
    {
        free(cursor_p);
        return NULL;
    }
    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        free(cursor_p);
        return NULL;
    }

    LeafPage *lp = (LeafPage *)all;

    if (cursor->slot_index < lp->num_keys - 1)
    {
        cursor_p->page_no = cursor->page_no;
        cursor_p->slot_index = cursor->slot_index + 1;
        cursor_p->key = lp->keys[cursor->slot_index + 1];
        cursor_p->value = lp->values[cursor->slot_index + 1];
    }
    else
    {
        if (lp->next_page == 0) // 如果没有下一页
        {
            free(cursor_p);

            return NULL;
        }

        int page_num = lp->next_page;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            free(cursor_p);
            return NULL;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            free(cursor_p);
            return NULL;
        }

        lp = (LeafPage *)all;

        cursor_p->page_no = page_num;
        cursor_p->slot_index = 0;
        cursor_p->key = lp->keys[0];
        cursor_p->value = lp->values[0];
    }

    return cursor_p;
}

/* Part 9. Range Query（回调只读 data）-------------------------------------*/

int ds_bplustree_range_query(DS_BPlusTree *bpt,
                             DS_BPLUSTREE_KEY_TYPE low,
                             DS_BPLUSTREE_KEY_TYPE high,
                             void *user_data,
                             void (*visit)(const DS_BPLUSTREE_KEY_TYPE *key, const DS_BPLUSTREE_VALUE_TYPE *value, void *callback_data))
{
    if (bpt == NULL)
    {
        return 0;
    }

    if (bpt->size == 0)
    {
        return 1;
    }

    if (visit == NULL)
    {
        return 0;
    }

    if (DS_BPLUSTREE_GT(low, high))
    {
        return 1;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    // step 1.从root_page开始，层层往下查找到叶子页

    while (1)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        InternalPage *ip = (InternalPage *)all;

        int is_found = 0;

        int i = 0;
        for (i = 0; i < ip->num_keys; i++)
        {
            if (DS_BPLUSTREE_LT(low, ip->keys[i]))
            {
                page_num = ip->children[i];

                is_found = 1;

                break;
            }
        }

        if (is_found == 0)
        {
            page_num = ip->children[ip->num_keys];
        }
    }

    // step 2.在叶子页中进行顺序查找（找到第一个 >= low 的key）

    LeafPage *lp = (LeafPage *)all;

    while (DS_BPLUSTREE_LT(lp->keys[lp->num_keys - 1], low)) // 特殊情况：若目前定位的叶子页的最大项仍然小于low，那么跳到下一页（可能不止一次）
    {
        page_num = lp->next_page;

        if (page_num == 0)
        {
            return 1;
        }

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        lp = (LeafPage *)all;
    }

    int i = 0;
    for (i = 0; i < lp->num_keys; i++)
    {
        if (DS_BPLUSTREE_GE(lp->keys[i], low))
        {
            // 此时找到了low对应的最小i，接下来进行区间traverse

            for (; i < lp->num_keys; i++) // 先将本页遍历
            {
                if (DS_BPLUSTREE_GT(lp->keys[i], high))
                {
                    return 1;
                }

                visit(&lp->keys[i], &lp->values[i], user_data);
            }

            page_num = lp->next_page; // 进入到下一页

            while (1) // 接下来批量遍历
            {
                if (page_num == 0)
                {
                    return 1;
                }

                if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
                {
                    return 0;
                }
                if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                {
                    return 0;
                }

                lp = (LeafPage *)all;

                for (i = 0; i < lp->num_keys; i++)
                {
                    if (DS_BPLUSTREE_GT(lp->keys[i], high))
                    {
                        return 1;
                    }

                    visit(&lp->keys[i], &lp->values[i], user_data);
                }

                page_num = lp->next_page;
            }
        }
    }

    return 0;
}

/* Part 10. Insert / Delete -------------------------------------------------*/

static int alloc_page(DS_BPlusTree *bpt)
{
    int new_page;

    FILE *fp = bpt->file;

    if (bpt->first_discarded_page != 0)
    {
        new_page = bpt->first_discarded_page;

        char all[4096];

        DiscardedPage *dp;
        FileHeaderPage *hp;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * new_page, SEEK_SET) != 0)
        {
            return -1;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return -1;
        }

        dp = (DiscardedPage *)all;

        int next_free = dp->next_free;

        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            return -1;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return -1;
        }

        hp = (FileHeaderPage *)all;

        hp->first_discarded_page = next_free;

        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            return -1;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return -1;
        }

        bpt->first_discarded_page = next_free;
    }
    else
    {
        if (fseek(fp, 0, SEEK_END) != 0)
        {
            return -1;
        }
        long file_size = ftell(fp);
        if (file_size < 0)
        {
            return -1;
        }

        new_page = file_size / DS_BPLUSTREE_PAGE_SIZE;
    }

    return new_page;
}

static int free_page(DS_BPlusTree *bpt, int page_no)
{
    FILE *fp = bpt->file;

    char all[4096] = {0};

    DiscardedPage *dp;
    FileHeaderPage *hp;

    dp = (DiscardedPage *)all;

    dp->page_type = PAGE_TYPE_DISCARDED;
    dp->next_free = bpt->first_discarded_page;

    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_no, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    hp = (FileHeaderPage *)all;

    hp->first_discarded_page = page_no;

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    bpt->first_discarded_page = page_no;

    return 1;
}

int ds_bplustree_insert(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key, DS_BPLUSTREE_VALUE_TYPE value)
{
    /*
        **对insert函数的解读**
        情况一、空树：创建第一个叶子页，更新文件头页和B+树元数据，插入完成。
        情况二、非空树，按四步执行：
            第一步——从root_page开始，层层往下查找到叶子页，并记录下降路径。
            第二步——在叶子页中顺序查找，确认无重复key后，定位第一个 > key 的位置作为插入点。
            第三步——插入新内容：
                1.本页未满（lp->num_keys + 1 <= N）：直接后移元素、放入新key/value即可；
                2.本页已满，需分裂叶子页 + 向上更新内部节点：
                Part 1.叶子层处理（9步）：
                    (1)将旧页内容与新key/value合并到 temp_keys[N+1] / temp_values[N+1] 暂存；
                    (2)存档旧页 next_page（后续链表修复用）；
                    (3)确定新叶子页的页码（file_size / PAGE_SIZE）；
                    (4)旧页取 temp 前半段（(N+1)/2 个元素），next 指向新叶；
                    (5)记录分裂点 temp_pos（供内部节点路由使用）；
                    (6)旧页写盘；
                    (7)新页取 temp 后半段（(N+2)/2 个元素），prev 指向旧叶，next 接上原链表后继；
                    (8)新页写盘；
                    (9)更新原链表后继页的 prev_page 指向新叶。
                Part 2.内部节点处理：
                    以 new_key = 新叶最小key、new_child = 新叶页号为初始值，从父节点（depth-1）开始向上循环：
                    (1)本层未满（ip->num_keys + 1 <= M）：插入路由key与child，写盘，结束；
                    (2)本层已满：将本层数据与 new_key/new_child 暂存到 temp，本层取前半、新页取后半，上传中间key，new_child = 新内部页号，继续上层循环；
                    (3)循环至上层的上一层（i < 0）：创建新根（仅 1 个路由key 和 2 个child），标记 is_level_add / is_new_root_page。
            第四步——更新文件头页（size++，若 is_level_add 则 level++，若 is_new_root_page 则更新 root_page）和B+树元数据，写入文件。
    */

    if (bpt == NULL)
    {
        return 0;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    FileHeaderPage *fh;
    InternalPage *ip;
    LeafPage *lp;

    // 设定两个全局标记
    int is_level_add = 0;     // is_level_add 变为非零意味着全局 level++
    int is_new_root_page = 0; // is_new_root_page 变为非零意味着全局 root_page 变为 is_new_root_page

    if (page_num == 0) // 情况一、空树：创建第一个叶子页，更新文件头页和B+树元数据，插入完成
    {
        // 第一步：分配新页
        page_num = alloc_page(bpt);
        if (page_num < 0)
        {
            return 0;
        }

        // 第二步：构造新叶子页并写入文件
        lp = (LeafPage *)all;

        lp->page_type = PAGE_TYPE_LEAF;
        lp->num_keys = 1;
        lp->prev_page = 0;
        lp->next_page = 0;
        lp->keys[0] = key;
        lp->values[0] = value;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // 第三步：更新文件头页内容并写入文件

        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        fh = (FileHeaderPage *)all;

        fh->root_page = page_num;
        fh->size = 1;
        fh->level = 1;

        if (fseek(fp, 0, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // 第四步：更新B+树
        bpt->root_page = page_num;
        bpt->size = 1;
        bpt->level = 1;

        return 1;
    }

    // 情况二、非空树，按四步执行

    /* 第一步——从root_page开始，层层往下查找到叶子页，并记录下降路径 */

    int path_pages[16]; // 下降时经过的页号
    int path_index[16]; // 对应 children[i] 中的 i
    int depth = 0;      // 路径深度（不算叶子）

    while (1)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        ip = (InternalPage *)all;

        int is_found = 0;

        path_pages[depth] = page_num; // 每次下降前记录

        int i = 0;
        for (i = 0; i < ip->num_keys; i++)
        {
            if (DS_BPLUSTREE_LT(key, ip->keys[i]))
            {
                page_num = ip->children[i];

                is_found = 1;

                break;
            }
        }

        if (is_found == 0)
        {
            i = ip->num_keys;

            page_num = ip->children[ip->num_keys];
        }

        path_index[depth] = i; // children[i] 就是下一步要走的
        depth++;
    }

    /* 第二步——在叶子页中顺序查找，确认无重复key后，定位第一个 > key 的位置作为插入点 */

    lp = (LeafPage *)all;

    int i = 0;
    for (i = 0; i < lp->num_keys; i++)
    {
        if (DS_BPLUSTREE_EQ(lp->keys[i], key))
        {
            // 发现key重复，拒绝插入，直接返回

            return 0;
        }

        if (DS_BPLUSTREE_GT(lp->keys[i], key))
        {
            // 发现了合适的插入位置

            break;
        }
    }

    // 此时的 i 就是插入位置的索引
    int pos = i; // pos 就是即将插入的位置（位置备份）

    /* 第三步——插入新内容 */

    if (lp->num_keys + 1 <= N) // 简单情况：本页未满（lp->num_keys + 1 <= N）：直接后移元素、放入新key/value即可
    {
        for (i = lp->num_keys - 1; i >= pos; i--)
        {
            lp->keys[i + 1] = lp->keys[i];
            lp->values[i + 1] = lp->values[i];
        }

        lp->keys[pos] = key;
        lp->values[pos] = value;

        lp->num_keys += 1;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }
    }
    else // 复杂情况：本页已满，需分裂叶子页 + 向上更新内部节点
    {
        /* Part 1.叶子层处理（9 steps）*/

        // step 1.将当前页的内容与插入内容合并暂存于内存
        DS_BPLUSTREE_KEY_TYPE temp_keys[N + 1];
        DS_BPLUSTREE_VALUE_TYPE temp_values[N + 1];

        for (i = 0; i < pos; i++)
        {
            temp_keys[i] = lp->keys[i];
            temp_values[i] = lp->values[i];
        }
        temp_keys[pos] = key;
        temp_values[pos] = value;
        for (i = pos; i < lp->num_keys; i++)
        {
            temp_keys[i + 1] = lp->keys[i];
            temp_values[i + 1] = lp->values[i];
        }

        // step 2.对当前页的next_page进行存档
        int temp_next_page = lp->next_page;

        // step 3.确定新的叶子页的页码
        int new_leaf_page = alloc_page(bpt);
        if (new_leaf_page < 0)
        {
            return 0;
        }

        // step 4.对当前页的内容进行重新整理
        lp->num_keys = (N + 1) / 2;
        lp->next_page = new_leaf_page;

        for (i = 0; i < lp->num_keys; i++)
        {
            lp->keys[i] = temp_keys[i];
            lp->values[i] = temp_values[i];
        }

        // step 5.记录当前已经从内存中读取到的位置
        int temp_pos = lp->num_keys;

        // step 6.写入对当前页的修改
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // step 7.接下来对新的叶子页进行设定
        lp = (LeafPage *)all;

        lp->page_type = PAGE_TYPE_LEAF;
        lp->num_keys = (N + 2) / 2;
        lp->prev_page = page_num;
        lp->next_page = temp_next_page;

        for (i = 0; i < lp->num_keys; i++)
        {
            lp->keys[i] = temp_keys[i + temp_pos];
            lp->values[i] = temp_values[i + temp_pos];
        }

        // step 8.将新的叶子页写入文件
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * new_leaf_page, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // step 9.将新叶子页的下一页内容进行更新（更新prev_page）
        if (temp_next_page != 0)
        {
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_next_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            lp = (LeafPage *)all;

            lp->prev_page = new_leaf_page;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_next_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }
        }

        /* Part 2.内部节点处理：*/

        DS_BPLUSTREE_KEY_TYPE new_key = temp_keys[temp_pos]; // 新叶子页的最小节点
        int new_child = new_leaf_page;                       // 新叶子页的页码

        int is_over = 0;

        for (i = depth - 1; i >= -1; i--) // 包含所有情况：是否有内部节点、是否分裂、分裂是否不止一次
        {
            if (i < 0) // 需要生成新的全局 root_page、全局 level++
            {
                // step 1.确定新的内部节点页的页码
                int new_root_page = alloc_page(bpt);
                if (new_root_page < 0)
                {
                    return 0;
                }

                // step 2.对新的内部节点页进行设定
                ip = (InternalPage *)all;

                ip->page_type = PAGE_TYPE_INTERNAL;
                ip->num_keys = 1;
                ip->children[0] = bpt->root_page;
                ip->children[1] = new_child;
                ip->keys[0] = new_key;

                // step 3.将新的内部节点写入文件
                if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * new_root_page, SEEK_SET) != 0)
                {
                    return 0;
                }
                if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                {
                    return 0;
                }

                // step 4.修改标记
                is_level_add = 1;                 // 树高要+1
                is_new_root_page = new_root_page; // root_page被更新了

                break;
            }

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            ip = (InternalPage *)all;

            int j;

            if (ip->num_keys + 1 <= M) // 简单情况：内部节点没写满（写入即可break）
            {
                for (j = ip->num_keys - 1; j >= path_index[i]; j--) // new_key 替代原本 keys[path_index[i]] 的位置（原 keys[path_index[i]] 及之后右移）
                {
                    ip->keys[j + 1] = ip->keys[j];
                }
                ip->keys[path_index[i]] = new_key;

                for (j = ip->num_keys; j >= path_index[i] + 1; j--) // new_child 替代原本 children[path_index[i] + 1] 的位置（原 children[path_index[i] + 1] 及之后右移）
                {
                    ip->children[j + 1] = ip->children[j];
                }
                ip->children[path_index[i] + 1] = new_child;

                ip->num_keys++;

                is_over = 1; // 完成标记
            }
            else // 复杂情况：内部节点写满了（本层分裂 + 继续向上更新）
            {
                // step 1.将当前页的内容与插入内容合并暂存于内存
                DS_BPLUSTREE_KEY_TYPE temp_keys[M + 1];
                int temp_children[M + 2];

                for (j = 0; j < path_index[i]; j++)
                {
                    temp_keys[j] = ip->keys[j];
                }
                for (j = 0; j < path_index[i] + 1; j++)
                {
                    temp_children[j] = ip->children[j];
                }
                temp_keys[path_index[i]] = new_key;
                temp_children[path_index[i] + 1] = new_child;
                for (j = path_index[i]; j < ip->num_keys; j++)
                {
                    temp_keys[j + 1] = ip->keys[j];
                }
                for (j = path_index[i] + 1; j < ip->num_keys + 1; j++)
                {
                    temp_children[j + 1] = ip->children[j];
                }

                // step 2.确定新的内部节点页的页码
                int new_internal_page = alloc_page(bpt);
                if (new_internal_page < 0)
                {
                    return 0;
                }

                // step 3.对当前页的内容进行重新整理
                ip->num_keys = M / 2;

                for (j = 0; j < ip->num_keys; j++)
                {
                    ip->keys[j] = temp_keys[j];
                }
                for (j = 0; j < ip->num_keys + 1; j++)
                {
                    ip->children[j] = temp_children[j];
                }

                // step 4.记录当前已经从内存中读取到的位置（同步更新用于下一次循环的 new_key 和 new_child）
                int temp_pos_keys = ip->num_keys;
                int temp_pos_children = ip->num_keys + 1;

                new_key = temp_keys[temp_pos_keys];
                new_child = new_internal_page;

                // step 5.接下来对新的内部节点页进行设定
                char buf[4096] = {0};

                ip = (InternalPage *)buf;

                ip->page_type = PAGE_TYPE_INTERNAL;
                ip->num_keys = M - M / 2;

                for (j = 0; j < ip->num_keys; j++)
                {
                    ip->keys[j] = temp_keys[temp_pos_keys + 1 + j];
                }
                for (j = 0; j < ip->num_keys + 1; j++)
                {
                    ip->children[j] = temp_children[temp_pos_children + j];
                }

                // step 6.将新的内部节点页写入文件
                if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * new_internal_page, SEEK_SET) != 0)
                {
                    return 0;
                }
                if (fwrite(buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                {
                    return 0;
                }
            }

            // 统一写入当前页的修改
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            if (is_over == 1)
            {
                break;
            }
        }
    }

    /* 第四步——更新文件头页和B+树元数据，写入文件 */

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    fh = (FileHeaderPage *)all;

    fh->size++;
    if (is_level_add != 0)
    {
        fh->level++;
    }
    if (is_new_root_page != 0)
    {
        fh->root_page = is_new_root_page;
    }

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    // 更新B+树
    bpt->size++;
    if (is_level_add != 0)
    {
        bpt->level++;
    }
    if (is_new_root_page != 0)
    {
        bpt->root_page = is_new_root_page;
    }

    return 1;
}

int ds_bplustree_delete(DS_BPlusTree *bpt, DS_BPLUSTREE_KEY_TYPE key)
{
    /*
        **对delete函数的解读**
        非空树，按五步执行：
            第一步——从root_page开始，层层往下查找到叶子页，并记录下降路径。
            第二步——在叶子页中顺序查找，定位 target key 的位置作为删除点；若未找到则直接返回。
            第三步——逻辑删除（覆盖）：将 pos 位置之后的元素全部左移一格，num_keys--。
            第四步——处理可能的叶子下溢，分为四种情况：
                情况 1.本页剩余 key 数过半（>= (N+1)/2）或 depth == 0：写盘，结束。
                情况 2.depth == 0 且 num_keys == 0：整棵树变空，标记 is_level_decrease / is_new_root_page。
                情况 3.左借或右借：
                    (1)左兄弟盈余（num_keys > (N+1)/2）：
                        [1]暂存左兄弟最后一个key/value，左兄弟 num_keys--，左兄弟写盘；
                        [2]当前页所有元素后移一格，借来的key/value放入当前页首位，num_keys++，当前页写盘；
                        [3]更新父节点路由键 keys[path_index[depth-1] - 1] 为借来的key，父节点写盘，结束。
                    (2)右兄弟盈余：对称于左借，但借右兄弟第一个key/value，放入当前页末尾，
                        更新路由键 keys[path_index[depth-1]] 为右兄弟新首key。
                情况 4.左右借都失败，合并（可能导致上层连锁反应）：
                    (1)左合并：
                        [1]暂存当前页所有key/value；
                        [2]追加到左兄弟末尾，更新左兄弟 next_page = 当前页 next_page；
                        [3]若当前页有后继，更新其后继的 prev_page = 左兄弟；
                        [4]父节点删除路由键 keys[path_index[depth-1] - 1] 和孩子 children[path_index[depth-1]]，
                            父节点 num_keys--，若父节点因此下溢则标记 is_fa_change；
                        [5]左兄弟写盘，父节点写盘。
                    (2)右合并：对称于左合并，但右兄弟数据后移腾出开头，当前页数据放入右兄弟开头，
                        父节点删除路由键 keys[path_index[depth-1]]，更新前后链表 prev_page。
            第五步——内部节点下溢传播（is_fa_change == 1 时）：
                从 depth-1 向根逐层处理（for 循环）：
                (1)到达根（i == 0）：
                    [1]若 num_keys == 0：树降层，仅剩 child 升为新根，标记 is_level_decrease / is_new_root_page；
                    [2]否则直接写盘，结束。
                (2)非根内部节点：
                    [1]若本层未下溢（num_keys >= (M+1)/2）：写盘，结束；
                    [2]否则读父节点找左右兄弟，尝试借/合并：
                        <1>左借：借左兄弟最后一个key+child，放入当前页首位，更新祖父路由键 keys[path_index[i-1] - 1]；
                        <2>右借：借右兄弟第一个key+child，放入当前页末尾，更新祖父路由键 keys[path_index[i-1]]；
                        <3>合并：当前页内容迁入兄弟页，祖父删除路由键和孩子，本层 num_keys--，continue 继续上层循环。
            第六步——更新文件头页和B+树元数据（size--，若 is_level_decrease 则 level--，若 is_new_root_page != -1 则更新 root_page），写入文件。
    */

    if (bpt == NULL || bpt->size == 0)
    {
        return 0;
    }

    FILE *fp = bpt->file;

    int page_num = bpt->root_page;
    int page_type;

    char all[DS_BPLUSTREE_PAGE_SIZE];

    FileHeaderPage *fh;
    InternalPage *ip;
    LeafPage *lp;

    // 设定两个全局标记
    int is_level_decrease = 0; // is_level_decrease 变为非零意味着全局 level--
    int is_new_root_page = -1; // is_new_root_page 变为非-1意味着全局 root_page 变为 is_new_root_page

    /* 第一步——从root_page开始，层层往下查找到叶子页，并记录下降路径 */

    int path_pages[16]; // 下降时经过的页号
    int path_index[16]; // 对应 children[i] 中的 i
    int depth = 0;      // 路径深度（不算叶子）

    while (1)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        page_type = *(int *)all;

        if (page_type == PAGE_TYPE_LEAF)
        {
            break;
        }

        ip = (InternalPage *)all;

        int is_found = 0;

        path_pages[depth] = page_num; // 每次下降前记录

        int i = 0;
        for (i = 0; i < ip->num_keys; i++)
        {
            if (DS_BPLUSTREE_LT(key, ip->keys[i]))
            {
                page_num = ip->children[i];

                is_found = 1;

                break;
            }
        }

        if (is_found == 0)
        {
            i = ip->num_keys;

            page_num = ip->children[ip->num_keys];
        }

        path_index[depth] = i; // children[i] 就是下一步要走的
        depth++;
    }

    /* 第二步——在叶子页中顺序查找，定位 key 的位置作为删除点 */

    lp = (LeafPage *)all;

    int is_found = 0;

    int i = 0;
    for (i = 0; i < lp->num_keys; i++)
    {
        if (DS_BPLUSTREE_EQ(lp->keys[i], key))
        {
            is_found = 1;

            break;
        }
    }

    if (is_found == 0) // 若未找到，说明 key 不存在，直接返回
    {
        return 0;
    }

    // 此时的 i 就是删除位置的索引
    int pos = i; // pos 就是即将删除的位置（位置备份）

    /* 第三步——逻辑删除（覆盖）找到的位置 */

    for (i = pos; i < lp->num_keys - 1; i++)
    {
        lp->keys[i] = lp->keys[i + 1];
        lp->values[i] = lp->values[i + 1];
    }
    lp->num_keys--;

    /* 第四步——处理可能的下溢 */

    int is_fa_change = 0; // 用来标记父节点的变化是否会扩散

    int num_keys_cur = lp->num_keys;

    // 情况 1：本页剩余的节点数量过半，直接将本页写入文件
    if (num_keys_cur >= (N + 1) / 2)
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        goto done;
    }

    // 情况 2：自己没有左右兄弟（自己是唯一一个叶子节点页，depth == 0）
    if (depth == 0)
    {
        if (num_keys_cur == 0)
        {
            is_level_decrease = 1;
            is_new_root_page = 0;

            if (free_page(bpt, page_num) != 1)
            {
                return 0;
            }
        }
        else // 与情况 1.相同处理方式，直接将本页写入文件
        {
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }
        }

        goto done;
    }

    // 情况 3：先尝试向左右兄弟借节点
    int leaf_left_bro = lp->prev_page;
    int leaf_right_bro = lp->next_page;

    // 当前页内容一直保存于 all 中
    char bro_buf[4096]; // 用来暂存当前页的兄弟页内容
    char fa_buf[4096];  // 用来暂存当前页的父节点页内容

    DS_BPLUSTREE_KEY_TYPE temp_key;
    DS_BPLUSTREE_VALUE_TYPE temp_value;

    if (leaf_left_bro != 0 && path_index[depth - 1] > 0) // 先尝试向左兄弟借（想要“左借”，必须满足两个情况：有左邻居，且自己不能是父节点页的第一个孩子）
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_left_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        lp = (LeafPage *)bro_buf;

        if (lp->num_keys > (N + 1) / 2)
        {
            // step 1.暂存左兄弟的最后一个节点内容，左兄弟节点数量-1，并将左兄弟页写入文件
            temp_key = lp->keys[lp->num_keys - 1];
            temp_value = lp->values[lp->num_keys - 1];

            lp->num_keys--;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_left_bro, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            // step 2.将本页所有节点后移一格，将暂存的节点当作本页第一个节点，本页节点数量+1，并将本页写入文件
            lp = (LeafPage *)all;

            for (i = lp->num_keys - 1; i >= 0; i--)
            {
                lp->keys[i + 1] = lp->keys[i];
                lp->values[i + 1] = lp->values[i];
            }

            lp->keys[0] = temp_key;
            lp->values[0] = temp_value;

            lp->num_keys++;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            // step 3.更新父节点中分隔这两个叶子的路由 key（= 本页的最小 key，即temp_key），并将父节点页写入文件
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            ip = (InternalPage *)fa_buf;

            ip->keys[path_index[depth - 1] - 1] = temp_key;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            goto done;
        }
    }

    // 增加一步：先读取父节点 num_keys 值，用于防止越界访问（用于右借和右合并）
    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
    {
        return 0;
    }
    if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    int parent_num_keys = ((InternalPage *)fa_buf)->num_keys;

    if (leaf_right_bro != 0 && path_index[depth - 1] < parent_num_keys) // 再尝试向右兄弟借（想要“右借”，必须满足两个情况：有右邻居，且自己不能是父节点的最后一个孩子）
    {
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_right_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        lp = (LeafPage *)bro_buf;

        DS_BPLUSTREE_KEY_TYPE new_route_key;

        if (lp->num_keys > (N + 1) / 2)
        {
            // step 1.暂存右兄弟的第一个节点内容，将右兄弟所有节点前移一格，暂存右兄弟新的第一个key，右兄弟节点数量-1，并将右兄弟页写入文件
            temp_key = lp->keys[0];
            temp_value = lp->values[0];

            for (i = 1; i < lp->num_keys; i++)
            {
                lp->keys[i - 1] = lp->keys[i];
                lp->values[i - 1] = lp->values[i];
            }

            new_route_key = lp->keys[0];

            lp->num_keys--;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_right_bro, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            // step 2.将暂存的节点当作本页最后一个节点，本页节点数量+1，并将本页写入文件
            lp = (LeafPage *)all;

            lp->keys[lp->num_keys] = temp_key;
            lp->values[lp->num_keys] = temp_value;

            lp->num_keys++;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * page_num, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            // step 3.更新父节点中分隔这两个叶子的路由 key（= 右兄弟的最小 key，即new_route_key），并将父节点页写入文件
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            ip = (InternalPage *)fa_buf;

            ip->keys[path_index[depth - 1]] = new_route_key;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            goto done;
        }
    }

    // 情况 4：左右兄弟都借不到，尝试与左右兄弟合并（可能导致上层连锁反应）
    DS_BPLUSTREE_KEY_TYPE temp_keys[N];
    DS_BPLUSTREE_VALUE_TYPE temp_values[N];

    if (leaf_left_bro != 0 && path_index[depth - 1] > 0) // 先尝试与左兄弟合并（想要“左合并”，必须满足两个情况：有左邻居，且自己不能是父节点页的第一个孩子）
    {
        // step 1.把当前页的内容暂存至内存，并把当前页废弃
        lp = (LeafPage *)all;

        int temp_pos = lp->num_keys;
        int temp_next_page = lp->next_page;

        for (i = 0; i < temp_pos; i++)
        {
            temp_keys[i] = lp->keys[i];
            temp_values[i] = lp->values[i];
        }

        if (free_page(bpt, page_num) != 1)
        {
            return 0;
        }

        // step 2.把暂存的所有内容追加到左兄弟末尾，左兄弟节点数量 +temp_pos，左兄弟的 next_page 更新为当前页的 next_page ，并将左兄弟页写入文件
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_left_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        lp = (LeafPage *)bro_buf;

        for (i = 0; i < temp_pos; i++)
        {
            lp->keys[i + lp->num_keys] = temp_keys[i];
            lp->values[i + lp->num_keys] = temp_values[i];
        }

        lp->num_keys += temp_pos;
        lp->next_page = temp_next_page;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_left_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // step 3.若当前页的 next_page 存在，那么更新其 prev_page
        if (temp_next_page != 0)
        {
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_next_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            lp = (LeafPage *)all;

            lp->prev_page = leaf_left_bro;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_next_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }
        }

        // step 4.父节点页删除对于当前页的路由键，根据父节点页的键数判断标记，父节点页节点数-1，并将父节点页写入文件
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        ip = (InternalPage *)fa_buf;

        for (i = path_index[depth - 1] - 1; i < ip->num_keys - 1; i++)
        {
            ip->keys[i] = ip->keys[i + 1];
        }
        for (i = path_index[depth - 1]; i < ip->num_keys; i++)
        {
            ip->children[i] = ip->children[i + 1];
        }

        ip->num_keys--;

        if (ip->num_keys < (M + 1) / 2)
        {
            is_fa_change = 1;
        }

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        goto done;
    }

    if (leaf_right_bro != 0 && path_index[depth - 1] < parent_num_keys) // 再尝试与右兄弟合并（想要“右合并”，必须满足两个情况：有右邻居，且自己不能是父节点的最后一个孩子）
    {
        // step 1.把当前页的内容暂存至内存，并把当前页废弃
        lp = (LeafPage *)all;

        int temp_pos = lp->num_keys;
        int temp_prev_page = lp->prev_page;

        for (i = 0; i < temp_pos; i++)
        {
            temp_keys[i] = lp->keys[i];
            temp_values[i] = lp->values[i];
        }

        if (free_page(bpt, page_num) != 1)
        {
            return 0;
        }

        // step 2.右兄弟所有 key/value 后移 temp_pos 格，腾出前面空间，把暂存的所有内容放到右兄弟开头，右兄弟节点数量 +temp_pos，右兄弟的 prev_page 更新为当前页的 prev_page ，并将右兄弟页写入文件
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_right_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        lp = (LeafPage *)bro_buf;

        for (i = lp->num_keys - 1; i >= 0; i--)
        {
            lp->keys[i + temp_pos] = lp->keys[i];
            lp->values[i + temp_pos] = lp->values[i];
        }
        for (i = 0; i < temp_pos; i++)
        {
            lp->keys[i] = temp_keys[i];
            lp->values[i] = temp_values[i];
        }

        lp->num_keys += temp_pos;
        lp->prev_page = temp_prev_page;

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * leaf_right_bro, SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        // step 3.若当前页的 prev_page 存在，那么更新其 prev_page
        if (temp_prev_page != 0)
        {
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_prev_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            lp = (LeafPage *)all;

            lp->next_page = leaf_right_bro;

            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * temp_prev_page, SEEK_SET) != 0)
            {
                return 0;
            }
            if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }
        }

        // step 4.父节点页删除对于当前页的路由键，根据父节点页的键数判断标记，父节点页节点数-1，并将父节点页写入文件
        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
        {
            return 0;
        }
        if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        ip = (InternalPage *)fa_buf;

        for (i = path_index[depth - 1]; i < ip->num_keys - 1; i++)
        {
            ip->keys[i] = ip->keys[i + 1];
        }
        for (i = path_index[depth - 1]; i < ip->num_keys; i++)
        {
            ip->children[i] = ip->children[i + 1];
        }

        ip->num_keys--;

        if (ip->num_keys < (M + 1) / 2)
        {
            is_fa_change = 1;
        }

        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[depth - 1], SEEK_SET) != 0)
        {
            return 0;
        }
        if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
        {
            return 0;
        }

        goto done;
    }

done:;
    /* 第五步——内部节点下溢传播（is_fa_change == 1 时）*/

    int temp_child;

    if (is_fa_change == 1)
    {
        for (i = depth - 1; i >= 0; i--)
        {
            if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
            {
                return 0;
            }
            if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
            {
                return 0;
            }

            ip = (InternalPage *)all;

            if (i == 0) // 若当前内部节点就是 root_page
            {
                if (ip->num_keys == 0) // 若因为合并导致根页丢失了所有 keys，那么仅剩的 child（即children[0]）将作为新根，原来的根页被废弃
                {
                    is_level_decrease = 1;
                    is_new_root_page = ip->children[0];

                    if (free_page(bpt, path_pages[i]) != 1)
                    {
                        return 0;
                    }
                }
                else // (ip->num_keys > 0) 根页可以有任意数量的 key，直接写入文件，结束
                {
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }
                }

                break;
            }
            else
            {
                // 先判断当前内部节点是否真的下溢
                if (ip->num_keys >= (M + 1) / 2)
                {
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    break;
                }

                // 读取目前的内部节点页的父节点页，找到当前页的左右兄弟节点页
                if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                {
                    return 0;
                }
                if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                {
                    return 0;
                }

                ip = (InternalPage *)fa_buf;

                int internal_left_bro = (path_index[i - 1] == 0 ? 0 : ip->children[path_index[i - 1] - 1]);
                int internal_right_bro = (path_index[i - 1] == ip->num_keys ? 0 : ip->children[path_index[i - 1] + 1]);

                // 情况 1：先尝试向左右兄弟借节点
                if (internal_left_bro != 0) // 先尝试向左兄弟借
                {
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_left_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)bro_buf;

                    if (ip->num_keys > (M + 1) / 2)
                    {
                        // step 1.暂存左兄弟的最后一个节点内容，左兄弟节点数量-1，并将左兄弟页写入文件
                        temp_key = ip->keys[ip->num_keys - 1];
                        temp_child = ip->children[ip->num_keys];

                        ip->num_keys--;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_left_bro, SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        // step 2.将本页所有节点后移一格，将暂存的节点当作本页第一个节点，本页节点数量+1，并将本页写入文件
                        ip = (InternalPage *)all;

                        int j;

                        for (j = ip->num_keys - 1; j >= 0; j--)
                        {
                            ip->keys[j + 1] = ip->keys[j];
                        }
                        for (j = ip->num_keys; j >= 0; j--)
                        {
                            ip->children[j + 1] = ip->children[j];
                        }

                        ip->keys[0] = temp_key;
                        ip->children[0] = temp_child;

                        ip->num_keys++;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        // step 3.更新父节点中分隔这两个叶子的路由 key（= 本页的最小 key，即temp_key），并将父节点页写入文件
                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        ip = (InternalPage *)fa_buf;

                        ip->keys[path_index[i - 1] - 1] = temp_key;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        break;
                    }
                }

                if (internal_right_bro != 0) // 再尝试向右兄弟借
                {
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_right_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    DS_BPLUSTREE_KEY_TYPE new_route_key;

                    ip = (InternalPage *)bro_buf;

                    if (ip->num_keys > (M + 1) / 2)
                    {
                        // step 1.暂存右兄弟的第一个节点内容，将右兄弟所有节点前移一格，暂存右兄弟新的第一个key 和 child，右兄弟节点数量-1，并将右兄弟页写入文件
                        temp_key = ip->keys[0];
                        temp_child = ip->children[0];

                        int j;

                        for (j = 1; j < ip->num_keys; j++)
                        {
                            ip->keys[j - 1] = ip->keys[j];
                        }
                        for (j = 1; j <= ip->num_keys; j++)
                        {
                            ip->children[j - 1] = ip->children[j];
                        }

                        new_route_key = ip->keys[0];

                        ip->num_keys--;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_right_bro, SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        // step 2.将暂存的节点当作本页最后一个节点，本页节点数量+1，并将本页写入文件
                        ip = (InternalPage *)all;

                        ip->keys[ip->num_keys] = temp_key;
                        ip->children[ip->num_keys + 1] = temp_child;

                        ip->num_keys++;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        // step 3.更新父节点中分隔这两个叶子的路由 key（= 右兄弟的最小 key，即new_route_key），并将父节点页写入文件
                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        ip = (InternalPage *)fa_buf;

                        ip->keys[path_index[i - 1]] = new_route_key;

                        if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                        {
                            return 0;
                        }
                        if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                        {
                            return 0;
                        }

                        break;
                    }
                }

                // 情况 2：左右兄弟都借不到，尝试与左右兄弟合并（可能导致上层连锁反应）
                int temp_children[M + 1];

                if (internal_left_bro != 0) // 先尝试与左兄弟合并
                {
                    // step 1.把当前页的内容暂存至内存，并把当前页废弃
                    ip = (InternalPage *)all;

                    int temp_pos = ip->num_keys;

                    int j;

                    for (j = 0; j < temp_pos; j++)
                    {
                        temp_keys[j] = ip->keys[j];
                    }
                    for (j = 0; j <= temp_pos; j++)
                    {
                        temp_children[j] = ip->children[j];
                    }

                    if (free_page(bpt, path_pages[i]) != 1)
                    {
                        return 0;
                    }

                    // step 2.先读取父节点，将父节点对于当前页的路由键暂存
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)fa_buf;

                    temp_key = ip->keys[path_index[i - 1] - 1];

                    // step 3.读取左兄弟节点，先把暂存的 temp_key 插入至左兄弟末尾，然后把暂存的所有内容追加到左兄弟末尾，左兄弟节点数量 +temp_pos+1 ，并将左兄弟页写入文件
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_left_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)bro_buf;

                    ip->keys[ip->num_keys] = temp_key;

                    for (j = 0; j < temp_pos; j++)
                    {
                        ip->keys[j + ip->num_keys + 1] = temp_keys[j];
                    }
                    for (j = 0; j <= temp_pos; j++)
                    {
                        ip->children[j + ip->num_keys + 1] = temp_children[j];
                    }

                    ip->num_keys += (temp_pos + 1);

                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_left_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    // step 4.再次读取父节点，父节点页删除对于当前页的路由键，父节点页节点数-1，并将父节点页写入文件
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)fa_buf;

                    for (j = path_index[i - 1] - 1; j < ip->num_keys - 1; j++)
                    {
                        ip->keys[j] = ip->keys[j + 1];
                    }
                    for (j = path_index[i - 1]; j < ip->num_keys; j++)
                    {
                        ip->children[j] = ip->children[j + 1];
                    }

                    ip->num_keys--;

                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    continue;
                }

                if (internal_right_bro != 0) // 再尝试与右兄弟合并
                {
                    // step 1.把当前页的内容暂存至内存，并把当前页废弃
                    ip = (InternalPage *)all;

                    int temp_pos = ip->num_keys;

                    int j;

                    for (j = 0; j < temp_pos; j++)
                    {
                        temp_keys[j] = ip->keys[j];
                    }
                    for (j = 0; j <= temp_pos; j++)
                    {
                        temp_children[j] = ip->children[j];
                    }

                    if (free_page(bpt, path_pages[i]) != 1)
                    {
                        return 0;
                    }

                    // step 2.先读取父节点，将父节点对于当前页的路由键暂存
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)fa_buf;

                    temp_key = ip->keys[path_index[i - 1]];

                    // step 3.右兄弟所有 key/value 后移 temp_pos+1 格，腾出前面空间，先把暂存的 temp_key 插入至右兄弟当前的开头，然后把暂存的所有内容放到右兄弟开头，右兄弟节点数量 +temp_pos+1，并将右兄弟页写入文件
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_right_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)bro_buf;

                    for (j = ip->num_keys - 1; j >= 0; j--)
                    {
                        ip->keys[j + temp_pos + 1] = ip->keys[j];
                    }
                    for (j = ip->num_keys; j >= 0; j--)
                    {
                        ip->children[j + temp_pos + 1] = ip->children[j];
                    }

                    ip->keys[temp_pos] = temp_key;

                    for (j = 0; j < temp_pos; j++)
                    {
                        ip->keys[j] = temp_keys[j];
                    }
                    for (j = 0; j <= temp_pos; j++)
                    {
                        ip->children[j] = temp_children[j];
                    }

                    ip->num_keys += (temp_pos + 1);

                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * internal_right_bro, SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(bro_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    // step 4.再次读取父节点，父节点页删除对于当前页的路由键，根据父节点页的键数判断标记，父节点页节点数-1，并将父节点页写入文件
                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fread(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    ip = (InternalPage *)fa_buf;

                    for (j = path_index[i - 1]; j < ip->num_keys - 1; j++)
                    {
                        ip->keys[j] = ip->keys[j + 1];
                    }
                    for (j = path_index[i - 1]; j < ip->num_keys; j++)
                    {
                        ip->children[j] = ip->children[j + 1];
                    }

                    ip->num_keys--;

                    if (fseek(fp, DS_BPLUSTREE_PAGE_SIZE * path_pages[i - 1], SEEK_SET) != 0)
                    {
                        return 0;
                    }
                    if (fwrite(fa_buf, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
                    {
                        return 0;
                    }

                    continue;
                }
            }
        }
    }

    /* 第六步——更新文件头页和B+树元数据，写入文件 */

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fread(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    fh = (FileHeaderPage *)all;

    fh->size--;
    if (is_level_decrease != 0)
    {
        fh->level--;
    }
    if (is_new_root_page != -1)
    {
        fh->root_page = is_new_root_page;
    }

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        return 0;
    }
    if (fwrite(all, DS_BPLUSTREE_PAGE_SIZE, 1, fp) != 1)
    {
        return 0;
    }

    // 更新B+树
    bpt->size--;
    if (is_level_decrease != 0)
    {
        bpt->level--;
    }
    if (is_new_root_page != -1)
    {
        bpt->root_page = is_new_root_page;
    }

    return 1;
}
