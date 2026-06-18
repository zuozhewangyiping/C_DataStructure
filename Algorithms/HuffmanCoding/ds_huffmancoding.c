#include <stdlib.h>
#include "ds_huffmancoding.h"

typedef struct HuffmanTreeNode
{
    unsigned char symbol; // 用于给叶子节点存原字符，内部节点无意义
    int freq;             // 该原字符的权重
    struct HuffmanTreeNode *left;
    struct HuffmanTreeNode *right;

} HuffmanTreeNode;

struct DS_HuffmanCoding
{
    HuffmanTreeNode *root;        // 哈夫曼树树根
    unsigned long long code[256]; // 用来储存所有字符的压缩结果
    int code_bit_length[256];     // 用来储存所有字符的压缩结果比特数
    int original_length;
};

/* Part 0. Inner Heap helpers -----------------------------------------------*/

#define DS_HEAP_TYPE HuffmanTreeNode *

typedef struct DS_Heap
{
    DS_HEAP_TYPE *data;
    int size;
    int capacity;
} DS_Heap;

static DS_Heap *ds_heap_create(void);
static void ds_heap_destroy(DS_Heap *h);
static int ds_heap_push(DS_Heap *h, DS_HEAP_TYPE value);
static int ds_heap_pop(DS_Heap *h, DS_HEAP_TYPE *x);

static DS_Heap *ds_heap_create(void)
{
    DS_Heap *h = (DS_Heap *)malloc(sizeof(DS_Heap));
    if (h == NULL)
    {
        return NULL;
    }

    h->data = NULL;
    h->capacity = 0;
    h->size = 0;

    return h;
}

static void free_tree(HuffmanTreeNode *node)
{
    if (node == NULL)
    {
        return;
    }

    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

static void ds_heap_destroy(DS_Heap *h)
{
    if (h == NULL)
    {
        return;
    }

    for (int i = 0; i < h->size; i++)
    {
        free_tree(h->data[i]); // 递归释放每个节点及其子树
    }

    free(h->data);
    free(h);
}

static int capacity_expansion(DS_Heap *h)
{
    if (h == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (h->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * h->capacity;
    }

    DS_HEAP_TYPE *temp = (DS_HEAP_TYPE *)realloc(h->data, new_capacity * sizeof(DS_HEAP_TYPE));
    if (temp == NULL)
    {
        return 0;
    }

    h->data = temp;
    h->capacity = new_capacity;

    return 1;
}

static int ds_heap_push(DS_Heap *h, DS_HEAP_TYPE value)
{
    if (h == NULL)
    {
        return 0;
    }

    if (h->size >= h->capacity)
    {
        if (!capacity_expansion(h))
        {
            return 0;
        }
    }

    h->data[h->size] = value;

    h->size++;

    int i = h->size - 1;
    while (i > 0 && h->data[(i - 1) / 2]->freq > h->data[i]->freq)
    {
        DS_HEAP_TYPE temp = h->data[i];
        h->data[i] = h->data[(i - 1) / 2];
        h->data[(i - 1) / 2] = temp;

        i = (i - 1) / 2;
    }

    return 1;
}

static int ds_heap_pop(DS_Heap *h, DS_HEAP_TYPE *x)
{
    if (h == NULL || x == NULL || h->size == 0)
    {
        return 0;
    }

    DS_HEAP_TYPE temp = h->data[0];

    h->data[0] = h->data[h->size - 1];

    h->data[h->size - 1] = temp;

    *x = h->data[h->size - 1];

    h->size--;

    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left >= h->size)
        {
            break;
        }

        int child;
        if (right >= h->size)
        {
            child = left;
        }
        else
        {
            if (h->data[right]->freq < h->data[left]->freq)
            {
                child = right;
            }
            else
            {
                child = left;
            }
        }

        if (h->data[i]->freq <= h->data[child]->freq)
        {
            break;
        }

        DS_HEAP_TYPE temp_ = h->data[i];
        h->data[i] = h->data[child];
        h->data[child] = temp_;

        i = child;
    }

    return 1;
}

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_HuffmanCoding *ds_huffmancoding_create(void)
{
    DS_HuffmanCoding *hc = (DS_HuffmanCoding *)malloc(sizeof(DS_HuffmanCoding));
    if (hc == NULL)
    {
        return NULL;
    }

    hc->root = NULL;
    hc->original_length = 0;

    int i = 0;
    for (i = 0; i < 256; i++)
    {
        hc->code[i] = 0;
        hc->code_bit_length[i] = 0;
    }

    return hc;
}

