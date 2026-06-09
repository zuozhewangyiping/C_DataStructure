#include <stdlib.h>
#include "ds_redblacktree.h"

#define RB_RED 0
#define RB_BLACK 1

struct DS_RedBlackTree
{
    RedBlackTreeNode *root;
};

struct RedBlackTreeNode
{
    DS_REDBLACKTREE_TYPE data;
    struct RedBlackTreeNode *left;
    struct RedBlackTreeNode *right;
    struct RedBlackTreeNode *parent;
    int color;
};

/* Part 0. Inner Stack & Queue helpers --------------------------------------*/

#define DS_STACK_TYPE RedBlackTreeNode *
#define DS_QUEUE_TYPE RedBlackTreeNode *

typedef struct DS_Stack
{
    DS_STACK_TYPE *data;
    int top;
    int capacity;
} DS_Stack;

typedef struct DS_Queue
{
    DS_QUEUE_TYPE *data;
    int front;
    int rear;
    int size;
    int capacity;
} DS_Queue;

/* Stack */
static DS_Stack *ds_stack_create(void);
static void ds_stack_destroy(DS_Stack *s);
static int ds_stack_length(const DS_Stack *s);
static int ds_stack_push(DS_Stack *s, DS_STACK_TYPE value);
static int ds_stack_pop(DS_Stack *s, DS_STACK_TYPE *x);
static int ds_stack_peek(const DS_Stack *s, DS_STACK_TYPE *x);
static int ds_stack_is_empty(const DS_Stack *s);

static DS_Stack *ds_stack_create(void)
{
    DS_Stack *s = (DS_Stack *)malloc(sizeof(DS_Stack));

    if (s == NULL)
    {
        return NULL;
    }

    s->data = NULL;
    s->top = -1;
    s->capacity = 0;

    return s;
}

static void ds_stack_destroy(DS_Stack *s)
{
    if (s == NULL)
    {
        return;
    }

    free(s->data);

    free(s);
}

static int ds_stack_length(const DS_Stack *s)
{
    if (s == NULL)
    {
        return -1;
    }

    return s->top + 1;
}

static int ds_stack_push(DS_Stack *s, DS_STACK_TYPE value)
{
    if (s == NULL)
    {
        return 0;
    }

    if (ds_stack_length(s) >= s->capacity)
    {
        int new_capacity;

        if (s->capacity == 0)
        {
            new_capacity = 1;
        }
        else
        {
            new_capacity = 2 * s->capacity;
        }

        DS_STACK_TYPE *temp = (DS_STACK_TYPE *)realloc(s->data, new_capacity * sizeof(DS_STACK_TYPE));

        if (temp == NULL)
        {
            return 0;
        }

        s->data = temp;
        s->capacity = new_capacity;
    }

    s->data[++s->top] = value;

    return 1;
}

static int ds_stack_pop(DS_Stack *s, DS_STACK_TYPE *x)
{
    if (s == NULL || x == NULL)
    {
        return 0;
    }

    if (ds_stack_is_empty(s))
    {
        return 0;
    }

    *x = s->data[s->top];

    s->top--;

    return 1;
}

static int ds_stack_peek(const DS_Stack *s, DS_STACK_TYPE *x)
{
    if (s == NULL || x == NULL)
    {
        return 0;
    }

    if (ds_stack_is_empty(s))
    {
        return 0;
    }

    *x = s->data[s->top];

    return 1;
}

