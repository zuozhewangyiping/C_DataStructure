# C语言数据结构库 / C Data Structure Library

一个面向学习但功能完备的C语言数据结构库，通过编译期宏展示如何设计类型安全的泛型容器，涵盖三文件扩展模型、深拷贝所有权、不透明指针API等设计要点。

A learning-oriented yet fully functional C library that demonstrates type-safe, generic data structure design in C — from compile-time macros and the three-file extension model to deep-copy ownership and opaque pointer APIs.

---

# 中文

## 概述

本库提供 12 种基础数据结构，每种都自包含、互不依赖——你只需要引入实际使用的那些文件。零依赖，无构建系统，只有 `.c` 和 `.h`。

### 设计理念

C 语言没有模板、没有 RAII、没有析构函数。在 C 里写一个泛型且内存安全的容器，必须正面处理那些高级语言替你掩盖了的设计问题。

本库的核心设计决策：

- **泛型通过编译期宏实现，而非 `void*`。** 专用的 `_type.h` 文件集中管理元素类型和操作——可审查、可调试、不依赖外部工具。
- **所有插入操作执行深拷贝，容器全权拥有其数据。** 调用者保留原数据的所有权，消除了 `void*` 方案中"谁来释放"的灰色地带。
- **不透明结构体隐藏实现细节。** 头文件中仅暴露前向声明，用户无法访问内部字段。所有操作均为直接函数调用，无虚表、无运行时开销。
- **统一的返回值约定处理所有错误。** 成功 1、失败 0、NULL 入参 -1——贯穿所有容器、所有函数，无例外。

结果是：一个简洁到能通读学习的库，同时也扎实到能投入真实项目。

### 数据结构一览

#### 数组类（连续存储）

| 结构 | 说明 |
|---|---|
| **DynamicArray** | 泛型动态数组（类比 C++ 的 `std::vector`） |
| **Stack** | 基于动态数组的 LIFO 栈 |
| **Deque** | 基于循环数组的双端队列，两端 O(1) 入队/出队 |
| **Queue** | 基于循环数组的 FIFO 队列，O(1) 入队/出队 |
| **String** | 面向字符的动态数组，提供字符串专属操作（比较、子串、拼接、C字符串互转） |

#### 节点类（链式存储）

| 结构 | 说明 |
|---|---|
| **SinglyLinkedList** | 单向链表，支持基于索引和游标的操作 |
| **DoubleLinkedList** | 双向链表，支持正反向遍历和游标插入/删除 |

#### 树类（自平衡二叉搜索树）

| 结构 | 说明 |
|---|---|
| **AVLTree** | AVL 树，LL/LR/RL/RR 四种旋转，节点维护高度 |
| **RedBlackTree** | 红黑树，标准插入/删除修正规则 |

#### 专用容器

| 结构 | 说明 |
|---|---|
| **PriorityQueue (Min)** | 最小堆，二叉堆实现 |
| **PriorityQueue (Max)** | 最大堆，二叉堆实现 |
| **HashTable** | 哈希表，链地址法（桶内单向链表），FNV-1a 哈希 |

### 目录结构

```
C_DataStructure_src/
├── DynamicArray/
│   ├── ds_dynamicarray.h          # 公共接口（含详细中文注释）
│   ├── ds_dynamicarray.c          # 实现
│   ├── ds_dynamicarray_type.h     # 元素类型配置（扩展点）
│   └── main.c                     # 使用示例
├── Stack/                          # 同上布局
├── Deque/                          # 同上
├── SinglyLinkedList/               # 同上
├── DoubleLinkedList/               # 同上
├── AVLTree/                        # 同上
├── RedBlackTree/                   # 同上
├── PriorityQueue/                  # 包含最小堆和最大堆两个独立实现
├── HashTable/                      # 同上布局
├── Queue/                          # 同上布局
├── String/                         # 同上布局
└── test_project/                   # 组合示例：DynamicArray + String
```

## 设计理念

### 三文件模式

每个数据结构严格遵循三文件布局：

1. **`ds_<name>.h`** — 公共头文件。声明不透明结构体类型和所有 API 函数。内含详尽的中文注释和用法示例。

