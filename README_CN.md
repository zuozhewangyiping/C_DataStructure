# C_DataStructure

用纯 C 语言对标 C++ STL 的教学级数据结构库。

C 语言缺少模板、RAII 和迭代器——STL 的三大基础。本库用纯 C 手段逐一给出了替代方案：

- **宏泛型 + 代码生成器** 替代模板，实现编译期单态化。
- **`DESTROY_ELEMENT` / `CLONE_ELEMENT` 宏** 替代析构与拷贝构造，容器自动管理元素生命周期。
- **不透明游标 + 索引访问** 替代迭代器。

在此基础上，14 个容器提供了 `push_back` / `pop_back` / `insert` / `erase` / `find` 等统一的接口命名。同时包含磁盘页 B+ 树与哈夫曼压缩算法。

---

## 目录

- [数据结构一览](#数据结构一览)
- [快速上手](#快速上手)
- [多类型支持（代码生成器）](#多类型支持代码生成器)
- [核心设计理念](#核心设计理念)
- [API 目录](#api-目录)
- [进阶功能](#进阶功能)
- [项目结构导读](#项目结构导读)
- [实现与设计考量](#实现与设计考量)
- [与其他方案对比](#与其他方案对比)
- [许可](#许可)

---

## 数据结构一览

本库包含 14 个数据结构和 1 个算法，分为四层：

### 1. 序列型

元素按插入顺序排列，支持遍历操作。

#### 连续存储

| 容器 | 定位 | 插入 | 删除 | 访问 | 说明 |
|------|------|:--:|:--:|:--:|------|
| **DynamicArray** | 泛型动态数组 | O(1)\* / O(n) | O(1)\* / O(n) | O(1) 索引 | 类比 C++ `std::vector` |
| **String** | 动态字符串 | O(1)\* / O(n) | O(n) | O(1) 索引 | 支持比较、子串、拼接、C 字符串互转 |
| **Deque** | 双端队列 | O(1)\* | O(1) | O(1) 索引 | 循环数组，支持随机位置插入/删除 |

> O(1)\* = 均摊 O(1)。

#### 节点存储

| 容器 | 定位 | 插入 | 删除 | 访问 | 说明 |
|------|------|:--:|:--:|:--:|------|
| **SinglyLinkedList** | 单向链表 | O(1) 头 / O(n) | O(1) 头 / O(n) | O(n) 索引 | 类比 C++ `std::forward_list` |
| **DoubleLinkedList** | 双向链表 | O(1) 头尾 / O(n) | O(1) 头尾 / O(n) | O(n) 索引 | 类比 C++ `std::list`，支持双向游标操作 |

#### 受限序列

| 容器 | 定位 | 核心操作 | 说明 |
|------|------|------|------|
| **Stack** | 栈（LIFO） | O(1) push / pop / peek | 基于动态数组 |
| **Queue** | 队列（FIFO） | O(1) enqueue / dequeue | 循环缓冲区 |

### 2. 关联型

元素按 key 组织和查找，而非插入顺序。

#### 散列存储

| 容器 | 定位 | 平均查找 | 说明 |
|------|------|:--:|------|
| **HashTable** | 哈希表 | O(1) | 拉链法，FNV-1a 哈希，支持 insert（拒绝重复）/ put（覆盖） |

#### 排序树

| 容器 | 定位 | 查找 | 插入 | 删除 | 说明 |
|------|------|:--:|:--:|:--:|------|
| **AVLTree** | 严格平衡 BST | O(log n) | O(log n) | O(log n) | LL/LR/RL/RR 四种旋转，高度差 ≤1 |
| **RedBlackTree** | 红黑树 | O(log n) | O(log n) | O(log n) | 插入最多两次旋转，删除更复杂 |

#### 概率排序

| 容器 | 定位 | 期望查找 | 说明 |
|------|------|:--:|------|
| **SkipList** | 跳表 | O(log n) | 32 层概率平衡，实现简洁 |

### 3. 堆型

只关心极值，不关心全局顺序。

| 容器 | 定位 | push | pop | peek | 说明 |
|------|------|:--:|:--:|:--:|------|
| **PriorityQueue（Max）** | 最大堆 | O(log n) | O(log n) | O(1) | 二叉堆实现 |
| **PriorityQueue（Min）** | 最小堆 | O(log n) | O(log n) | O(1) | 与 Max 独立实现，比较方向相反 |

### 4. 持久型

数据持久化于磁盘（运行时通过 `fread` / `fwrite` 整页读写）。

| 容器 | 定位 | 查找 | 插入 | 删除 | 说明 |
|------|------|:--:|:--:|:--:|------|
| **BPlusTree** | 磁盘 B+ 树 | O(log n) | O(log n) | O(log n) | 4KB 页，文件持久化，废弃页回收 |

### 5. 算法

| 算法 | 说明 |
|------|------|
| **HuffmanCoding** | 哈夫曼压缩，内嵌最小堆建树，8 比特位打包，支持编解码往返 |

## 快速上手

### 1. 零门槛体验

```bash
git clone https://github.com/zuozhewangyiping/C_DataStructure.git
cd C_DataStructure
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && ./demo
```

零依赖，无构建系统，只有 `.c` 和 `.h` 文件。**要求 C99 或更高版本。**

### 2. 写第一个程序

```c
// main.c
#include <stdio.h>
#include "DynamicArray/ds_dynamicarray.h"

int main(void)
{
    DS_DynamicArray *da = ds_dynamicarray_create();

    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){10});
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){20});
    ds_dynamicarray_push_back(da, (ds_dynamicarray_type){30});

    printf("size: %d\n", ds_dynamicarray_size(da));

    DS_DYNAMICARRAY_TYPE *p;
    for (int i = 0; i < ds_dynamicarray_size(da); i++) {
        ds_dynamicarray_get(da, i, &p);
        printf("da[%d] = %d\n", i, p->data);
    }

    ds_dynamicarray_destroy(da);
    return 0;
}
```

```bash
gcc -o main main.c DynamicArray/ds_dynamicarray.c && ./main
```

默认元素类型是 `{ int data }`。到此为止不需要了解任何泛型机制。

### 3. 配置元素类型

如果默认的 `int` 不满足需求，编辑对应容器的 `ds_xxx_type.h`。这是你唯一需要修改的文件：

```c
// ds_dynamicarray_type.h
typedef struct {
    int   id;
    char *name;        // 堆分配字段
    double score;
} ds_dynamicarray_type;

// 销毁：释放 name
static inline void destroy_element(ds_dynamicarray_type *e) {
    free(e->name);
    e->name = NULL;
}
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) destroy_element(&(e))

// 克隆：深拷贝 name
static inline ds_dynamicarray_type
clone_element(const ds_dynamicarray_type *src, int *judge) {
    ds_dynamicarray_type copy = { .id = src->id, .score = src->score, .name = NULL };
    if (src->name) {
        copy.name = strdup(src->name);
        if (!copy.name) { *judge = 0; return copy; }
    }
    return copy;
}
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, j) clone_element(&(e), j)

// 匹配：按 id 查找
#define DS_DYNAMICARRAY_MATCH_TYPE int
static inline int match_element(const ds_dynamicarray_type *e,
                                DS_DYNAMICARRAY_MATCH_TYPE target) {
    return e->id == target ? 1 : 0;
}
#define DS_DYNAMICARRAY_MATCH(e, t) match_element(&(e), t)
```

### 4. 使用

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = { 1, strdup("Alice"), 95.5 };
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);   // 容器已深拷贝，释放自己的原值

    DS_DYNAMICARRAY_TYPE tmp2 = { 2, strdup("Bob"), 87.0 };
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);    // 2

    ds_dynamicarray_destroy(arr);            // 自动释放所有元素和容器
    return 0;
}
```

> **注意：调用者负责清理传入的原始数据。**
>
> 容器在 `push` / `insert` / `set` 时通过 `CLONE_ELEMENT` 深拷贝传入的数据。原始数据的所有权仍属于调用者——如果元素类型含堆字段（如 `strdup` 产生的 `char *`），调用者须在操作完成后调用 `DESTROY_ELEMENT` 清理自身持有的副本。容器不会释放调用者传入的堆资源。
>
> 如果元素类型仅含标量字段，直接传入字面量即可，无需清理。

### 5. 编译

无需构建系统，直接将需要的 `.c` 文件加入编译：

```bash
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# 多容器混用
gcc -o myapp myapp.c \
    DynamicArray/ds_dynamicarray.c \
    HashTable/ds_hashtable.c \
    String/ds_string.c
```

所有容器互相独立——只编译你需要的 `.c` 文件即可。

---

## 多类型支持（代码生成器）

### 为什么需要代码生成器

`ds_xxx_type.h` 中的宏是全局的——一个编译单元内只能定义一种 `ds_dynamicarray_type`。如果你需要在一个程序里同时使用 `DynamicArray<Student>` 和 `DynamicArray<Course>`，直接多次 `#include` 会因类型名和宏名冲突而无法编译。

这是 C 语言宏泛型的固有限制。代码生成器通过标识符批量重命名来突破这个限制。

### 使用流程

```bash
# 1. 在母版 ds_dynamicarray_type.h 中定义 Student 类型（同上节）
# 2. 运行脚本
cd DynamicArray
python generate.py student     # 生成 ds_dynamicarray_student_type.h / .h / .c

# 3. 修改 _type.h 为 Course 类型，再生成一份
python generate.py course      # 生成 ds_dynamicarray_course_type.h / .h / .c
```

### 在代码中使用

```c
#include "ds_dynamicarray_student.h"
#include "ds_dynamicarray_course.h"

int main() {
    // Student 数组
    DS_DynamicArray_Student *roster = ds_dynamicarray_student_create();
    DS_DYNAMICARRAY_STUDENT_TYPE s = { 1, strdup("Alice"), 95.5 };
    ds_dynamicarray_student_push_back(roster, s);
    DS_DYNAMICARRAY_STUDENT_DESTROY_ELEMENT(s);

    // Course 数组 —— 完全独立的类型
    DS_DynamicArray_Course *courses = ds_dynamicarray_course_create();
    DS_DYNAMICARRAY_COURSE_TYPE c = { 1001, strdup("Math"), 4 };
    ds_dynamicarray_course_push_back(courses, c);
    DS_DYNAMICARRAY_COURSE_DESTROY_ELEMENT(c);

    ds_dynamicarray_student_destroy(roster);
    ds_dynamicarray_course_destroy(courses);
    return 0;
}
```

```bash
gcc -o app main.c ds_dynamicarray_student.c ds_dynamicarray_course.c
```

### 单类型场景

如果不需要在同一编译单元内对同一容器使用多种元素类型，完全忽略 `generate.py` 即可。原有的"编辑 `_type.h` → 编译 `.c`"流程不受任何影响。

---

## 核心设计理念

### 宏泛型机制

```
┌──────────────────────────────┐
|  ds_xxx_type.h   （用户编辑） |
|  定义元素 struct  + 三大宏:   |
|  DESTROY  ← 析构             |
|  CLONE    ← 拷贝             |
|  MATCH    ← 查找             |
└─────────────┬────────────────┘
              │ #include
┌─────────────▼────────────────┐
|  ds_xxx.h       （公有 API）  |
|  #define DS_XXX_TYPE 为别名   |
|  所有函数签名使用宏名          |
└─────────────┬────────────────┘
              │
┌─────────────▼────────────────┐
|  ds_xxx.c       （不改）      |
|  仅通过宏引用元素类型          |
|  算法代码与具体类型完全解耦    |
└──────────────────────────────┘
```

与 C++ 模板的 monomorphization 本质相同——都是为每种类型生成独立的类型安全代码。区别在于 C++ 编译器自动完成，本库用 `generate.py` 脚本手动触发。

### 深拷贝所有权模型

所有 `push` / `insert` / `set` / `clone` 操作通过 `CLONE_ELEMENT` 执行深拷贝。容器拥有其内部副本的完整所有权，`destroy` 时自动逐个调用 `DESTROY_ELEMENT`。调用者保有原始数据的所有权——没有"容器可能帮你释放也可能不释放"的模糊地带。

### 两类容器，两类 erase 返回值

#### 数组型（DynamicArray / Deque / Stack / Queue）

数据存储在连续内存中。`erase` / `pop` 返回的指针指向这块内存内部或越界位置，不是独立的堆块。**绝对不能 `free()`。** 下次 `push` / `insert` / `reserve` 时该位置被覆盖，指针随即失效。

```c
DS_DYNAMICARRAY_TYPE *p;
ds_dynamicarray_erase(da, 0, &p);
DS_DYNAMICARRAY_DESTROY_ELEMENT(*p);   // 清理元素内部堆资源（如有）
// 不能 free(p) —— p 指向数组内部
```

#### 节点型（SinglyLinkedList / DoubleLinkedList / HashTable）

数据存储在独立 `malloc` 的节点中。`erase` / `pop` 将节点数据浅拷贝到新 `malloc` 的堆块，释放原节点，返回新堆块的指针。**用完必须 `DESTROY_ELEMENT` 再 `free`。**

```c
DS_HASHTABLE_TYPE *p;
ds_hashtable_erase(ht, 100, &p);
DS_HASHTABLE_DESTROY_ELEMENT(*p);   // 先清理元素内部堆资源
free(p);                            // 再释放元素本身
```

#### `_and_destroy` 变体

所有 `erase` / `pop` / `dequeue` 均提供 `_and_destroy` 变体，由容器负责清理工作。

#### 树型 cursor

树容器的 cursor（`AVLTreeNode *` 等）直接指向树内部节点——绝对不能 `free`。B+ 树的 cursor 是 `malloc` 的副本——必须 `free`。

### 错误处理

| 返回 | 含义 |
|:--:|------|
| `1` | 成功 |
| `0` | 失败：NULL 参数、越界、空容器、malloc 失败、未找到、重复 key |
| `-1` | 查询函数（`size` / `capacity` / `is_empty`）收到 NULL 容器 |
| `NULL` + `judge=1` | `clone` 成功（源为 NULL） |
| `NULL` + `judge=0` | `clone` 失败（malloc 失败） |

### 命名规范

```
ds_<模块>_<操作>[_<变体>]

例: ds_dynamicarray_push_back
    ds_avltree_insert
    ds_hashtable_erase_and_destroy
```

操作名跨容器尽量统一，具体差异见下章。

---

## API 目录

以下按操作类别列出统一的接口后缀。读者可根据规律自行推断任何容器的对应函数名。

### 说明

表格列头沿用"数据结构一览"中的分类。各简称对应容器如下：

| 简称 | 容器 |
|------|------|
| 序列-连续 | DynamicArray, String, Deque |
| 序列-节点 | SinglyLinkedList, DoubleLinkedList |
| 序列-受限 | Stack, Queue |
| 关联-散列 | HashTable |
| 关联-树 | AVLTree, RedBlackTree, SkipList |
| 堆 | PriorityQueue |
| 持久 | BPlusTree |

表格中的 ● = 该类容器全部支持。标记具体容器名（如 `Deque`、`AVL,RB`）表示同类内部存在不对齐情况，不宜统标 ●。

表格不列完整函数签名——统一前缀规则 `ds_<模块>_` 加上后缀即可得到完整函数名。

### 创建与销毁

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 | 算法 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `create` / `destroy` | ● | ● | ● | ● | ● | ● | ● | ● |
| `build` | | | | | | | | ● |

### 状态查询

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `size` | ● | ● | ● | ● | SkipList | ● | ● |
| `capacity` | ● | | ● | ● | | ● | |
| `is_empty` | ● | ● | ● | ● | ● | ● | ● |
| `height` | | | | | AVL,RB | | |
| `count` | | | | | AVL,RB | | |
| `level` | | | | | SkipList | | ● |

### 插入

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `push_back` | ● | ● | Deque | | | | |
| `push_front` | Deque | ● | Deque | | | | |
| `push` | | | Stack | | | ● | |
| `enqueue` | | | Queue | | | | |
| `insert` | ● | ● | Deque | ● | ● | | ● |

### 删除

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `pop_back` / `_and_destroy` | ● | ● | Deque | | | | |
| `pop_front` / `_and_destroy` | Deque | ● | Deque | | | | |
| `pop` / `_and_destroy` | | | Stack | | | ● | |
| `dequeue` / `_and_destroy` | | | Queue | | | | |
| `erase` / `_and_destroy` | ● | ● | Deque | ● | | | |
| `delete` | | | | | ● | | ● |

### 访问

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `get` | ● | ● | Deque | | | | |
| `set` | ● | ● | Deque | | | | |
| `peek` | | | Stack | | | ● | |
| `peek_front` / `peek_back` | | | Queue,Deque | | | | |
| `node_get_data` | | ● | | | ● | | ● |

### 查找

| 后缀 | 序列-连续 | 序列-节点 | 关联-散列 | 关联-树 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|
| `find` | ● | ● | ● | | |
| `search` | | ● | | ● | ● |
| `find_min` / `find_max` | | | | ● | ● |
| `successor` / `predecessor` | | | | ● | ● |

### 遍历

| 后缀 | 序列-连续 | 序列-节点 | 关联-散列 | 关联-树 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|
| `traverse` | ● | ● | ● | SkipList | ● |
| `begin` / `next` | | ● | | ● | ● |
| `prev` / `rbegin` | | DoubleLL | | AVL,RB | |
| `_preorder` / `_inorder` / `_postorder` / `_levelorder` | | | | AVL,RB | |
| `range_query` | | | | ● | ● |

### 容量管理

| 后缀 | 序列-连续 | 序列-受限 | 关联-散列 | 堆 |
|------|:--:|:--:|:--:|:--:|
| `reserve` | ● | ● | ● | ● |
| `shrink_to_fit` | ● | ● | | ● |

### 克隆与合并

| 后缀 | 序列-连续 | 序列-节点 | 序列-受限 | 关联-散列 | 关联-树 | 堆 | 持久 |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `clone` | ● | ● | ● | ● | ● | ● | ● |
| `concat` | ● | ● | | | | | |

### 游标操作变体（仅序列-节点）

| 后缀 | 涵盖操作 | 适用 |
|------|---------|:--:|
| `_after_cursor` / `_and_destroy` | insert / erase | ● |
| `_before_cursor` / `_and_destroy` | insert / erase | DoubleLinkedList |
| `_cursor` / `_and_destroy` | erase | DoubleLinkedList |

### 特化操作

| 后缀 | 适用 | 说明 |
|------|------|------|
| `put` | HashTable | 插入或覆盖（键存在时覆盖，不存在时插入） |
| `compare` | String | 字典序比较 |
| `substring` | String | 提取子串 |
| `cstr_to_string` / `string_to_cstr` | String | C 字符串互转 |
| `encode` / `decode` | HuffmanCoding | 压缩与解压 |

---

## 进阶功能

### 游标迭代（树与链表）

树和链表提供不透明游标类型，避免索引带来的额外遍历开销：

```c
AVLTreeNode *cursor = ds_avltree_find_min(tree);
while (cursor != NULL) {
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key = %d\n", data->key);
    cursor = ds_avltree_successor(cursor);
}
```

### 遍历回调

树容器支持前序、中序、后序、层序四种遍历；链表和哈希表支持单向遍历：

```c
void print_value(DS_AVLTREE_TYPE *value, void *user_data) {
    (void)user_data;
    printf("key = %d, value = %d\n", value->key, value->value);
}
ds_avltree_traverse_inorder_value(tree, NULL, print_value);
```

### 范围查询（树）

```c
ds_avltree_range_query(tree,
    (DS_AVLTREE_TYPE){ .key = 30 },
    (DS_AVLTREE_TYPE){ .key = 70 },
    NULL, visit_callback);
```

### Insert 与 Put 的区别（哈希表）

```c
ds_hashtable_insert(ht, value);   // 键已存在时拒绝，返回 0
ds_hashtable_put(ht, value);      // 键已存在时覆盖，否则插入
```

### 容器嵌套使用（test_project）

`test_project/` 目录展示了两种数据结构的组合使用：用 `DynamicArray` 存储含 `DS_String *` 字段的元素，构建一个学生成绩管理的综合示例。

```c
DS_DynamicArray *roster = ds_dynamicarray_create();

DS_String *name1 = ds_cstr_to_string("Alice");
ds_dynamicarray_push_back(roster,
    (DS_DYNAMICARRAY_TYPE){ .name = name1, .id = 2025001, .score = 95 });

DS_String *name2 = ds_cstr_to_string("Bob");
ds_dynamicarray_push_back(roster,
    (DS_DYNAMICARRAY_TYPE){ .name = name2, .id = 2025002, .score = 87 });

ds_dynamicarray_destroy(roster);   // 递归销毁嵌套的 DS_String
```

它演示了当容器的元素包含其他堆资源时，`DESTROY_ELEMENT` / `CLONE_ELEMENT` 宏如何通过调用 `ds_string_destroy` / `ds_string_clone` 正确处理嵌套容器的生命周期。

---

## 项目结构导读

```
C-DataStructure/
│
├── DynamicArray/                ← 建议从这里开始
│   ├── ds_dynamicarray_type.h   ← 用户可编辑的元素类型
│   ├── ds_dynamicarray.h        ← 公有 API
│   ├── ds_dynamicarray.c        ← 实现
│   ├── main.c                   ← 演示
│   └── generate.py              ← 多类型代码生成器
│
├── SinglyLinkedList/            ← 然后链表与游标
├── DoubleLinkedList/
├── Stack/
├── Queue/
├── Deque/
├── PriorityQueue/               ← 两个独立实现：generate_max.py / generate_min.py
│
├── HashTable/
├── AVLTree/                     ← 进入自平衡树
├── RedBlackTree/
├── SkipList/
├── BPlusTree/                   ← 磁盘存储，最大的模块
├── String/
│
├── Algorithms/
│   └── HuffmanCoding/
│
└── test_project/                ← 综合集成 Demo
```

### 推荐学习顺序

| 阶段 | 模块 | 目的 |
|------|------|------|
| 1 | **DynamicArray** | 理解动态数组、倍增扩容、宏泛型基础 |
| 2 | **String** | 理解同一动态数组机制在固定类型（`char`）下如何褪去泛型层，获得简洁接口与字符串专属操作 |
| 3 | **SinglyLinkedList** → **DoubleLinkedList** | 理解指针结构、不透明游标、回调遍历 |
| 4 | **Stack** → **Queue** → **Deque** | 理解受限接口、循环缓冲区 |
| 5 | **PriorityQueue** | 理解二叉堆、sift-up / sift-down |
| 6 | **HashTable** | 理解拉链法、FNV-1a、rehash |
| 7 | **AVLTree** → **RedBlackTree** | 理解自平衡 BST、旋转与染色 |
| 8 | **SkipList** | 理解概率数据结构 |
| 9 | **BPlusTree** | 理解磁盘页格式、分裂与合并、自由链表 |
| 10 | **HuffmanCoding** | 理解数据结构组合构建算法：最小堆建树、位级编码 |
| 11 | **test_project** | 理解多容器组合、级联所有权、真实项目结构 |

---

## 实现与设计考量

### 宏的可移植性

`ds_xxx_type.h` 中默认使用 `static inline` 函数实现克隆和销毁逻辑，再由宏调用。兼容所有 C99 编译器（GCC、Clang、MSVC），且支持断点调试。

要点：

- **`static`** 避免被多个 `.c` 文件包含时出现符号重复定义。
- **`inline`** 允许编译器消除调用开销。
- 宏的调用语法保持不变——修改 `_type.h` 后，所有 `.c` / `.h` 实现文件无需任何改动。
- 核心不变式：**`_type.h` 是用户唯一需要修改的文件。**（若使用代码生成器，同样只需编辑母版 `_type.h`，再运行脚本。）
- 磁盘类容器例外：B+ 树的 key/value 为定长 POD 类型，通过 `fread` / `fwrite` 整页读写，不使用 CLONE/DESTROY 宏。

### 扩容策略

所有基于数组的容器采用翻倍扩容策略：初始容量 0 → 首次插入变为 1 → 之后每次翻倍。哈希表在 `size >= capacity`（负载因子 1.0）时触发 rehash，桶数翻倍。

### 设计取舍

- **不用 `void*` 擦除类型。** 泛型通过 `_type.h` 中的宏实现，编译期类型安全。宏的天然限制（一编译单元一种类型）由 `generate.py` 代码生成器解决。
- **只靠返回值报告错误。** 不使用 `errno`、`assert` 或 `exit`。所有错误通过返回值体现——无隐藏控制流。
- **单线程。** 无锁、无原子操作，假设单线程环境。
- **无构建系统。** 每个数据结构就是一组独立的 `.c` / `.h` 文件，不需要 Makefile 或 CMake。
- **不透明结构体。** 内部字段对用户隐藏，所有操作均为直接函数调用——无虚表开销。

---

## 与其他方案对比

| 方案 | 类型安全 | 所有权清晰度 | 上手难度 | 适用场景 |
|------|:--:|:--:|:--:|------|
| **本库** | 编译期（宏） | 深拷贝模型，边界明确 | 中等 | 学习数据结构与 C 泛型设计；小型项目可参考实现 |
| `C++ STL` | 编译期（模板） | RAII | 低（如果会用 C++） | 能用 C++ 编译器的场景 |
| `glib` / `libuv` 等 | 无（void* + 强转） | 易出错（"谁释放？"无标准答案） | 低 | 快速内部原型 |
| `klib` / `uthash` | 宏（纯头文件） | 因库而异，无统一约定 | 中等 | 极简、只需头文件、不介意宏调试的场景 |
| 手写裸结构体 | 编译期 | 手动管理 | 入门低，做对难 | 一次性、简单场景 |

**简言之：** 如果你在写 C、想通过可读的源码理解泛型容器与数据结构的设计、在意清晰的所有权语义，并且愿意为每个容器配置一个 `_type.h` 文件——那么这个库就是为你准备的。

---

## 许可

本项目基于 MIT 许可证发布，详见 [LICENSE](LICENSE) 文件。

---

*如果你读完这份 README 想要深入代码细节，任何一个模块的 `_type.h` 和 `.h` 文件都是最好的入口——前者包含类型定制示例，后者包含详尽的函数级使用文档和所有权语义说明。*