static int ds_stack_is_empty(const DS_Stack *s)
{
    if (s == NULL)
    {
        return -1;
    }
    else if (s->top == -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Queue */
static DS_Queue *ds_queue_create(void);
static void ds_queue_destroy(DS_Queue *q);
static int ds_queue_length(const DS_Queue *q);
static int ds_queue_enqueue(DS_Queue *q, DS_QUEUE_TYPE value);
static int ds_queue_dequeue(DS_Queue *q, DS_QUEUE_TYPE *x);
static int ds_queue_is_empty(const DS_Queue *q);

static DS_Queue *ds_queue_create(void)
{
    DS_Queue *q = (DS_Queue *)malloc(sizeof(DS_Queue));

    if (q == NULL)
    {
        return NULL;
    }

    q->data = NULL;
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    q->capacity = 0;

    return q;
}

static void ds_queue_destroy(DS_Queue *q)
{
    if (q == NULL)
    {
        return;
    }

    free(q->data);

    free(q);
}

static int ds_queue_length(const DS_Queue *q)
{
    if (q == NULL)
    {
        return -1;
    }

    return q->size;
}

static int ds_queue_enqueue(DS_Queue *q, DS_QUEUE_TYPE value)
{
    if (q == NULL)
    {
        return 0;
    }

    if (ds_queue_length(q) >= q->capacity)
    {
        int new_capacity;

        if (q->capacity == 0)
        {
            new_capacity = 1;
        }
        else
        {
            new_capacity = 2 * q->capacity;
        }

        DS_QUEUE_TYPE *temp = (DS_QUEUE_TYPE *)malloc(new_capacity * sizeof(DS_QUEUE_TYPE));

        if (temp == NULL)
        {
            return 0;
        }

        int i;
        for (i = 0; i < q->size; ++i)
        {
            temp[i] = q->data[(q->front + i) % q->capacity];
        }

        free(q->data);

        q->data = temp;
        q->front = 0;
        q->rear = q->size;
        q->capacity = new_capacity;
    }

    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;

    return 1;
}

static int ds_queue_dequeue(DS_Queue *q, DS_QUEUE_TYPE *x)
{
    if (q == NULL || x == NULL)
    {
        return 0;
    }

    if (q->size == 0)
    {
        return 0;
    }

    *x = q->data[q->front];

    q->front = (q->front + 1) % q->capacity;

    q->size--;

    return 1;
}

static int ds_queue_is_empty(const DS_Queue *q)
{
    if (q == NULL)
    {
        return -1;
    }
    else if (q->size == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Part 1. Create / Destroy -------------------------------------------------*/

DS_RedBlackTree *ds_redblacktree_create(void)
{
    DS_RedBlackTree *tree = (DS_RedBlackTree *)malloc(sizeof(DS_RedBlackTree));
    if (tree == NULL)
    {
        return NULL;
    }

    tree->root = NULL;

    return tree;
}

void ds_redblacktree_destroy(DS_RedBlackTree *tree)
{
    if (tree == NULL)
    {
        return;
    }
    if (tree->root == NULL)
    {
        free(tree);
        return;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        free(tree);
        return;
    }

    RedBlackTreeNode *cur = tree->root;
    RedBlackTreeNode *last_visit = NULL;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                free(tree);
                return;
            }

            cur = cur->left;
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                RedBlackTreeNode *top;

                if (!ds_stack_peek(s, &top))
                {
                    ds_stack_destroy(s);
                    free(tree);
                    return;
                }

                if (top->right == NULL || last_visit == top->right)
                {
                    if (!ds_stack_pop(s, &top))
                    {
                        ds_stack_destroy(s);
                        free(tree);
                        return;
                    }

                    DS_REDBLACKTREE_DESTROY_ELEMENT(top->data);

                    last_visit = top;

                    free(top);
                }
                else
                {
                    cur = top->right;
                }
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    free(tree);
}

/* Part 2. Basic Query ------------------------------------------------------*/

int ds_redblacktree_height(const DS_RedBlackTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return 0;
    }

    DS_Queue *q = ds_queue_create();
    if (q == NULL)
    {
        return -1;
    }

    if (!ds_queue_enqueue(q, tree->root))
    {
        ds_queue_destroy(q);
        return -1;
    }

    int height = 0;

    RedBlackTreeNode *cur = NULL;

    while (!ds_queue_is_empty(q))
    {
        int level_size = ds_queue_length(q);

        int i;
        for (i = 0; i < level_size; ++i)
        {
            if (!ds_queue_dequeue(q, &cur))
            {
                ds_queue_destroy(q);
                return -1;
            }

            if (cur->left != NULL)
            {
                if (!ds_queue_enqueue(q, cur->left))
                {
                    ds_queue_destroy(q);
                    return -1;
                }
            }
            if (cur->right != NULL)
            {
                if (!ds_queue_enqueue(q, cur->right))
                {
                    ds_queue_destroy(q);
                    return -1;
                }
            }
        }

        ++height;
    }

    ds_queue_destroy(q);

    return height;
}

int ds_redblacktree_count(const DS_RedBlackTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return 0;
    }

    int count = 0;

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return -1;
    }

    RedBlackTreeNode *cur = tree->root;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                return -1;
            }

            cur = cur->left;
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                if (!ds_stack_pop(s, &cur))
                {
                    ds_stack_destroy(s);
                    return -1;
                }

                count++;

                cur = cur->right;
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    return count;
}

int ds_redblacktree_is_empty(const DS_RedBlackTree *tree)
{
    if (tree == NULL)
    {
        return -1;
    }

    return (tree->root == NULL ? 1 : 0);
}

/* Part 3. Traversals（回调可读写 data）---------------------------------------*/

int ds_redblacktree_traverse_preorder_value(DS_RedBlackTree *tree, void *user_data, void (*visit)(DS_REDBLACKTREE_TYPE *value, void *callback_data))
{
    if (tree == NULL || tree->root == NULL)
    {
        return 1;
    }
    if (visit == NULL)
    {
        return 0;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return 0;
    }

    RedBlackTreeNode *cur = tree->root;

    while (1)
    {
        if (cur != NULL)
        {
            visit(&(cur->data), user_data);

            if (cur->right != NULL)
            {
                if (!ds_stack_push(s, cur->right))
                {
                    ds_stack_destroy(s);
                    return 0;
                }
            }

            cur = cur->left;
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                if (!ds_stack_pop(s, &cur))
                {
                    ds_stack_destroy(s);
                    return 0;
                }
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    return 1;
}

int ds_redblacktree_traverse_inorder_value(DS_RedBlackTree *tree, void *user_data, void (*visit)(DS_REDBLACKTREE_TYPE *value, void *callback_data))
{
    if (tree == NULL || tree->root == NULL)
    {
        return 1;
    }
    if (visit == NULL)
    {
        return 0;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return 0;
    }

    RedBlackTreeNode *cur = tree->root;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                return 0;
            }

            cur = cur->left;
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                if (!ds_stack_pop(s, &cur))
                {
                    ds_stack_destroy(s);
                    return 0;
                }

                visit(&(cur->data), user_data);

                cur = cur->right;
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    return 1;
}

int ds_redblacktree_traverse_postorder_value(DS_RedBlackTree *tree, void *user_data, void (*visit)(DS_REDBLACKTREE_TYPE *value, void *callback_data))
{
    if (tree == NULL || tree->root == NULL)
    {
        return 1;
    }
    if (visit == NULL)
    {
        return 0;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return 0;
    }

    RedBlackTreeNode *cur = tree->root;
    RedBlackTreeNode *last_visit = NULL;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                return 0;
            }

            cur = cur->left;
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                RedBlackTreeNode *top;

                if (!ds_stack_peek(s, &top))
                {
                    ds_stack_destroy(s);
                    return 0;
                }

                if (top->right == NULL || last_visit == top->right)
                {
                    if (!ds_stack_pop(s, &top))
                    {
                        ds_stack_destroy(s);
                        return 0;
                    }

                    visit(&(top->data), user_data);

                    last_visit = top;
                }
                else
                {
                    cur = top->right;
                }
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    return 1;
}