2. **`ds_<name>.c`** — 实现文件。定义内部结构体（对用户不可见），实现所有功能。内部辅助函数用 `static` 修饰。

3. **`ds_<name>_type.h`** — **类型配置扩展点。** 用户在此定义：
   - 元素结构体（`ds_<name>_type`）
   - `DESTROY_ELEMENT` 宏 — 释放元素内部的堆资源
   - `CLONE_ELEMENT` 宏 — 深拷贝元素
   - `MATCH` 宏 — 相等判定（用于顺序容器）
   - `_LT`、`_GT`、`_EQ`、`_LE`、`_GE` 比较宏（用于树和堆）
   - `HASH` 宏（用于哈希表）

每个 `_type.h` 末尾都有一段被注释掉的修改示例，展示当元素含有 `char *name` 等堆分配字段时如何修改宏。

### 命名规范

所有公共符号统一使用 `ds_` 前缀：

```
ds_<容器>_<操作>
```

例如：`ds_dynamicarray_push_back`、`ds_avltree_insert`、`ds_hashtable_find`

### 不透明类型

内部结构体定义对用户隐藏，只能通过函数调用操作容器：

```c
typedef struct DS_DynamicArray DS_DynamicArray;  // .h 中的前向声明
struct DS_DynamicArray { ... };                  // .c 中的实际定义，用户不可见
```

## API 约定

### 返回值规范

| 类别 | 返回值 |
|---|---|
| `create` / `clone` | 成功返回指针，失败返回 `NULL` |
| `destroy` | `void`（允许传入 `NULL`） |
| 查询（`size`、`capacity`、`is_empty`） | 成功返回非负数，传入 `NULL` 返回 `-1` |
| 修改（`push`、`insert`、`set`、`erase`...） | 成功返回 `1`，失败返回 `0` |
| 元素获取（`get`、`find`、`peek`、`pop`） | 成功返回 `1`，失败返回 `0`；元素通过输出参数返回 |
| 树游标（`search`、`find_min`、`successor`...） | 成功返回游标指针，未找到/遍历结束返回 `NULL` |

### 输出参数模式

返回元素的函数使用**二级指针**作为输出参数：

```c
DS_DYNAMICARRAY_TYPE *data;
if (ds_dynamicarray_get(array, 0, &data)) {
    printf("value = %d\n", data->value);
    data->value = 999;  // 可直接修改
}
```

## 两类容器，两种 erase 返回值约定

本库的容器分为两类，它们的 `erase` / `pop` 返回值的生命周期不同：

**数组类** — DynamicArray、Deque、Stack、Queue

数据存储在连续内存中。`erase` 返回的指针 **直接指向这块内存内部**，不是独立堆块。
下次 `push` / `insert` 时该位置会被覆盖，指针随即失效。

```c
DS_DYNAMICARRAY_TYPE *data;
ds_dynamicarray_erase(da, 0, &data);

DS_DYNAMICARRAY_DESTROY_ELEMENT(*data);  // 清理元素内部的堆资源（如有）
// ⚠ 不能 free(data) —— 它指向数组内部
```

**节点类** — SinglyLinkedList、DoubleLinkedList、HashTable

数据存储在独立 `malloc` 的节点中。`erase` 会将节点内的数据 **浅拷贝到一个新 `malloc` 的堆块**，
释放原节点，然后把新堆块的指针返回给你。你用完必须手动 `free(data)`。

```c
DS_HASHTABLE_TYPE *data;
ds_hashtable_erase(ht, 100, &data);

DS_HASHTABLE_DESTROY_ELEMENT(*data);  // 清理元素内部的堆资源（如有）
free(data);                           // ⚠ 必须 free —— 它是 malloc 分配的新堆块
```

### `_and_destroy` 变体

不想手动管理上述清理流程？直接使用 `_and_destroy` 变体（如 `pop_back_and_destroy`、
`erase_and_destroy`），容器帮你完成 **DESTROY_ELEMENT + free（节点类）** 的全部工作。

```c
// 数组类自动清理
ds_dynamicarray_pop_back_and_destroy(array);

// 节点类自动清理
ds_hashtable_erase_and_destroy(ht, 100);
```

### NULL 安全