void ds_huffmancoding_destroy(DS_HuffmanCoding *hc)
{
    if (hc == NULL)
    {
        return;
    }

    if (hc->root != NULL)
    {
        free_tree(hc->root);
    }

    free(hc);
}

/* Part 2. Build ------------------------------------------------------------*/

static HuffmanTreeNode *create_new_node(unsigned char symbol, int freq)
{
    HuffmanTreeNode *new_node = (HuffmanTreeNode *)malloc(sizeof(HuffmanTreeNode));
    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->symbol = symbol;
    new_node->freq = freq;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

static void code_generator(DS_HuffmanCoding *hc, HuffmanTreeNode *node, unsigned long long code, int depth)
{
    if (node == NULL)
    {
        return;
    }

    if (node->left == NULL && node->right == NULL)
    {
        hc->code[node->symbol] = code;
        hc->code_bit_length[node->symbol] = depth;
        return;
    }

    code_generator(hc, node->left, code * 2, depth + 1);
    code_generator(hc, node->right, code * 2 + 1, depth + 1);
}

int ds_huffmancoding_build(DS_HuffmanCoding *hc, const unsigned char *user_data, int user_data_length)
{
    if (hc == NULL || user_data == NULL)
    {
        return 0;
    }

    if (hc->root != NULL) // 若发现用户重复调用 build 函数，就将上次的作废（直接重置hc）
    {
        free_tree(hc->root);
        hc->root = NULL;

        for (int i = 0; i < 256; i++)
        {
            hc->code[i] = 0;
            hc->code_bit_length[i] = 0;
        }

        hc->original_length = 0;
    }

    int freq[256] = {0};

    // step 1.统计权重
    int i;
    for (i = 0; i < user_data_length; i++)
    {
        freq[user_data[i]]++;
    }

    DS_Heap *h = ds_heap_create();
    if (h == NULL)
    {
        return 0;
    }

    // step 2.为每个非零权重字符创建叶子节点→入堆
    for (i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            HuffmanTreeNode *new_node = create_new_node((unsigned char)i, freq[i]);
            if (new_node == NULL)
            {
                ds_heap_destroy(h);
                return 0;
            }

            if (!ds_heap_push(h, new_node))
            {
                free(new_node);
                ds_heap_destroy(h);
                return 0;
            }
        }
    }

    if (h->size == 1) // 特殊情况：如果只有一种字符
    {
        HuffmanTreeNode *only = NULL;
        if (!ds_heap_pop(h, &only))
        {
            ds_heap_destroy(h);
            return 0;
        }

        hc->code[only->symbol] = 0;
        hc->code_bit_length[only->symbol] = 1;
        hc->root = only;
        hc->original_length = user_data_length;

        ds_heap_destroy(h);
        return 1;
    }

    // step 3.构建哈夫曼编码树
    while (h->size > 1)
    {
        HuffmanTreeNode *node_1 = NULL, *node_2 = NULL;

        if (!ds_heap_pop(h, &node_1) || !ds_heap_pop(h, &node_2))
        {
            free_tree(node_1);
            free_tree(node_2);
            ds_heap_destroy(h);
            return 0;
        }

        HuffmanTreeNode *parent = create_new_node((unsigned char)0, node_1->freq + node_2->freq);
        if (parent == NULL)
        {
            free_tree(node_1);
            free_tree(node_2);
            ds_heap_destroy(h);
            return 0;
        }

        parent->left = node_1;
        parent->right = node_2;

        if (!ds_heap_push(h, parent))
        {
            free_tree(parent);
            ds_heap_destroy(h);
            return 0;
        }
    }

    HuffmanTreeNode *root = NULL;

    if (!ds_heap_pop(h, &root))
    {
        ds_heap_destroy(h);
        return 0;
    }

    ds_heap_destroy(h);

    hc->root = root; // 树已成功构建，树根已被保存

    // step 4.遍历树并累积比特到叶子时填 code_table
    code_generator(hc, hc->root, 0, 0);

    hc->original_length = user_data_length;

    return 1;
}

/* Part 3. Encode & Decode --------------------------------------------------*/