int ds_redblacktree_traverse_levelorder_value(DS_RedBlackTree *tree, void *user_data, void (*visit)(DS_REDBLACKTREE_TYPE *value, void *callback_data))
{
    if (tree == NULL || tree->root == NULL)
    {
        return 1;
    }
    if (visit == NULL)
    {
        return 0;
    }

    DS_Queue *q = ds_queue_create();
    if (q == NULL)
    {
        return 0;
    }

    if (!ds_queue_enqueue(q, tree->root))
    {
        ds_queue_destroy(q);
        return 0;
    }

    RedBlackTreeNode *cur = NULL;

    while (!ds_queue_is_empty(q))
    {
        if (!ds_queue_dequeue(q, &cur))
        {
            ds_queue_destroy(q);
            return 0;
        }

        visit(&(cur->data), user_data);

        if (cur->left != NULL)
        {
            if (!ds_queue_enqueue(q, cur->left))
            {
                ds_queue_destroy(q);
                return 0;
            }
        }
        if (cur->right != NULL)
        {
            if (!ds_queue_enqueue(q, cur->right))
            {
                ds_queue_destroy(q);
                return 0;
            }
        }
    }

    ds_queue_destroy(q);

    return 1;
}

/* Part 4. Clone ------------------------------------------------------------*/