所有公共函数在访问任何字段前都会检查 `NULL` 指针，传入 `NULL` 容器会返回错误码（`0`、`-1` 或 `NULL`），不会发生段错误。

### 深拷贝语义

所有 insert、set、clone 操作都通过 `CLONE_ELEMENT` 宏进行深拷贝。容器拥有其内部副本的所有权；调用者保留原始数据的所有权。

## 快速上手

### 1. 配置元素类型

编辑所选数据结构的 `_type.h` 文件，定义元素结构体和必需的宏：

```c
// ds_dynamicarray_type.h
typedef struct {
    int id;
    char *name;       // 堆分配字段
    double score;
} ds_dynamicarray_type;

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e)  \
    do { free((e).name); (e).name = NULL; } while (0)

#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge)        \
    ({                                                  \
        char *name_copy = (e).name ? strdup((e).name) : NULL; \
        if ((e).name && !name_copy) *(judge) = 0;      \
        (ds_dynamicarray_type){.id = (e).id,            \
                               .name = name_copy,       \
                               .score = (e).score};     \
    })

#define DS_DYNAMICARRAY_MATCH_TYPE int
#define DS_DYNAMICARRAY_MATCH(e, target) ((e).id == target ? 1 : 0)
```

### 2. 使用数据结构

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = {1, strdup("Alice"), 95.5};
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);  // 容器已有深拷贝，释放自己这份

    DS_DYNAMICARRAY_TYPE tmp2 = {2, strdup("Bob"), 87.0};
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);  // 2

    ds_dynamicarray_destroy(arr);  // 释放所有元素和容器本身
    return 0;
}
```

> **重要：清理你自己手上的那份数据**
>
> 容器在 insert / set / push 时会深拷贝你的数据。**你自己传入的那份数据仍然属于你。**
> 如果你的元素类型含堆字段（如 `strdup` 产生的 `char *name`），操作完成后**必须**
> 调用 `DESTROY_ELEMENT` 清理自己手上的副本。否则你分配的堆内存无人释放——容器不
> 会替你 `free`。
>
> 如果你的元素类型只有标量字段（int、double 等），像 `(type){10}` 这样直接传入即
> 可——不存在堆资源，无需手动清理。

### 3. 编译

无需构建系统，直接用任意C编译器编译：

```bash
# 编译单个数据结构及其示例
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# 编译组合测试项目
gcc -o test_project test_project/ds_dynamicarray.c test_project/ds_string.c test_project/main.c
```

所有数据结构互相独立——只编译你需要的 `.c` 文件即可。

## 进阶功能

### 游标迭代（树 & 链表）

树和链表提供不透明游标类型，避免索引带来的额外开销：

```c
// 中序遍历 AVL 树
AVLTreeNode *cursor = ds_avltree_find_min(tree);
while (cursor != NULL) {
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key=%d\n", data->key);
    cursor = ds_avltree_successor(tree, cursor);
}
```

### 遍历回调

树和链表支持通过回调函数遍历：

```c
void print_value(DS_AVLTREE_TYPE *value, void *user_data) {
    printf("key=%d, value=%d\n", value->key, value->value);
}
ds_avltree_traverse_inorder_value(tree, NULL, print_value);
```

### 范围查询（树）

```c
ds_avltree_range_query(tree,
    (DS_AVLTREE_TYPE){.key = 30},
    (DS_AVLTREE_TYPE){.key = 70},
    NULL, visit_callback);
