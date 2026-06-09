#include <stdlib.h>
#include "ds_avltree.h"

struct DS_AVLTree
{
    AVLTreeNode *root;
};

struct AVLTreeNode
{
    DS_AVLTREE_TYPE data;
    struct AVLTreeNode *left;
    struct AVLTreeNode *right;
    struct AVLTreeNode *parent;
    int height;
};

/* Part 0. Inner Stack & Queue helpers --------------------------------------*/

#define DS_STACK_TYPE AVLTreeNode *
#define DS_QUEUE_TYPE AVLTreeNode *

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

DS_AVLTree *ds_avltree_create(void)
{
    DS_AVLTree *tree = (DS_AVLTree *)malloc(sizeof(DS_AVLTree));
    if (tree == NULL)
    {
        return NULL;
    }

    tree->root = NULL;

    return tree;
}

void ds_avltree_destroy(DS_AVLTree *tree)
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

    AVLTreeNode *cur = tree->root;
    AVLTreeNode *last_visit = NULL;

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
                AVLTreeNode *top;

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

                    DS_AVLTREE_DESTROY_ELEMENT(top->data);

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

int ds_avltree_height(const DS_AVLTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return 0;
    }

    return tree->root->height;
}

int ds_avltree_count(const DS_AVLTree *tree)
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

    AVLTreeNode *cur = tree->root;

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

int ds_avltree_is_empty(const DS_AVLTree *tree)
{
    if (tree == NULL)
    {
        return -1;
    }

    return (tree->root == NULL ? 1 : 0);
}

/* Part 3. Traversals（回调可读写 data）---------------------------------------*/

int ds_avltree_traverse_preorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data))
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

    AVLTreeNode *cur = tree->root;

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

int ds_avltree_traverse_inorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data))
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

    AVLTreeNode *cur = tree->root;

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

int ds_avltree_traverse_postorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data))
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

    AVLTreeNode *cur = tree->root;
    AVLTreeNode *last_visit = NULL;

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
                AVLTreeNode *top;

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

int ds_avltree_traverse_levelorder_value(DS_AVLTree *tree, void *user_data, void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data))
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

    AVLTreeNode *cur = NULL;

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