static RedBlackTreeNode *create_rbtree_node(DS_REDBLACKTREE_TYPE value)
{
    RedBlackTreeNode *node = (RedBlackTreeNode *)malloc(sizeof(RedBlackTreeNode));
    if (node == NULL)
    {
        return NULL;
    }

    int clone_judge = 1;
    node->data = DS_REDBLACKTREE_CLONE_ELEMENT(value, &clone_judge);
    if (!clone_judge)
    {
        DS_REDBLACKTREE_DESTROY_ELEMENT(node->data);
        free(node);
        return NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->color = RB_RED;

    return node;
}

DS_RedBlackTree *ds_redblacktree_clone(const DS_RedBlackTree *tree, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }
    if (tree == NULL)
    {
        *judge = 1;
        return NULL;
    }

    DS_RedBlackTree *new_tree = ds_redblacktree_create();
    if (new_tree == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (tree->root == NULL)
    {
        *judge = 1;
        return new_tree;
    }

    new_tree->root = create_rbtree_node(tree->root->data);
    if (new_tree->root == NULL)
    {
        ds_redblacktree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    DS_Queue *q_old = ds_queue_create();
    if (q_old == NULL)
    {
        ds_redblacktree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }
    DS_Queue *q_new = ds_queue_create();
    if (q_new == NULL)
    {
        ds_queue_destroy(q_old);
        ds_redblacktree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    if (!ds_queue_enqueue(q_old, tree->root))
    {
        ds_queue_destroy(q_old);
        ds_queue_destroy(q_new);
        ds_redblacktree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }
    if (!ds_queue_enqueue(q_new, new_tree->root))
    {
        ds_queue_destroy(q_old);
        ds_queue_destroy(q_new);
        ds_redblacktree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    RedBlackTreeNode *cur_old;
    RedBlackTreeNode *cur_new;

    while (!ds_queue_is_empty(q_old))
    {
        if (!ds_queue_dequeue(q_old, &cur_old))
        {
            ds_queue_destroy(q_old);
            ds_queue_destroy(q_new);
            ds_redblacktree_destroy(new_tree);
            *judge = 0;
            return NULL;
        }
        if (!ds_queue_dequeue(q_new, &cur_new))
        {
            ds_queue_destroy(q_old);
            ds_queue_destroy(q_new);
            ds_redblacktree_destroy(new_tree);
            *judge = 0;
            return NULL;
        }

        if (cur_old->left != NULL)
        {
            if (!ds_queue_enqueue(q_old, cur_old->left))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }

            cur_new->left = create_rbtree_node((cur_old->left)->data);
            if (cur_new->left == NULL)
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
            (cur_new->left)->color = (cur_old->left)->color;
            (cur_new->left)->parent = cur_new;

            if (!ds_queue_enqueue(q_new, cur_new->left))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
        }
        if (cur_old->right != NULL)
        {
            if (!ds_queue_enqueue(q_old, cur_old->right))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }

            cur_new->right = create_rbtree_node((cur_old->right)->data);
            if (cur_new->right == NULL)
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
            (cur_new->right)->color = (cur_old->right)->color;
            (cur_new->right)->parent = cur_new;

            if (!ds_queue_enqueue(q_new, cur_new->right))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_redblacktree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
        }
    }

    ds_queue_destroy(q_old);
    ds_queue_destroy(q_new);

    *judge = 1;
    return new_tree;
}

/* Part 5. Cursor Accessor --------------------------------------------------*/

int ds_redblacktree_node_get_data(RedBlackTreeNode *cursor, DS_REDBLACKTREE_TYPE **x)
{
    if (cursor == NULL || x == NULL)
    {
        return 0;
    }

    *x = &(cursor->data);

    return 1;
}

/* Part 6. Search -----------------------------------------------------------*/

RedBlackTreeNode *ds_redblacktree_search(DS_RedBlackTree *tree, DS_REDBLACKTREE_TYPE value)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    RedBlackTreeNode *cur = tree->root;

    while (1)
    {
        if (DS_REDBLACKTREE_EQ(cur->data, value))
        {
            return cur;
        }
        else if (DS_REDBLACKTREE_GT(cur->data, value))
        {
            if (cur->left != NULL)
            {
                cur = cur->left;
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            if (cur->right != NULL)
            {
                cur = cur->right;
            }
            else
            {
                return NULL;
            }
        }
    }
}

/* Part 7. Cursor Acquisition -----------------------------------------------*/

RedBlackTreeNode *ds_redblacktree_find_max(DS_RedBlackTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    RedBlackTreeNode *cur = tree->root;

    while (cur->right != NULL)
    {
        cur = cur->right;
    }

    return cur;
}

RedBlackTreeNode *ds_redblacktree_find_min(DS_RedBlackTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    RedBlackTreeNode *cur = tree->root;

    while (cur->left != NULL)
    {
        cur = cur->left;
    }

    return cur;
}

/* Part 8. Cursor Navigation ------------------------------------------------*/

RedBlackTreeNode *ds_redblacktree_predecessor(DS_RedBlackTree *tree, RedBlackTreeNode *cursor)
{
    if (tree == NULL || tree->root == NULL || cursor == NULL)
    {
        return NULL;
    }

    if (cursor->left != NULL)
    {
        RedBlackTreeNode *cur = cursor->left;

        while (cur->right != NULL)
        {
            cur = cur->right;
        }

        return cur;
    }
    else
    {
        RedBlackTreeNode *cur = tree->root;
        RedBlackTreeNode *prob = NULL;

        while (1)
        {
            if (DS_REDBLACKTREE_LT(cursor->data, cur->data))
            {
                cur = cur->left;
            }
            else if (DS_REDBLACKTREE_GT(cursor->data, cur->data))
            {
                prob = cur;

                cur = cur->right;
            }
            else
            {
                break;
            }
        }

        return prob;
    }
}

RedBlackTreeNode *ds_redblacktree_successor(DS_RedBlackTree *tree, RedBlackTreeNode *cursor)
{
    if (tree == NULL || tree->root == NULL || cursor == NULL)
    {
        return NULL;
    }

    if (cursor->right != NULL)
    {
        RedBlackTreeNode *cur = cursor->right;

        while (cur->left != NULL)
        {
            cur = cur->left;
        }

        return cur;
    }
    else
    {
        RedBlackTreeNode *cur = tree->root;
        RedBlackTreeNode *prob = NULL;

        while (1)
        {
            if (DS_REDBLACKTREE_LT(cursor->data, cur->data))
            {
                prob = cur;

                cur = cur->left;
            }
            else if (DS_REDBLACKTREE_GT(cursor->data, cur->data))
            {
                cur = cur->right;
            }
            else
            {
                break;
            }
        }

        return prob;
    }
}

/* Part 9. Range Query（回调可读写 data）-------------------------------------*/

int ds_redblacktree_range_query(DS_RedBlackTree *tree,
                                DS_REDBLACKTREE_TYPE low,
                                DS_REDBLACKTREE_TYPE high,
                                void *user_data,
                                void (*visit)(DS_REDBLACKTREE_TYPE *value, void *callback_data))
{
    if (tree == NULL)
    {
        return 0;
    }

    if (tree->root == NULL)
    {
        return 1;
    }

    if (visit == NULL)
    {
        return 0;
    }

    if (DS_REDBLACKTREE_GT(low, high))
    {
        return 1;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return 0;
    }

    RedBlackTreeNode *cur = tree->root;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                return 0;
            }

            if (DS_REDBLACKTREE_GE(cur->data, low))
            {
                cur = cur->left;
            }
            else
            {
                cur = NULL;
            }
        }
        else
        {
            if (!ds_stack_is_empty(s))
            {
                if (!ds_stack_pop(s, &cur))
                {
                    ds_stack_destroy(s);
                    return 0;
                }

                if (DS_REDBLACKTREE_LE(low, cur->data) && DS_REDBLACKTREE_LE(cur->data, high))
                {
                    visit(&(cur->data), user_data);
                }

                if (DS_REDBLACKTREE_LE(cur->data, high))
                {
                    cur = cur->right;
                }
                else
                {
                    cur = NULL;
                }
            }
            else
            {
                break;
            }
        }
    }

    ds_stack_destroy(s);

    return 1;
}

/* Part 10. Insert / Delete -------------------------------------------------*/

static RedBlackTreeNode *rotate_left_left(RedBlackTreeNode *root)
{
    RedBlackTreeNode *ancestor = root->parent;
    RedBlackTreeNode *C = root;
    RedBlackTreeNode *B = C->left;
    RedBlackTreeNode *temp = B->right;

    B->right = C;
    C->left = temp;

    C->parent = B;
    if (temp != NULL)
    {
        temp->parent = C;
    }

    if (ancestor != NULL)
    {
        if (ancestor->left == root)
        {
            ancestor->left = B;
        }
        else
        {
            ancestor->right = B;
        }
    }
    B->parent = ancestor;

    return B;
}

static RedBlackTreeNode *rotate_right_right(RedBlackTreeNode *root)
{
    RedBlackTreeNode *ancestor = root->parent;
    RedBlackTreeNode *A = root;
    RedBlackTreeNode *B = A->right;
    RedBlackTreeNode *temp = B->left;

    B->left = A;
    A->right = temp;

    A->parent = B;
    if (temp != NULL)
    {
        temp->parent = A;
    }

    if (ancestor != NULL)
    {
        if (ancestor->left == root)
        {
            ancestor->left = B;
        }
        else
        {
            ancestor->right = B;
        }
    }
    B->parent = ancestor;

    return B;
}