```

### 容量管理（数组类）

```c
ds_dynamicarray_reserve(array, 1000);   // 预留空间，避免反复扩容
ds_dynamicarray_shrink_to_fit(array);   // 回收多余容量
```

### Insert 与 Put 的区别（哈希表）

```c
ds_hashtable_insert(ht, value);  // 键已存在时拒绝，返回 0
ds_hashtable_put(ht, value);     // 键已存在时覆盖，不存在则插入
```

## 扩容策略

所有基于数组的容器采用**翻倍扩容**策略：初始容量 0 → 首次插入变为 1 → 之后每次翻倍。哈希表在 `size >= capacity`（负载因子 = 1.0）时触发 rehash，桶数翻倍。

## 设计取舍

- **不用 `void*` 擦除类型** — 泛型通过 `_type.h` 中的宏实现。编译期类型安全，但修改元素类型后需重新编译。
- **只靠返回值报告错误** — 不使用 `errno`、`assert` 或 `exit`。所有错误通过返回值体现。
- **单线程** — 无锁、无原子操作，假设单线程环境。
- **无构建系统** — 每个数据结构就是一组独立的 `.c`/`.h` 文件，不需要 Makefile 或 CMake。

## 编写可移植的 CLONE / DESTROY 宏

默认的 `CLONE_ELEMENT` 使用 GNU 语句表达式 `({...})` 将克隆逻辑直接嵌入宏体内。这样写很紧凑，但有两个缺点：MSVC 下无法编译，且宏展开后的代码难以逐步调试。

更可移植的替代方案是在 `_type.h` 中编写 **`static inline` 函数**，然后让宏去调用它：

```c
// ds_dynamicarray_type.h

typedef struct {
    int id;
    char *name;
} ds_dynamicarray_type;

// --- 用户编写的 clone / destroy 函数 ---

static inline void destroy_element(ds_dynamicarray_type *e)
{
    free(e->name);
    e->name = NULL;
}

static inline ds_dynamicarray_type
clone_element(const ds_dynamicarray_type *src, int *judge)
{
    ds_dynamicarray_type copy = {.id = src->id, .name = NULL};
    if (src->name) {
        copy.name = strdup(src->name);
        if (!copy.name) { *judge = 0; return copy; }
    }
    return copy;
}