int ds_huffmancoding_encode(DS_HuffmanCoding *hc,
                            const unsigned char *uncompressed_data, int uncompressed_data_length,
                            unsigned char **compressed_data, int *compressed_data_length)
{
    if (hc == NULL || uncompressed_data == NULL || compressed_data == NULL || compressed_data_length == NULL || hc->root == NULL)
    {
        return 0;
    }

    // step 1.累加计算出uncompressed_data经过压缩后占用字节数
    int total_bytes = 0;
    int i = 0;
    for (i = 0; i < uncompressed_data_length; i++)
    {
        total_bytes += hc->code_bit_length[uncompressed_data[i]];
    }

    total_bytes = (total_bytes + 7) / 8;

    // step 2.按照计算出的大小申请内存
    unsigned char *out = (unsigned char *)malloc(total_bytes * sizeof(unsigned char));
    if (out == NULL)
    {
        return 0;
    }
    int out_pos = 0; // 创建游标

    // step 3.将压缩后的数据传入out
    unsigned char buf = 0;
    int buf_length = 0;
    for (i = 0; i < uncompressed_data_length; i++)
    {
        unsigned long long cur = hc->code[uncompressed_data[i]];
        int cur_bit_length = hc->code_bit_length[uncompressed_data[i]];

        if (cur_bit_length == 0) // 检查：如果在用户传入的数据中发现了 build 时不存在的字符，直接退出（用户违反了API规定）
        {
            free(out);
            return 0;
        }

        int temp[256];
        int temp_length = 0;

        int j = 0;
        for (j = 0; j < cur_bit_length; j++)
        {
            temp[j] = cur % 2;
            temp_length++;
            cur /= 2;
        }

        for (j = temp_length - 1; j >= 0; j--)
        {
            buf = buf * 2 + temp[j];
            buf_length++;

            if (buf_length == 8)
            {
                out[out_pos] = buf;
                out_pos++;

                buf = 0;
                buf_length = 0;
            }
        }
    }

    if (buf_length > 0)
    {
        for (i = 0; i < 8 - buf_length; i++)
        {
            buf *= 2;
            buf_length++;
        }

        out[out_pos] = buf;
        out_pos++;
    }

    // step 4.将结果传出函数
    *compressed_data = out;
    *compressed_data_length = total_bytes;

    return 1;
}

int ds_huffmancoding_decode(DS_HuffmanCoding *hc,
                            const unsigned char *compressed_data, int compressed_data_length,
                            unsigned char **uncompressed_data, int *uncompressed_data_length)
{
    if (hc == NULL || compressed_data == NULL || uncompressed_data == NULL || uncompressed_data_length == NULL || hc->root == NULL)
    {
        return 0;
    }

    // step 1.按照原始大小申请内存
    unsigned char *out = (unsigned char *)malloc(hc->original_length * sizeof(unsigned char));
    if (out == NULL)
    {
        return 0;
    }
    int out_pos = 0; // 创建游标

    if (hc->root->left == NULL && hc->root->right == NULL) // 特殊情况：如果只有一种字符（导致树根即为叶子节点，无法走树）
    {
        int i = 0;
        for (i = 0; i < hc->original_length; i++)
        {
            out[i] = hc->root->symbol;
        }

        *uncompressed_data = out;
        *uncompressed_data_length = hc->original_length;

        return 1;
    }

    // step 2.一边解压数据，一边走树解码
    int compressed_data_cnt = 0;     // 用来记录下一次该解压的位置
    HuffmanTreeNode *cur = hc->root; // 用来记录目前走树的位置

    while (out_pos < hc->original_length && compressed_data_cnt < compressed_data_length)
    {
        unsigned char temp = compressed_data[compressed_data_cnt];
        compressed_data_cnt++;

        int tmp[8]; // 用来临时储存目前字节的8个比特值

        int i = 0;
        for (i = 0; i < 8; i++)
        {
            tmp[i] = temp % 2;
            temp /= 2;
        }

        for (i = 7; i >= 0; i--)
        {
            if (tmp[i] == 0)
            {
                cur = cur->left;
            }
            else
            {
                cur = cur->right;
            }

            if (cur->left == NULL && cur->right == NULL)
            {
                out[out_pos] = cur->symbol;
                out_pos++;

                cur = hc->root;

                if (out_pos >= hc->original_length) // 检查是否已经解压完毕（防止 out 溢出）
                {
                    break;
                }
            }
        }
    }

    // step 3.将结果传出函数
    *uncompressed_data = out;
    *uncompressed_data_length = out_pos; // 并非直接传出 hc->original_length ，给了用户检查的机会

    return 1;
}