static RedBlackTreeNode *rotate_left_right(RedBlackTreeNode *root)
{
    RedBlackTreeNode *ancestor = root->parent;
    RedBlackTreeNode *C = root;
    RedBlackTreeNode *A = C->left;
    RedBlackTreeNode *B = A->right;
    RedBlackTreeNode *temp1 = B->left;
    RedBlackTreeNode *temp2 = B->right;

    B->left = A;
    B->right = C;
    A->right = temp1;
    C->left = temp2;

    A->parent = B;
    C->parent = B;
    if (temp1 != NULL)
    {
        temp1->parent = A;
    }
    if (temp2 != NULL)
    {
        temp2->parent = C;
    }

    if (ancestor != NULL)
    {
        if (ancestor->left == root)
        {
            ancestor->left = B;
        }
        else
        {
            ancestor->right = B;
        }
    }
    B->parent = ancestor;

    return B;
}

static RedBlackTreeNode *rotate_right_left(RedBlackTreeNode *root)
{
    RedBlackTreeNode *ancestor = root->parent;
    RedBlackTreeNode *A = root;
    RedBlackTreeNode *C = A->right;
    RedBlackTreeNode *B = C->left;
    RedBlackTreeNode *temp1 = B->left;
    RedBlackTreeNode *temp2 = B->right;

    B->left = A;
    B->right = C;
    A->right = temp1;
    C->left = temp2;

    A->parent = B;
    C->parent = B;
    if (temp1 != NULL)
    {
        temp1->parent = A;
    }
    if (temp2 != NULL)
    {
        temp2->parent = C;
    }

    if (ancestor != NULL)
    {
        if (ancestor->left == root)
        {
            ancestor->left = B;
        }
        else
        {
            ancestor->right = B;
        }
    }
    B->parent = ancestor;

    return B;
}

static RedBlackTreeNode *rule_4_fix(RedBlackTreeNode *root, RedBlackTreeNode *cur)
{
    while (cur->parent != NULL && cur->parent->color != RB_BLACK)
    {
        /*
            **对修复大循环的解读**
            一、进入循环的，必满足以下情况：
            1.cur不是树根；
            2.cur有父节点（cur不是树根就肯定有父节点）；
            3.cur的父节点必是红色（否则已经修复完成）；
            4.cur必有祖父节点，且祖父节点为黑色（否则父节点不可能是红色）；
            5.cur必有叔叔节点（无论叔叔是不是NULL）。
            二、循环内的情况分类：
            1.uncle是NULL或为黑色：
              (1)按照cur/par/uncle三者的相对位置，分出ll/lr/rl/rr四种情况，分别旋转+变色进行修复；
              (2)其中“变色”操作具体为：旋转后的局部root为黑，局部root的两个孩子都为红（四种旋转的变色操作相同）；
              (3)修复结束即可退出循环（直接break）。
            2.uncle是红色：
              (1)不旋转，按照 “父变黑，叔变黑，祖变红” 的规则变色；
              (2)此时祖父节点之下达成了新平衡，但祖父的颜色从黑改为红可能引起上层新的不平衡，需要以祖父节点为新的cur继续参与循环（不能break）。
        */

        RedBlackTreeNode *par = cur->parent;
        RedBlackTreeNode *grand_par = par->parent;
        RedBlackTreeNode *uncle;
        char uncle_side;
        if (grand_par->left == par)
        {
            uncle = grand_par->right;
            uncle_side = 'r';
        }
        else
        {
            uncle = grand_par->left;
            uncle_side = 'l';
        }

        char cur_side;
        if (par->left == cur)
        {
            cur_side = 'l';
        }
        else
        {
            cur_side = 'r';
        }

        if (uncle == NULL || uncle->color == RB_BLACK) /* uncle是NULL或为黑色：分出ll/lr/rl/rr四种情况，分别旋转，并统一变色 */
        {
            RedBlackTreeNode *local_root = NULL;

            if (uncle_side == 'r' && cur_side == 'l') // ll
            {
                local_root = rotate_left_left(grand_par);
            }
            else if (uncle_side == 'r' && cur_side == 'r') // lr
            {
                local_root = rotate_left_right(grand_par);
            }
            else if (uncle_side == 'l' && cur_side == 'l') // rl
            {
                local_root = rotate_right_left(grand_par);
            }
            else // rr
            {
                local_root = rotate_right_right(grand_par);
            }

            local_root->color = RB_BLACK;
            local_root->left->color = RB_RED;
            local_root->right->color = RB_RED;

            if (local_root->parent == NULL)
            {
                root = local_root;
            }

            break;
        }
        else
        {
            par->color = RB_BLACK;
            uncle->color = RB_BLACK;
            grand_par->color = RB_RED;

            cur = grand_par;
        }
    }

    root->color = RB_BLACK; // 强制操作，保证规则一不被破坏

    return root;
}