// --- 宏退化为薄封装 ---

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e)  destroy_element(&(e))
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, j) clone_element(&(e), (j))
#define DS_DYNAMICARRAY_MATCH_TYPE          int
#define DS_DYNAMICARRAY_MATCH(e, t)         ((e).id == (t) ? 1 : 0)
```

要点：

- **`static`** 避免 `_type.h` 被多个 `.c` 文件包含时出现符号重复定义。
- **`inline`** 允许编译器消除调用开销——生成的代码与宏版完全相同。
- 宏的**调用语法不变**，所有 `.c` / `.h` 实现文件无需任何修改。用户只改 `_type.h`。
- 此模式适用于**任何 C99 编译器**（GCC、Clang、MSVC）。

两种风格的取舍很明确：`({...})` 宏把所有东西放在一个地方，紧凑；`static inline` 函数多了几行代码，但换来了可移植性和可调试性。无论选哪种，都遵守本库的核心不变式：**`_type.h` 是用户唯一需要修改的文件。**

## test_project

`test_project/` 目录展示了两种数据结构的组合使用：用 `DynamicArray` 存储 `String` 元素，构建一个简易学生成绩管理系统。它展示了如何编写正确处理嵌套容器生命周期的 `DESTROY_ELEMENT` / `CLONE_ELEMENT` 宏。

## 许可

本项目按原样提供，用于教学和实用目的，未声明特定许可证。

---

# English

## Overview

This library provides 12 fundamental data structures, each self-contained and independent — you only need the files for the data structures you actually use. No dependencies, no build system, just `.c` and `.h` files.

### Design Philosophy

C has no templates, no RAII, no destructors. Writing a generic, memory-safe container in C means confronting the design questions that higher-level languages handle for you.

This library's core design decisions:

- **Generics via compile-time macros, not `void*`.** A dedicated `_type.h` file centralizes element type definitions and operations — inspectable, debuggable, and free of external tooling dependencies.
- **Deep copy on every insert; the container owns its data.** The caller retains ownership of what they passed in. No ambiguity about who frees what — a perennial source of bugs in `void*`-based C libraries.
- **Opaque structs hide implementation.** Headers expose only a forward declaration. Internal fields are invisible to the user, yet every operation is a direct function call — no vtable, no runtime overhead.
- **Uniform return-value conventions for error handling.** 1 for success, 0 for failure, -1 for NULL input — applied consistently across every function in every container. No exceptions, no errno.

The result is a library that is clean enough to read and learn from, and solid enough to drop into a real project.

### Data Structures

#### Array-Based (Contiguous Storage)

| Structure | Description |
|---|---|
| **DynamicArray** | Generic resizable array (analogous to `std::vector` in C++) |
| **Stack** | LIFO stack built on a dynamic array |
| **Deque** | Double-ended queue using a circular buffer, O(1) push/pop at both ends |
| **Queue** | FIFO queue using a circular buffer, O(1) enqueue/dequeue |
| **String** | Character-oriented dynamic array with string-specific operations (compare, substring, concat, C-string conversion) |

#### Node-Based (Linked Storage)

| Structure | Description |
|---|---|
| **SinglyLinkedList** | Singly linked list with index-based and cursor-based operations |
| **DoubleLinkedList** | Doubly linked list with forward/backward traversal and cursor insertion/deletion |

#### Tree-Based (Self-Balancing BST)

| Structure | Description |
|---|---|
| **AVLTree** | AVL tree with LL/LR/RL/RR rotations, maintaining height per node |
| **RedBlackTree** | Red-black tree with standard insert/delete fixup rules |

#### Specialized

| Structure | Description |
|---|---|
| **PriorityQueue (Min)** | Min-heap using a binary heap (array-based) |
| **PriorityQueue (Max)** | Max-heap using a binary heap (array-based) |
| **HashTable** | Hash table with separate chaining (singly linked buckets), FNV-1a hash |

### Directory Structure

```
C_DataStructure_src/
├── DynamicArray/
│   ├── ds_dynamicarray.h          # Public API
│   ├── ds_dynamicarray.c          # Implementation
│   ├── ds_dynamicarray_type.h     # Element type configuration
│   └── main.c                     # Usage demo
├── Stack/                          # (same layout)
├── Deque/                          # (same layout)
├── SinglyLinkedList/               # (same layout)
├── DoubleLinkedList/               # (same layout)
├── AVLTree/                        # (same layout)
├── RedBlackTree/                   # (same layout)
├── PriorityQueue/                  # two heaps: min + max
├── HashTable/                      # (same layout)
├── Queue/                          # (same layout)
├── String/                         # (same layout)
└── test_project/                   # Integration demo: DynamicArray + String
```

## Design

### Three-File Pattern

Every data structure follows a consistent three-file layout:

1. **`ds_<name>.h`** — Public header. Declares an opaque struct type and all API functions. Rich inline documentation with usage examples.

2. **`ds_<name>.c`** — Implementation file. Defines the internal struct (hidden from users) and all function bodies. Internal helpers are `static`.

3. **`ds_<name>_type.h`** — **Extension point for customization.** This is where you define:
   - The element struct (`ds_<name>_type`)
   - `DESTROY_ELEMENT` macro — frees heap resources inside an element
   - `CLONE_ELEMENT` macro — deep-copies an element
   - `MATCH` macro — equality predicate (for sequence containers)
   - Comparison macros `_LT`, `_GT`, `_EQ`, `_LE`, `_GE` (for trees and heaps)
   - `HASH` macro (for HashTable)

Each `_type.h` includes a commented-out example showing how to modify the macros when your element contains heap-allocated fields like `char *name`.

### Naming Convention

All public symbols use the `ds_` prefix:

```
ds_<container>_<operation>
```

Examples: `ds_dynamicarray_push_back`, `ds_avltree_insert`, `ds_hashtable_find`

### Opaque Types

Internal struct details are hidden from users. You interact with containers only through function calls:

```c
typedef struct DS_DynamicArray DS_DynamicArray;  // forward declaration in .h
struct DS_DynamicArray { ... };                  // definition in .c, invisible to user
```

## API Conventions

### Return Values

| Category | Returns |
|---|---|
| `create` / `clone` | Pointer on success, `NULL` on failure |
| `destroy` | `void` (safe to pass `NULL`) |
| Query (`size`, `capacity`, `is_empty`) | Non-negative on success, `-1` for `NULL` input |
| Mutation (`push`, `insert`, `set`, `erase`...) | `1` on success, `0` on failure |
| Element retrieval (`get`, `find`, `peek`, `pop`) | `1` on success, `0` on failure; element returned via output parameter |
| Tree cursors (`search`, `find_min`, `successor`...) | Cursor pointer on success, `NULL` if not found / exhausted |

### Output Parameter Pattern

Functions that return elements use a **pointer-to-pointer** output parameter:

```c
DS_DYNAMICARRAY_TYPE *data;
if (ds_dynamicarray_get(array, 0, &data)) {
    printf("value = %d\n", data->value);
    data->value = 999;  // can modify in place
}
```

## Two Container Families, Two Erase Return Conventions

This library's containers fall into two categories, and their `erase` / `pop` return values have different lifetimes:

**Array-based** — DynamicArray, Deque, Stack, Queue

Data lives in contiguous memory. `erase` returns a pointer **directly into that memory**
— it is not a separate heap block. The next `push` / `insert` will overwrite that slot,
invalidating the pointer.

```c
DS_DYNAMICARRAY_TYPE *data;
ds_dynamicarray_erase(da, 0, &data);

