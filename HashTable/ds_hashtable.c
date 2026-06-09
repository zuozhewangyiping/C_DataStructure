#include <stdlib.h>
#include "ds_hashtable.h"

typedef struct HashNode
{
    DS_HASHTABLE_TYPE data;
    struct HashNode *next;
} HashNode;

struct DS_HashTable
{
    HashNode **buckets;
    int size;
    int capacity;
};

DS_HashTable *ds_hashtable_create(void)
{
    DS_HashTable *ht = (DS_HashTable *)malloc(sizeof(DS_HashTable));
    if (ht == NULL)
    {
        return NULL;
    }

    ht->buckets = NULL;
    ht->capacity = 0;
    ht->size = 0;

    return ht;
}

void ds_hashtable_destroy(DS_HashTable *ht)
{
    if (ht == NULL)
    {
        return;
    }

    int i;
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *cur = ht->buckets[i];

        while (cur != NULL)
        {
            HashNode *temp = cur;
            cur = cur->next;

            DS_HASHTABLE_DESTROY_ELEMENT(temp->data);
            free(temp);
        }
    }

    free(ht->buckets);
    free(ht);
}

int ds_hashtable_traverse(DS_HashTable *ht, void *user_data, void (*visit)(DS_HASHTABLE_TYPE *value, void *call_back))
{
    if (ht == NULL || visit == NULL)
    {
        return 0;
    }

    if (ht->buckets == NULL)
    {
        return 1;
    }

    int i;
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *cur = ht->buckets[i];

        while (cur != NULL)
        {
            visit(&(cur->data), user_data);

            cur = cur->next;
        }
    }

    return 1;
}

int ds_hashtable_size(DS_HashTable *ht)
{
    if (ht == NULL)
    {
        return -1;
    }

    return ht->size;
}

int ds_hashtable_capacity(DS_HashTable *ht)
{
    if (ht == NULL)
    {
        return -1;
    }

    return ht->capacity;
}

int ds_hashtable_is_empty(DS_HashTable *ht)
{
    if (ht == NULL)
    {
        return -1;
    }

    return (ht->size == 0 ? 1 : 0);
}

int ds_hashtable_find(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target, DS_HASHTABLE_TYPE **x)
{
    if (ht == NULL || x == NULL || ht->buckets == NULL)
    {
        return 0;
    }

    int hash_index = DS_HASHTABLE_HASH_KEY(target, ht->capacity);

    HashNode *cur = ht->buckets[hash_index];

    while (cur != NULL)
    {
        if (DS_HASHTABLE_MATCH(cur->data, target))
        {
            *x = &(cur->data);

            return 1;
        }

        cur = cur->next;
    }

    return 0;
}

static int capacity_expansion(DS_HashTable *ht)
{
    if (ht == NULL)
    {
        return 0;
    }

    int new_capacity;
    if (ht->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * ht->capacity;
    }

    HashNode **new_buckets = (HashNode **)malloc(new_capacity * sizeof(HashNode *));
    if (new_buckets == NULL)
    {
        return 0;
    }

    int i;
    for (i = 0; i < new_capacity; i++)
    {
        new_buckets[i] = NULL;
    }

    for (i = 0; i < ht->capacity; i++)
    {
        if (ht->buckets[i] != NULL)
        {
            HashNode *cur = ht->buckets[i];

            while (cur != NULL)
            {
                HashNode *temp = cur;
                cur = cur->next;

                int hash_index = DS_HASHTABLE_HASH(temp->data, new_capacity);

                if (new_buckets[hash_index] == NULL)
                {
                    new_buckets[hash_index] = temp;
                    temp->next = NULL;
                }
                else
                {
                    HashNode *cur_ = new_buckets[hash_index];

                    new_buckets[hash_index] = temp;
                    temp->next = cur_;
                }
            }
        }
    }

    free(ht->buckets);

    ht->buckets = new_buckets;
    ht->capacity = new_capacity;

    return 1;
}

static HashNode *create_Node(DS_HASHTABLE_TYPE value)
{
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL)
    {
        return NULL;
    }

    // step 1.深度CLONE到临时变量temp
    int judge = 1;
    DS_HASHTABLE_TYPE temp = DS_HASHTABLE_CLONE_ELEMENT(value, &judge);
    if (judge == 0)
    {
        DS_HASHTABLE_DESTROY_ELEMENT(temp);
        free(new_node);
        return NULL;
    }

    // step 2.新data放入该节点
    new_node->data = temp;

    new_node->next = NULL;

    return new_node;
}

int ds_hashtable_insert(DS_HashTable *ht, DS_HASHTABLE_TYPE value)
{
    if (ht == NULL)
    {
        return 0;
    }

    if (ht->size >= ht->capacity)
    {
        if (!capacity_expansion(ht))
        {
            return 0;
        }
    }

    int hash_index = DS_HASHTABLE_HASH(value, ht->capacity);

    HashNode *cur = ht->buckets[hash_index];
    while (cur != NULL)
    {
        if (DS_HASHTABLE_MATCH_KEY(cur->data, value))
        {
            return 0;
        }

        cur = cur->next;
    }

    HashNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    HashNode *temp = ht->buckets[hash_index];

    ht->buckets[hash_index] = new_node;
    new_node->next = temp;

    ht->size++;

    return 1;
}