int ds_redblacktree_insert(DS_RedBlackTree *tree, DS_REDBLACKTREE_TYPE value)
{
    /*
        **对插入函数的解读**
        一、整体按照bst的insert逻辑走，直到找到正确的插入位置。
        二、新节点默认是红色，所以根据其父节点颜色进行分类：
        1.父节点为黑色：直接插入，未违反任何规则，插入成功；
        2.父节点为红色：插入新节点后，出现“两个红色节点相连”的情况，违反规则四，进入修复函数，修复结束后插入成功（需要用树根root接受修复函数的返回值）。
    */

    if (tree == NULL)
    {
        return 0;
    }

    if (tree->root == NULL)
    {
        RedBlackTreeNode *new_node = create_rbtree_node(value);
        if (new_node == NULL)
        {
            return 0;
        }

        tree->root = new_node;
        tree->root->color = RB_BLACK;

        return 1;
    }
    else
    {
        RedBlackTreeNode *cur = tree->root;

        while (1)
        {
            if (DS_REDBLACKTREE_LT(value, cur->data))
            {
                if (cur->left != NULL)
                {
                    cur = cur->left;
                }
                else
                {
                    RedBlackTreeNode *new_node = create_rbtree_node(value);
                    if (new_node == NULL)
                    {
                        return 0;
                    }

                    cur->left = new_node;
                    new_node->parent = cur;
                    cur = cur->left;

                    if (cur->parent->color == RB_RED)
                    {
                        tree->root = rule_4_fix(tree->root, cur);
                    }

                    break;
                }
            }
            else if (DS_REDBLACKTREE_GT(value, cur->data))
            {
                if (cur->right != NULL)
                {
                    cur = cur->right;
                }
                else
                {
                    RedBlackTreeNode *new_node = create_rbtree_node(value);
                    if (new_node == NULL)
                    {
                        return 0;
                    }

                    cur->right = new_node;
                    new_node->parent = cur;
                    cur = cur->right;

                    if (cur->parent->color == RB_RED)
                    {
                        tree->root = rule_4_fix(tree->root, cur);
                    }

                    break;
                }
            }
            else
            {
                return 0;
            }
        }
    }

    return 1;
}

static RedBlackTreeNode *rule_5_fix(RedBlackTreeNode *root, RedBlackTreeNode *cur, char cur_side) // victim的bro作为cur传入此函数
{
    while (cur->parent != NULL)
    {
        /*
            **对修复大循环的解读**
            一、进入循环的，必满足以下情况：
            1.cur不是树根；
            2.cur有父节点（cur不是树根就肯定有父节点）；
            二、循环内的情况分类：
            首先设par为cur的父节点。
            1.cur是黑色，根据cur的孩子颜色进行分类：
              (1)若cur有至少一个红色孩子：
                 [1]根据“ ‘这个红孩子在cur的位置’ + ‘cur_side（即cur在par的位置）’ ”，分出ll/lr/rl/rr四种情况，分别旋转+变色进行修复；
                 [2]其中“变色”操作具体为：旋转后的局部root继承原来par的颜色，局部root的两个孩子都为黑（四种旋转的变色操作相同）；
                 [3]修复结束即可退出循环（直接break）。
              (2)若cur没有红色孩子（孩子全都是黑色或 NULL）：
                 [1]直接将cur染红（此时par的两棵子树都少了一个黑节点，par之下达成平衡；但是从par开始少了一个黑节点，par上层不平衡）；
                 [2]然后根据par的颜色进行分类：
                    <1>par为红色：将par染黑，修复完成（par自身变黑直接补充了缺少的那个黑节点，par上层达到了平衡），退出循环（直接break）；
                    <2>par为黑色，根据par的父节点情况分类：
                       {1}若par没有父节点（即par为树根），那么直接退出，（因为par没有上层，所以自然就不存在“par上层不平衡”的说法），修复已完成；
                       {2}若par有父节点，那么令par的bro作为新的cur开启下一轮循环（将不平衡上移）。
            2.cur是红色，那么先试图将不平衡状态变换成情况1，后续循环再进行修复：
              (1)根据cur在par的位置（通过cur_side判断），分出ll/rr两种情况，分别旋转+变色；
              (2)其中变色操作具体为：将cur染黑，par染红（旋转后，cur变成了局部根，par降为cur的孩子）；
              (3)旋转变色后，cur的其中一个孩子会变为par的孩子，这个孩子将作为新的cur开启下一轮循环，情况如下：
                 [1]若进行了ll旋转，那么新cur就是旋转后par的左孩子，cur_side为‘l’；
                 [2]若进行了rr旋转，那么新cur就是旋转后par的右孩子，cur_side为‘r’。
            三、补充说明：
            对于情况2，cur为红色时必有两个黑孩子（因为delete之前是平衡的，那么cur之下必有黑节点与被删除的黑节点保持平衡）。
        */

        RedBlackTreeNode *par = cur->parent;

        if (cur->color == RB_BLACK) // cur是黑色，根据cur的孩子颜色进行分类
        {
            if ((cur->left == NULL || cur->left->color == RB_BLACK) && (cur->right == NULL || cur->right->color == RB_BLACK)) // 若cur没有红色孩子
            {
                cur->color = RB_RED;

                if (par->color == RB_RED) // par为红色：将par染黑，修复完成
                {
                    par->color = RB_BLACK;

                    break;
                }
                else // par为黑色，根据par的父节点情况分类
                {
                    if (par->parent == NULL) // 若par没有父节点，那么直接退出
                    {
                        break;
                    }
                    else // 若par有父节点，那么令par的bro作为新的cur开启下一轮循环
                    {
                        if (par->parent->left == par)
                        {
                            cur = par->parent->right;
                            cur_side = 'r';
                        }
                        else
                        {
                            cur = par->parent->left;
                            cur_side = 'l';
                        }
                    }
                }
            }
            else // 若cur有至少一个红色孩子
            {
                // 根据“ ‘这个红孩子在cur的位置’ + ‘cur_side（即cur在par的位置）’ ”，分出ll/lr/rl/rr四种情况，分别旋转+变色进行修复

                RedBlackTreeNode *local_root = NULL;

                if (cur->left != NULL && cur->left->color == RB_RED)
                {
                    if (cur_side == 'l') // ll
                    {
                        local_root = rotate_left_left(par);
                    }
                    else // rl
                    {
                        local_root = rotate_right_left(par);
                    }
                }
                else if (cur->right != NULL && cur->right->color == RB_RED)
                {
                    if (cur_side == 'l') // lr
                    {
                        local_root = rotate_left_right(par);
                    }
                    else // rr
                    {
                        local_root = rotate_right_right(par);
                    }
                }

                // 旋转后的局部root继承原来par的颜色，局部root的两个孩子都为黑（四种旋转的变色操作相同）
                local_root->color = par->color;
                local_root->left->color = RB_BLACK;
                local_root->right->color = RB_BLACK;

                if (local_root->parent == NULL)
                {
                    root = local_root;
                }

                break;
            }
        }
        else // cur是红色，那么先试图将不平衡状态变换成情况1，后续循环再进行修复
        {
            // 根据cur在par的位置（通过cur_side判断），分出ll/rr两种情况，分别旋转+变色

            RedBlackTreeNode *local_root = NULL;

            // 将cur染黑，par染红（先染色还是先旋转无所谓）
            cur->color = RB_BLACK;
            par->color = RB_RED;

            if (cur_side == 'l') // 若进行了ll旋转，那么新cur就是旋转后par的左孩子，cur_side为‘l’
            {
                local_root = rotate_left_left(par);

                cur = par->left;
                cur_side = 'l';
            }
            else // 若进行了rr旋转，那么新cur就是旋转后par的右孩子，cur_side为‘r’
            {
                local_root = rotate_right_right(par);

                cur = par->right;
                cur_side = 'r';
            }

            if (local_root->parent == NULL)
            {
                root = local_root;
            }
        }
    }

    root->color = RB_BLACK;

    return root;
}