DS_DYNAMICARRAY_DESTROY_ELEMENT(*data);  // free heap members inside the element (if any)
// Do NOT free(data) — it points into the container
```

**Node-based** — SinglyLinkedList, DoubleLinkedList, HashTable

Data lives in separately allocated nodes. `erase` **shallow-copies the node's data
into a new `malloc`'d block**, frees the node, and returns the new block to you.
You are responsible for `free(data)` when done.

```c
DS_HASHTABLE_TYPE *data;
ds_hashtable_erase(ht, 100, &data);

DS_HASHTABLE_DESTROY_ELEMENT(*data);  // free heap members inside the element (if any)
free(data);                           // Must free — it was malloc'd for you
```

### `_and_destroy` Variants

Prefer not to manage this manually? Use the `_and_destroy` variants (e.g. `pop_back_and_destroy`,
`erase_and_destroy`) — the container handles **DESTROY_ELEMENT + free** (node-based) for you.

```c
// Array-based automatic cleanup
ds_dynamicarray_pop_back_and_destroy(array);

// Node-based automatic cleanup
ds_hashtable_erase_and_destroy(ht, 100);
```

### NULL Safety

All public functions safely handle `NULL` container pointers by returning an error code (`0`, `-1`, or `NULL`).

### Deep Copy Semantics

All insert, set, and clone operations deep-copy elements via `CLONE_ELEMENT`. The container owns its copies; the caller retains ownership of the original data.

## Quick Start

### 1. Configure Your Element Type

Edit the `_type.h` file for your chosen data structure. Define your element struct and the required macros:

```c
// ds_dynamicarray_type.h
typedef struct {
    int id;
    char *name;       // heap-allocated field
    double score;
} ds_dynamicarray_type;

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e)  \
    do { free((e).name); (e).name = NULL; } while (0)

#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, judge)        \
    ({                                                  \
        char *name_copy = (e).name ? strdup((e).name) : NULL; \
        if ((e).name && !name_copy) *(judge) = 0;      \
        (ds_dynamicarray_type){.id = (e).id,            \
                               .name = name_copy,       \
                               .score = (e).score};     \
    })

#define DS_DYNAMICARRAY_MATCH_TYPE int
#define DS_DYNAMICARRAY_MATCH(e, target) ((e).id == target ? 1 : 0)
```

### 2. Use the Data Structure

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = {1, strdup("Alice"), 95.5};
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);  // container has its own deep copy

    DS_DYNAMICARRAY_TYPE tmp2 = {2, strdup("Bob"), 87.0};
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);  // 2

    ds_dynamicarray_destroy(arr);  // frees all elements and the container
    return 0;
}
```

> **IMPORTANT: Clean Up Your Own Copy**
>
> The container deep-copies your data on insert / set / push. **You remain the owner**
> of the original you passed in. If your element type has heap fields (like `char *name`
> from `strdup`), you **must** call `DESTROY_ELEMENT` on your local copy after the
> operation. Otherwise the heap memory you allocated leaks — the container does not
> free it for you.
>
> If your element type has only scalar fields (int, double, etc.), a compound literal
> like `(type){10}` is harmless — nothing on the heap to clean up.

### 3. Compile

No build system required. Compile directly with any C compiler:

```bash
# Compile a single data structure with its demo
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# Compile the integration test project
gcc -o test_project test_project/ds_dynamicarray.c test_project/ds_string.c test_project/main.c
```