static AVLTreeNode *create_avltree_node(DS_AVLTREE_TYPE value)
{
    AVLTreeNode *node = (AVLTreeNode *)malloc(sizeof(AVLTreeNode));
    if (node == NULL)
    {
        return NULL;
    }

    int clone_judge = 1;
    node->data = DS_AVLTREE_CLONE_ELEMENT(value, &clone_judge);
    if (!clone_judge)
    {
        DS_AVLTREE_DESTROY_ELEMENT(node->data);
        free(node);
        return NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->height = 1;

    return node;
}

DS_AVLTree *ds_avltree_clone(const DS_AVLTree *tree, int *judge)
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

    DS_AVLTree *new_tree = ds_avltree_create();
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

    new_tree->root = create_avltree_node(tree->root->data);
    if (new_tree->root == NULL)
    {
        ds_avltree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    DS_Queue *q_old = ds_queue_create();
    if (q_old == NULL)
    {
        ds_avltree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }
    DS_Queue *q_new = ds_queue_create();
    if (q_new == NULL)
    {
        ds_queue_destroy(q_old);
        ds_avltree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    if (!ds_queue_enqueue(q_old, tree->root))
    {
        ds_queue_destroy(q_old);
        ds_queue_destroy(q_new);
        ds_avltree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }
    if (!ds_queue_enqueue(q_new, new_tree->root))
    {
        ds_queue_destroy(q_old);
        ds_queue_destroy(q_new);
        ds_avltree_destroy(new_tree);
        *judge = 0;
        return NULL;
    }

    AVLTreeNode *cur_old;
    AVLTreeNode *cur_new;

    while (!ds_queue_is_empty(q_old))
    {
        if (!ds_queue_dequeue(q_old, &cur_old))
        {
            ds_queue_destroy(q_old);
            ds_queue_destroy(q_new);
            ds_avltree_destroy(new_tree);
            *judge = 0;
            return NULL;
        }
        if (!ds_queue_dequeue(q_new, &cur_new))
        {
            ds_queue_destroy(q_old);
            ds_queue_destroy(q_new);
            ds_avltree_destroy(new_tree);
            *judge = 0;
            return NULL;
        }

        if (cur_old->left != NULL)
        {
            if (!ds_queue_enqueue(q_old, cur_old->left))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_avltree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }

            cur_new->left = create_avltree_node((cur_old->left)->data);
            if (cur_new->left == NULL)
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_avltree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
            (cur_new->left)->height = (cur_old->left)->height;
            (cur_new->left)->parent = cur_new;

            if (!ds_queue_enqueue(q_new, cur_new->left))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_avltree_destroy(new_tree);
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
                ds_avltree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }

            cur_new->right = create_avltree_node((cur_old->right)->data);
            if (cur_new->right == NULL)
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_avltree_destroy(new_tree);
                *judge = 0;
                return NULL;
            }
            (cur_new->right)->height = (cur_old->right)->height;
            (cur_new->right)->parent = cur_new;

            if (!ds_queue_enqueue(q_new, cur_new->right))
            {
                ds_queue_destroy(q_old);
                ds_queue_destroy(q_new);
                ds_avltree_destroy(new_tree);
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

int ds_avltree_node_get_data(AVLTreeNode *cursor, DS_AVLTREE_TYPE **x)
{
    if (cursor == NULL || x == NULL)
    {
        return 0;
    }

    *x = &(cursor->data);

    return 1;
}

/* Part 6. Search -----------------------------------------------------------*/

AVLTreeNode *ds_avltree_search(DS_AVLTree *tree, DS_AVLTREE_TYPE value)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    AVLTreeNode *cur = tree->root;

    while (1)
    {
        if (DS_AVLTREE_EQ(cur->data, value))
        {
            return cur;
        }
        else if (DS_AVLTREE_GT(cur->data, value))
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

AVLTreeNode *ds_avltree_find_max(DS_AVLTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    AVLTreeNode *cur = tree->root;

    while (cur->right != NULL)
    {
        cur = cur->right;
    }

    return cur;
}

AVLTreeNode *ds_avltree_find_min(DS_AVLTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }

    AVLTreeNode *cur = tree->root;

    while (cur->left != NULL)
    {
        cur = cur->left;
    }

    return cur;
}

/* Part 8. Cursor Navigation ------------------------------------------------*/

AVLTreeNode *ds_avltree_predecessor(DS_AVLTree *tree, AVLTreeNode *cursor)
{
    if (tree == NULL || tree->root == NULL || cursor == NULL)
    {
        return NULL;
    }

    if (cursor->left != NULL)
    {
        AVLTreeNode *cur = cursor->left;

        while (cur->right != NULL)
        {
            cur = cur->right;
        }

        return cur;
    }
    else
    {
        AVLTreeNode *cur = tree->root;
        AVLTreeNode *prob = NULL;

        while (1)
        {
            if (DS_AVLTREE_LT(cursor->data, cur->data))
            {
                cur = cur->left;
            }
            else if (DS_AVLTREE_GT(cursor->data, cur->data))
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

AVLTreeNode *ds_avltree_successor(DS_AVLTree *tree, AVLTreeNode *cursor)
{
    if (tree == NULL || tree->root == NULL || cursor == NULL)
    {
        return NULL;
    }

    if (cursor->right != NULL)
    {
        AVLTreeNode *cur = cursor->right;

        while (cur->left != NULL)
        {
            cur = cur->left;
        }

        return cur;
    }
    else
    {
        AVLTreeNode *cur = tree->root;
        AVLTreeNode *prob = NULL;

        while (1)
        {
            if (DS_AVLTREE_LT(cursor->data, cur->data))
            {
                prob = cur;

                cur = cur->left;
            }
            else if (DS_AVLTREE_GT(cursor->data, cur->data))
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

int ds_avltree_range_query(DS_AVLTree *tree,
                           DS_AVLTREE_TYPE low,
                           DS_AVLTREE_TYPE high,
                           void *user_data,
                           void (*visit)(DS_AVLTREE_TYPE *value, void *callback_data))
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

    if (DS_AVLTREE_GT(low, high))
    {
        return 1;
    }

    DS_Stack *s = ds_stack_create();
    if (s == NULL)
    {
        return 0;
    }

    AVLTreeNode *cur = tree->root;

    while (1)
    {
        if (cur != NULL)
        {
            if (!ds_stack_push(s, cur))
            {
                ds_stack_destroy(s);
                return 0;
            }

            if (DS_AVLTREE_GE(cur->data, low))
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

                if (DS_AVLTREE_LE(low, cur->data) && DS_AVLTREE_LE(cur->data, high))
                {
                    visit(&(cur->data), user_data);
                }

                if (DS_AVLTREE_LE(cur->data, high))
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

static int abs_int(int num)
{
    return (num >= 0 ? num : -num);
}

static int max_int(int a, int b)
{
    return (a >= b ? a : b);
}

static void update_height(AVLTreeNode *cur)
{
    int lh, rh;
    if (cur->left == NULL)
    {
        lh = 0;
    }
    else
    {
        lh = (cur->left)->height;
    }
    if (cur->right == NULL)
    {
        rh = 0;
    }
    else
    {
        rh = (cur->right)->height;
    }

    cur->height = max_int(lh, rh) + 1;
}

static int children_height_difference(const AVLTreeNode *root)
{
    int l;
    if (root->left == NULL)
    {
        l = 0;
    }
    else
    {
        l = (root->left)->height;
    }

    int r;
    if (root->right == NULL)
    {
        r = 0;
    }
    else
    {
        r = (root->right)->height;
    }

    return l - r;
}

static AVLTreeNode *rotate_left_left(AVLTreeNode *root)
{
    AVLTreeNode *ancestor = root->parent;
    AVLTreeNode *C = root;
    AVLTreeNode *B = C->left;
    AVLTreeNode *temp = B->right;

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

    update_height(C);
    update_height(B);

    return B;
}

static AVLTreeNode *rotate_right_right(AVLTreeNode *root)
{
    AVLTreeNode *ancestor = root->parent;
    AVLTreeNode *A = root;
    AVLTreeNode *B = A->right;
    AVLTreeNode *temp = B->left;

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

    update_height(A);
    update_height(B);

    return B;
}

static AVLTreeNode *rotate_left_right(AVLTreeNode *root)
{
    AVLTreeNode *ancestor = root->parent;
    AVLTreeNode *C = root;
    AVLTreeNode *A = C->left;
    AVLTreeNode *B = A->right;
    AVLTreeNode *temp1 = B->left;
    AVLTreeNode *temp2 = B->right;

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

    update_height(A);
    update_height(C);
    update_height(B);

    return B;
}

static AVLTreeNode *rotate_right_left(AVLTreeNode *root)
{
    AVLTreeNode *ancestor = root->parent;
    AVLTreeNode *A = root;
    AVLTreeNode *C = A->right;
    AVLTreeNode *B = C->left;
    AVLTreeNode *temp1 = B->left;
    AVLTreeNode *temp2 = B->right;

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

    update_height(A);
    update_height(C);
    update_height(B);

    return B;
}

static AVLTreeNode *ds_avltree_insert_delete_fixup(AVLTreeNode *root, AVLTreeNode *cur, const char c)
{
    while (cur != NULL)
    {
        update_height(cur);

        if (abs_int(children_height_difference(cur)) >= 2)
        {
            AVLTreeNode *temp = NULL;

            if (children_height_difference(cur) > 0) /* ll or lr or l_ */
            {
                if (children_height_difference(cur->left) >= 0) /* ll or l_ */
                {
                    temp = rotate_left_left(cur);
                }
                else /* lr */
                {
                    temp = rotate_left_right(cur);
                }
            }
            else /* rl or rr or r_ */
            {
                if (children_height_difference(cur->right) <= 0) /* rr or r_ */
                {
                    temp = rotate_right_right(cur);
                }
                else /* rl */
                {
                    temp = rotate_right_left(cur);
                }
            }

            if (temp->parent == NULL)
            {
                root = temp;
            }

            if (c == 'i') /* insert: one rotation suffices */
            {
                break;
            }
        }

        cur = cur->parent;
    }

    return root;
}

int ds_avltree_insert(DS_AVLTree *tree, DS_AVLTREE_TYPE value)
{
    if (tree == NULL)
    {
        return 0;
    }

    if (tree->root == NULL)
    {
        AVLTreeNode *new_node = create_avltree_node(value);
        if (new_node == NULL)
        {
            return 0;
        }

        tree->root = new_node;

        return 1;
    }

    AVLTreeNode *cur = tree->root;

    while (1)
    {
        if (DS_AVLTREE_LT(value, cur->data))
        {
            if (cur->left != NULL)
            {
                cur = cur->left;
            }
            else
            {
                AVLTreeNode *new_node = create_avltree_node(value);
                if (new_node == NULL)
                {
                    return 0;
                }

                cur->left = new_node;
                new_node->parent = cur;

                break;
            }
        }
        else if (DS_AVLTREE_GT(value, cur->data))
        {
            if (cur->right != NULL)
            {
                cur = cur->right;
            }
            else
            {
                AVLTreeNode *new_node = create_avltree_node(value);
                if (new_node == NULL)
                {
                    return 0;
                }

                cur->right = new_node;
                new_node->parent = cur;

                break;
            }
        }
        else
        {
            return 0;
        }
    }

    char c = 'i';
    tree->root = ds_avltree_insert_delete_fixup(tree->root, cur, c);

    return 1;
}

int ds_avltree_delete(DS_AVLTree *tree, DS_AVLTREE_TYPE value)
{
    if (tree == NULL || tree->root == NULL)
    {
        return 0;
    }

    AVLTreeNode *cur = tree->root;

    while (1)
    {
        if (DS_AVLTREE_LT(value, cur->data))
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
        else if (DS_AVLTREE_GT(value, cur->data))
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
            if (cur->left != NULL && cur->right != NULL) /* two children */
            {
                AVLTreeNode *temp = cur->right;
                while (temp->left != NULL)
                {
                    temp = temp->left;
                }

                DS_AVLTREE_TYPE tmp_data = cur->data;
                cur->data = temp->data;
                temp->data = tmp_data;

                AVLTreeNode *temp_parent = temp->parent;

                if (temp_parent->left == temp)
                {
                    temp_parent->left = temp->right;
                }
                else
                {
                    temp_parent->right = temp->right;
                }

                if (temp->right != NULL)
                {
                    temp->right->parent = temp_parent;
                }

                DS_AVLTREE_DESTROY_ELEMENT(temp->data);
                free(temp);

                cur = temp_parent;

                break;
            }

            if (cur == tree->root)
            {
                if (cur->left == NULL && cur->right == NULL)
                {
                    tree->root = NULL;

                    DS_AVLTREE_DESTROY_ELEMENT(cur->data);
                    free(cur);

                    return 1;
                }
                else
                {
                    if (cur->left != NULL)
                    {
                        tree->root = cur->left;
                    }
                    else
                    {
                        tree->root = cur->right;
                    }

                    tree->root->parent = NULL;

                    DS_AVLTREE_DESTROY_ELEMENT(cur->data);
                    free(cur);

                    return 1;
                }
            }
            else
            {
                AVLTreeNode *par = cur->parent;

                if (cur->left == NULL && cur->right == NULL)
                {
                    if (par->left == cur)
                    {
                        par->left = NULL;
                    }
                    else
                    {
                        par->right = NULL;
                    }

                    DS_AVLTREE_DESTROY_ELEMENT(cur->data);
                    free(cur);
                }
                else
                {
                    AVLTreeNode *child = NULL;
                    if (cur->left != NULL)
                    {
                        child = cur->left;
                    }
                    else
                    {
                        child = cur->right;
                    }

                    if (par->left == cur)
                    {
                        par->left = child;
                    }
                    else
                    {
                        par->right = child;
                    }

                    child->parent = par;

                    DS_AVLTREE_DESTROY_ELEMENT(cur->data);
                    free(cur);
                }

                cur = par;
            }

            break;
        }
    }

    char c = 'd';
    tree->root = ds_avltree_insert_delete_fixup(tree->root, cur, c);

    return 1;
}