int ds_redblacktree_delete(DS_RedBlackTree *tree, DS_REDBLACKTREE_TYPE value)
{
    /*
        **对delete函数的解读**
        一、整体按照bst的insert逻辑走，直到找到正确的插入位置。
        二、找到需删除节点cur后，分为6种情况：
        1.cur是树根：
          (1)cur是叶子节点：直接删除cur，整棵树变为NULL，删除完成；
          (2)cur是双孩子节点：找到该节点的后继节点（后继节点才是真正的victim），将后继结点的值赋给cur，分类讨论：
             [1]victim是红色，直接删除victim，删除完成；
             [2]victim是黑色，分类讨论：
                 <1>victim有右孩子，将这个右孩子染黑，删除victim，删除完成；
                 <2>victim无右孩子（即victim是叶子节点），获取victim的bro和bro_side，删除victim后进入rule_5_fix()函数，修复完成后删除成功（见补充{2}）。
          (3)cur是单孩子节点：把root让给它的孩子，并将孩子颜色设为黑，然后删除cur，删除完成。
        2.cur不是树根：
          (1)cur是叶子节点：分类讨论：
             [1]cur是红色，直接删除cur，删除完成；
             [2]cur是黑色，获取cur的bro和bro_side，删除cur后进入rule_5_fix()函数，修复完成后删除成功（见补充{3}）。
          (2)cur是双孩子节点：与“cur是树根”情况一致（直接复制即可）；
          (3)cur是单孩子节点：将cur的孩子染黑，删除victim，删除完成（见补充{4}）。
        三、补充说明：
        {1}以上步骤中“删除某节点”的具体步骤是：将该节点的parent与child相连（双方指针都相连），然后free掉该节点；
        {2}对于情况1-(2)-[2]-<1>，victim此时是“非根的单孩子节点，且为黑”，那么victim唯一的孩子必为红色（若cur的唯一孩子为黑，那么cur的左右子树black失衡）；
        {3}对于情况1-(2)-[2]-<2>，victim此时是“非根的叶子节点，且为黑”，那么victim必有bro，否则victim的parent的左右子树的black数失衡；
        {4}对于情况2-(1)-[2]，cur此时是“非根的叶子节点，且为黑”，与“情况1-(2)-[2]-<2>”一致；
        {5}对于情况2-(3)，此时cur是“非根的单孩子节点”，那么cur必为黑色（若cur为红，那么cur唯一的孩子只能为黑，那么cur的左右子树black失衡），
           且cur唯一的孩子必为红色（若cur的唯一孩子为黑，那么cur的左右子树black失衡）。
        四、总结：
        只有当真正被删除的节点是 “非根黑色叶子节点” 时才需要进入修复函数rule_5_fix，其余情况可通过调整局部颜色达成平衡。
    */

    if (tree == NULL)
    {
        return 0;
    }

    if (tree->root == NULL)
    {
        return 0;
    }
    else
    {
        RedBlackTreeNode *cur = tree->root;

        while (1)
        {
            if (DS_REDBLACKTREE_LT(value, cur->data))
            {
                if (cur->left != NULL)
                {
                    cur = cur->left;
                }
                else
                {
                    return 0;
                }
            }
            else if (DS_REDBLACKTREE_GT(value, cur->data))
            {
                if (cur->right != NULL)
                {
                    cur = cur->right;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if (cur == tree->root)
                {
                    if (cur->left == NULL && cur->right == NULL) // cur是叶子节点：直接删除cur，整棵树变为NULL，删除完成
                    {
                        DS_REDBLACKTREE_DESTROY_ELEMENT(cur->data);
                        free(cur);
                        tree->root = NULL;

                        return 1;
                    }
                    else if (cur->left != NULL && cur->right != NULL) // cur是双孩子节点
                    {
                        RedBlackTreeNode *victim = ds_redblacktree_successor(tree, cur); // 获取后继节点（即真正的victim）

                        DS_REDBLACKTREE_TYPE tmp_data = cur->data;
                        cur->data = victim->data; // 将后继结点的值赋给cur
                        victim->data = tmp_data;

                        // 统一执行 “获取bro和bro_side” 与 “将victim的parent与child相连（双方指针都相连）”两个操作
                        RedBlackTreeNode *bro;
                        char bro_side;
                        if (victim->parent->left == victim)
                        {
                            bro = victim->parent->right;
                            bro_side = 'r';

                            victim->parent->left = victim->right;
                            if (victim->right != NULL)
                            {
                                victim->right->parent = victim->parent;
                            }
                        }
                        else
                        {
                            bro = victim->parent->left;
                            bro_side = 'l';

                            victim->parent->right = victim->right;
                            if (victim->right != NULL)
                            {
                                victim->right->parent = victim->parent;
                            }
                        }

                        if (victim->color == RB_RED) // victim是红色，直接删除victim，删除完成
                        {
                            DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);

                            free(victim);
                            return 1;
                        }
                        else
                        {
                            if (victim->right != NULL) // victim有右孩子，将这个右孩子染黑，删除victim，删除完成
                            {
                                victim->right->color = RB_BLACK;

                                DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);

                                free(victim);
                                return 1;
                            }
                            else // victim无右孩子，获取victim的bro和bro_side，删除victim后进入修复函数，修复完成后删除成功
                            {
                                DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);
                                free(victim);

                                tree->root = rule_5_fix(tree->root, bro, bro_side);

                                return 1;
                            }
                        }
                    }
                    else // cur是单孩子节点：把tree->root让给它的孩子，并将孩子颜色设为黑，然后删除cur，删除完成
                    {
                        RedBlackTreeNode *child;
                        if (cur->left != NULL)
                        {
                            child = cur->left;
                        }
                        else
                        {
                            child = cur->right;
                        }

                        tree->root = child;
                        child->parent = NULL;
                        child->color = RB_BLACK;

                        DS_REDBLACKTREE_DESTROY_ELEMENT(cur->data);
                        free(cur);

                        return 1;
                    }
                }
                else // cur不是树根
                {
                    if (cur->left == NULL && cur->right == NULL) // cur是叶子节点：分类讨论
                    {
                        // 统一执行 “获取bro和bro_side” 与 “将cur与parent断连”两个操作
                        RedBlackTreeNode *bro;
                        char bro_side;
                        if (cur->parent->left == cur)
                        {
                            bro = cur->parent->right;
                            bro_side = 'r';

                            cur->parent->left = NULL;
                        }
                        else
                        {
                            bro = cur->parent->left;
                            bro_side = 'l';

                            cur->parent->right = NULL;
                        }

                        if (cur->color == RB_RED) // cur是红色，直接删除cur，删除完成
                        {
                            DS_REDBLACKTREE_DESTROY_ELEMENT(cur->data);
                            free(cur);

                            return 1;
                        }
                        else // cur是黑色，删除cur后进入rule_5_fix()函数，修复完成后删除成功
                        {
                            DS_REDBLACKTREE_DESTROY_ELEMENT(cur->data);
                            free(cur);

                            tree->root = rule_5_fix(tree->root, bro, bro_side);

                            return 1;
                        }
                    }
                    else if (cur->left != NULL && cur->right != NULL) // cur是双孩子节点：与“cur是树根”情况一致（直接复制）
                    {
                        RedBlackTreeNode *victim = ds_redblacktree_successor(tree, cur); // 获取后继节点（即真正的victim）

                        DS_REDBLACKTREE_TYPE tmp_data = cur->data;
                        cur->data = victim->data; // 将后继结点的值赋给cur
                        victim->data = tmp_data;

                        // 统一执行 “获取bro和bro_side” 与 “将victim的parent与child相连（双方指针都相连）”两个操作
                        RedBlackTreeNode *bro;
                        char bro_side;
                        if (victim->parent->left == victim)
                        {
                            bro = victim->parent->right;
                            bro_side = 'r';

                            victim->parent->left = victim->right;
                            if (victim->right != NULL)
                            {
                                victim->right->parent = victim->parent;
                            }
                        }
                        else
                        {
                            bro = victim->parent->left;
                            bro_side = 'l';

                            victim->parent->right = victim->right;
                            if (victim->right != NULL)
                            {
                                victim->right->parent = victim->parent;
                            }
                        }

                        if (victim->color == RB_RED) // victim是红色，直接删除victim，删除完成
                        {
                            DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);

                            free(victim);
                            return 1;
                        }
                        else
                        {
                            if (victim->right != NULL) // victim有右孩子，将这个右孩子染黑，删除victim，删除完成
                            {
                                victim->right->color = RB_BLACK;

                                DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);

                                free(victim);
                                return 1;
                            }
                            else // victim无右孩子，获取victim的bro和bro_side，删除victim后进入修复函数，修复完成后删除成功
                            {
                                DS_REDBLACKTREE_DESTROY_ELEMENT(victim->data);
                                free(victim);

                                tree->root = rule_5_fix(tree->root, bro, bro_side);

                                return 1;
                            }
                        }
                    }
                    else // cur是单孩子节点：将cur的孩子染黑，删除cur，删除完成
                    {
                        RedBlackTreeNode *child;
                        if (cur->left != NULL)
                        {
                            child = cur->left;
                        }
                        else
                        {
                            child = cur->right;
                        }

                        if (cur->parent->left == cur)
                        {
                            cur->parent->left = child;
                        }
                        else
                        {
                            cur->parent->right = child;
                        }
                        child->parent = cur->parent;

                        child->color = RB_BLACK;

                        DS_REDBLACKTREE_DESTROY_ELEMENT(cur->data);
                        free(cur);

                        return 1;
                    }
                }

                break;
            }
        }
    }

    return 1;
}