All data structures are independent — compile only the `.c` files you need.

## Advanced Features

### Cursor-Based Iteration (Trees & Linked Lists)

```c
// In-order traversal of an AVL tree
AVLTreeNode *cursor = ds_avltree_find_min(tree);
while (cursor != NULL) {
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key=%d\n", data->key);
    cursor = ds_avltree_successor(tree, cursor);
}
```

### Traversal Callbacks

```c
void print_value(DS_AVLTREE_TYPE *value, void *user_data) {
    printf("key=%d, value=%d\n", value->key, value->value);
}
ds_avltree_traverse_inorder_value(tree, NULL, print_value);
```

### Range Queries (Trees)

```c
ds_avltree_range_query(tree,
    (DS_AVLTREE_TYPE){.key = 30},
    (DS_AVLTREE_TYPE){.key = 70},
    NULL, visit_callback);
```

### Capacity Management (Array-Based)

```c
ds_dynamicarray_reserve(array, 1000);   // pre-allocate to avoid repeated reallocs
ds_dynamicarray_shrink_to_fit(array);   // free excess capacity
```

### Insert vs Put (HashTable)

```c
ds_hashtable_insert(ht, value);  // fails if key already exists
ds_hashtable_put(ht, value);     // overwrites if key exists, inserts otherwise
```

## Capacity Growth

All array-based containers use a **doubling growth strategy**: initial capacity 0 → 1 on first insert → doubles thereafter. HashTable triggers rehashing when `size >= capacity` (load factor = 1.0).

## Design Trade-offs

- **No `void*` erasure** — generics via macros in `_type.h`. Compile-time type safety at the cost of recompilation when the element type changes.
- **No error codes beyond return values** — no `errno`, `assert`, or `exit`. All errors reported through return values.
- **Single-threaded** — no locking or atomic operations.
- **No build system** — each data structure is a standalone set of `.c`/`.h` files.

## Writing Portable CLONE / DESTROY Macros

The default `CLONE_ELEMENT` in each `_type.h` uses a GNU statement expression `({...})` to embed clone logic directly in the macro body. This is concise but has two drawbacks: it does not compile under MSVC, and step-through debugging of macro-expanded code is difficult.

A more portable alternative is to write a **`static inline` function** in `_type.h`, then have the macro delegate to it:

```c
// ds_dynamicarray_type.h

typedef struct {
    int id;
    char *name;
} ds_dynamicarray_type;

// --- user-written clone / destroy functions ---

static inline void destroy_element(ds_dynamicarray_type *e)
{
    free(e->name);
    e->name = NULL;
}

static inline ds_dynamicarray_type
clone_element(const ds_dynamicarray_type *src, int *judge)
{
    ds_dynamicarray_type copy = {.id = src->id, .name = NULL};
    if (src->name) {
        copy.name = strdup(src->name);
        if (!copy.name) { *judge = 0; return copy; }
    }
    return copy;
}

// --- macros are now trivial wrappers ---

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e)  destroy_element(&(e))
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, j) clone_element(&(e), (j))
#define DS_DYNAMICARRAY_MATCH_TYPE          int
#define DS_DYNAMICARRAY_MATCH(e, t)         ((e).id == (t) ? 1 : 0)
```

Key points:

- **`static`** avoids duplicate-symbol errors when `_type.h` is included by multiple `.c` files.
- **`inline`** lets the compiler eliminate the call overhead — the generated code is identical to the macro version.
- The macro **call syntax does not change**, so all `.c` / `.h` implementation files remain untouched. Users only edit `_type.h`.
- This pattern works on **any C99 compiler** (GCC, Clang, MSVC).

The choice between the two styles is purely a trade-off: the `({...})` macro keeps everything in one place; the `static inline` function adds a few lines but gains portability and debuggability. Both respect the core invariant of this library: **`_type.h` is the only file the user ever needs to modify.**

## test_project

The `test_project/` directory demonstrates composing two data structures: `DynamicArray` storing `String` elements to build a student grade system. It shows how to write `DESTROY_ELEMENT` / `CLONE_ELEMENT` macros that respect nested container lifecycles.

## License

This project is provided as-is for educational and practical use. No specific license is declared.