int ds_hashtable_put(DS_HashTable *ht, DS_HASHTABLE_TYPE value)
{
    if (ht == NULL)
    {
        return 0;
    }

    if (ht->size >= ht->capacity)
    {
        if (!capacity_expansion(ht))
        {
            return 0;
        }
    }

    int hash_index = DS_HASHTABLE_HASH(value, ht->capacity);

    HashNode *cur = ht->buckets[hash_index];
    while (cur != NULL)
    {
        if (DS_HASHTABLE_MATCH_KEY(cur->data, value))
        {
            int judge = 1;
            DS_HASHTABLE_TYPE new_data = DS_HASHTABLE_CLONE_ELEMENT(value, &judge);
            if (judge == 0)
            {
                DS_HASHTABLE_DESTROY_ELEMENT(new_data);
                return 0;
            }

            DS_HASHTABLE_DESTROY_ELEMENT(cur->data);

            cur->data = new_data;

            return 1;
        }

        cur = cur->next;
    }

    HashNode *new_node = create_Node(value);
    if (new_node == NULL)
    {
        return 0;
    }

    HashNode *temp = ht->buckets[hash_index];

    ht->buckets[hash_index] = new_node;
    new_node->next = temp;

    ht->size++;

    return 1;
}

int ds_hashtable_erase(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target, DS_HASHTABLE_TYPE **x)
{
    if (ht == NULL || x == NULL || ht->buckets == NULL)
    {
        return 0;
    }

    int hash_index = DS_HASHTABLE_HASH_KEY(target, ht->capacity);

    HashNode *cur_1 = ht->buckets[hash_index];
    HashNode *cur_2 = NULL;

    while (cur_1 != NULL)
    {
        if (DS_HASHTABLE_MATCH(cur_1->data, target))
        {
            DS_HASHTABLE_TYPE *erased = (DS_HASHTABLE_TYPE *)malloc(sizeof(DS_HASHTABLE_TYPE));
            if (erased == NULL)
            {
                return 0;
            }

            if (cur_2 == NULL)
            {
                ht->buckets[hash_index] = cur_1->next;
            }
            else
            {
                cur_2->next = cur_1->next;
            }

            *erased = cur_1->data;
            *x = erased;
            free(cur_1);

            ht->size--;

            return 1;
        }

        cur_2 = cur_1;
        cur_1 = cur_1->next;
    }

    return 0;
}

int ds_hashtable_erase_and_destroy(DS_HashTable *ht, DS_HASHTABLE_MATCH_TYPE target)
{
    DS_HASHTABLE_TYPE *x = NULL;

    if (!ds_hashtable_erase(ht, target, &x))
    {
        return 0;
    }

    DS_HASHTABLE_DESTROY_ELEMENT(*x);
    free(x);

    return 1;
}

DS_HashTable *ds_hashtable_clone(const DS_HashTable *ht, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    if (ht == NULL)
    {
        *judge = 1;
        return NULL;
    }

    // step 1.创建新表
    DS_HashTable *new_ht = ds_hashtable_create();
    if (new_ht == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (ht->capacity == 0)
    {
        new_ht->buckets = NULL;
        new_ht->capacity = 0;
        new_ht->size = 0;

        *judge = 1;
        return new_ht;
    }

    // 为新表分配内存
    if (!ds_hashtable_reserve(new_ht, ht->capacity))
    {
        ds_hashtable_destroy(new_ht);
        *judge = 0;
        return NULL;
    }

    // step 3.依次深拷贝每一个元素
    int i;
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *cur = ht->buckets[i];
        HashNode *cur_new = NULL;

        while (cur != NULL)
        {
            HashNode *new_node = create_Node(cur->data);
            if (new_node == NULL)
            {
                ds_hashtable_destroy(new_ht);
                *judge = 0;
                return NULL;
            }

            if (cur_new == NULL)
            {
                new_ht->buckets[i] = new_node;
            }
            else
            {
                cur_new->next = new_node;
            }

            cur = cur->next;
            cur_new = new_node;
        }
    }

    new_ht->size = ht->size;

    *judge = 1;
    return new_ht;
}

int ds_hashtable_reserve(DS_HashTable *ht, int new_capacity)
{
    if (ht == NULL || new_capacity < ht->size)
    {
        return 0;
    }

    if (new_capacity == ht->capacity)
    {
        return 1;
    }

    if (new_capacity == 0) // 能进入此分支意味着 ht->size == 0，ht 里面无元素
    {
        free(ht->buckets);
        ht->buckets = NULL;
        ht->capacity = 0;
        ht->size = 0;

        return 1;
    }

    HashNode **new_buckets = (HashNode **)malloc(new_capacity * sizeof(HashNode *));
    if (new_buckets == NULL)
    {
        return 0;
    }

    int i;
    for (i = 0; i < new_capacity; i++)
    {
        new_buckets[i] = NULL;
    }

    for (i = 0; i < ht->capacity; i++)
    {
        if (ht->buckets[i] != NULL)
        {
            HashNode *cur = ht->buckets[i];

            while (cur != NULL)
            {
                HashNode *temp = cur;
                cur = cur->next;

                int hash_index = DS_HASHTABLE_HASH(temp->data, new_capacity);

                if (new_buckets[hash_index] == NULL)
                {
                    new_buckets[hash_index] = temp;
                    temp->next = NULL;
                }
                else
                {
                    HashNode *cur_ = new_buckets[hash_index];

                    new_buckets[hash_index] = temp;
                    temp->next = cur_;
                }
            }
        }
    }

    free(ht->buckets);

    ht->buckets = new_buckets;
    ht->capacity = new_capacity;

    return 1;
}
