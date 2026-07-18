# C语言数据结构库

面向学习但功能完备的C语言数据结构库。编译期宏实现类型安全的泛型容器，深拷贝所有权模型，不透明指针 API。提供 Python 脚本代码生成器以支持单编译单元多类型并存，同时包含字节级哈夫曼压缩算法与磁盘页 B+ 树。

[:us: English Version](README_EN.md)

---

## 快速体验

```bash
git clone https://github.com/zuozhewangyiping/C_DataStructure.git
cd C_DataStructure

# Linux / macOS
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && ./demo

# Windows（CMD / PowerShell）
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && demo

# 输出: is_empty: 1, size: 0, capacity: 0
# ...（实际输出不止一行）
```

零依赖，无构建系统，只有 `.c` 和 `.h` 文件。**要求 C99 或更高版本。**

---

## 目录

- [数据结构一览](#数据结构一览)
  - [内存容器](#内存容器)
  - [算法](#算法)
  - [磁盘类](#磁盘类)
- [快速上手](#快速上手)
- [多类型支持（代码生成器）](#多类型支持代码生成器)
- [API 约定](#api-约定)
- [进阶功能](#进阶功能)
- [两类容器，两种 erase 返回值约定](#两类容器两种-erase-返回值约定)
- [扩容策略](#扩容策略)
- [设计取舍](#设计取舍)
- [编写可移植的宏](#编写可移植的宏)
- [test_project](#test_project)
- [与其他方案对比](#与其他方案对比)
- [许可](#许可)

---

## 数据结构一览

### 内存容器

#### 数组类（连续存储）

| 结构 | 插入 | 删除 | 查找 | 说明 |
|---|---|---|---|---|
| **DynamicArray** | O(1)* / O(n) | O(1)* / O(n) | O(1) / O(n) | 泛型动态数组（类比 C++ `std::vector`） |
| **Stack** | O(1)* | O(1) | O(1) 栈顶 | LIFO 栈，基于动态数组 |
| **Deque** | O(1)* | O(1) | O(1) | 双端队列，循环数组，两端 O(1) |
| **Queue** | O(1)* | O(1) | O(1) 队首 | FIFO 队列，循环数组 |
| **String** | O(1)* / O(n) | O(n) | O(1) | 字符动态数组，支持比较、子串、拼接 |

> O(1)* = 均摊 O(1)。查找列同时列出索引访问 O(1) 和搜索 O(n)。

#### 节点类（链式存储）

| 结构 | 插入 | 删除 | 查找 | 说明 |
|---|---|---|---|---|
| **SinglyLinkedList** | O(1) 头 / O(n) | O(1) 头 / O(n) | O(n) | 单向链表，支持索引和游标操作 |
| **DoubleLinkedList** | O(1) 头尾 / O(n) | O(1) 头尾 / O(n) | O(n) | 双向链表，支持正反向遍历 |
| **SkipList** | O(log n) 概率 | O(log n) 概率 | O(log n) 概率 | 概率平衡的多层索引结构，游标操作 |

#### 树类（自平衡二叉搜索树）

| 结构 | 插入 | 删除 | 搜索 | 最小/最大 | 遍历 | 说明 |
|---|---|---|---|---|---|---|
| **AVLTree** | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | LL/LR/RL/RR 四种旋转 |
| **RedBlackTree** | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | 标准插入/删除修正规则 |

#### 专用容器

| 结构 | 插入 | 删除 | 搜索 | 取顶 | 说明 |
|---|---|---|---|---|---|
| **PriorityQueue (Min)** | O(log n) | O(log n) | — | O(1) | 最小堆，二叉堆实现 |
| **PriorityQueue (Max)** | O(log n) | O(log n) | — | O(1) | 最大堆，二叉堆实现 |
| **HashTable** | O(1) 平均 | O(1) 平均 | O(1) 平均 | — | 链地址法，FNV-1a 哈希 |

### 算法

| 算法 | 说明 |
|---|---|
| **HuffmanCoding** | 字节级哈夫曼压缩，8 比特打包输出，内嵌最小堆建树，支持任意数据的编码/解码往返 |

### 磁盘类（持久化存储）

| 结构 | 插入 | 删除 | 搜索 | 说明 |
|---|---|---|---|---|
| **BPlusTree** | O(log n) | O(log n) | O(log n) | 4KB 磁盘页 B+ 树，数据持久化到文件，key/value 通过 fread/fwrite 整页读写

---

## 快速上手

### 1. 配置元素类型

编辑所选数据结构的 `_type.h` 文件。这是**你唯一需要修改的文件**：

```c
// ds_dynamicarray_type.h
typedef struct {
    int id;
    char *name;       // 堆分配字段
    double score;
} ds_dynamicarray_type;

static inline void destroy_element(ds_dynamicarray_type *e)
{
    free(e->name);
    e->name = NULL;
}
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) destroy_element(&(e))

// 编写适配函数，再由宏调用
static inline ds_dynamicarray_type
clone_element(const ds_dynamicarray_type *src, int *judge)
{
    ds_dynamicarray_type copy = {.id = src->id, .score = src->score, .name = NULL};
    if (src->name) {
        copy.name = strdup(src->name);
        if (!copy.name) { *judge = 0; return copy; }
    }
    return copy;
}
#define DS_DYNAMICARRAY_CLONE_ELEMENT(e, j) clone_element(&(e), j)

#define DS_DYNAMICARRAY_MATCH_TYPE int
static inline int match_element(const ds_dynamicarray_type *e, DS_DYNAMICARRAY_MATCH_TYPE target)
{
    return e->id == target ? 1 : 0;
}
#define DS_DYNAMICARRAY_MATCH(e, t) match_element(&(e), t)
```

### 2. 使用数据结构

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = {1, strdup("Alice"), 95.5};
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);  // 容器已有深拷贝，释放自己的副本

    DS_DYNAMICARRAY_TYPE tmp2 = {2, strdup("Bob"), 87.0};
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);  // 2

    ds_dynamicarray_destroy(arr);  // 释放所有元素和容器
    return 0;
}
```

> **注意：调用者负责清理传入的原始数据**
>
> 容器在 insert / set / push 时深拷贝传入的数据，**原始数据的所有权仍属于调用者**。
> 如果元素类型含堆字段（如 `strdup` 产生的 `char *name`），调用者须在操作完成后
> 调用 `DESTROY_ELEMENT` 清理自身持有的副本。容器不会自动释放调用者传入的堆资源。
>
> 如果元素类型仅含标量字段（int、double 等），直接传入字面量即可，无需清理。

### 3. 编译

无需构建系统，直接用任意 C 编译器编译：

```bash
# 编译单个数据结构及其示例
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# 编译组合测试项目
gcc -o test_project test_project/ds_dynamicarray.c test_project/ds_string.c test_project/main.c
```

所有数据结构互相独立——只编译你需要的 `.c` 文件即可。

> **进阶：同一容器、多种类型？** 如果你需要在同一个程序里使用同一容器的多种元素类型（比如 `DynamicArray<Student>` 和 `DynamicArray<Course>`），请参考[多类型支持（代码生成器）](#多类型支持代码生成器)章节。

---

## 多类型支持（代码生成器）

### 设计动机

`_type.h` 中的宏是全局的——一个编译单元内只能定义一种 `ds_dynamicarray_type`。如果你需要一个存 `Student` 的动态数组和一个存 `Course` 的动态数组，直接 `#include` 同一容器的头文件两次会因类型名和宏名冲突而无法编译。

这个问题是 C 语言宏泛型的固有限制——并非本库的设计缺陷。**代码生成器**就是为突破这个限制而设计的。

### 使用方法

每个容器目录下都有一个 `generate.py`（PriorityQueue 有 `generate_max.py` 和 `generate_min.py` 两个）。它读取母版三件套（`_type.h`、`.h`、`.c`），通过标识符批量重命名，生成三份带后缀的独立副本。母版文件不受任何影响。

**使用流程：**

```bash
# 1. 编辑母版 ds_dynamicarray_type.h，定义 Student 类型
# 2. 运行脚本，传入后缀名
cd DynamicArray
python generate.py student    # 生成 ds_dynamicarray_student_type.h / .h / .c

# 3. 修改 _type.h，定义 Course 类型，再生成一份
python generate.py course     # 生成 ds_dynamicarray_course_type.h / .h / .c
```

**在代码中同时使用：**

```c
#include "ds_dynamicarray_student.h"
#include "ds_dynamicarray_course.h"

int main() {
    // 学生数组
    DS_DynamicArray_Student *roster = ds_dynamicarray_student_create();
    DS_DYNAMICARRAY_STUDENT_TYPE s = {1, strdup("Alice"), 95.5};
    ds_dynamicarray_student_push_back(roster, s);
    DS_DYNAMICARRAY_STUDENT_DESTROY_ELEMENT(s);

    // 课程数组 —— 完全独立的类型
    DS_DynamicArray_Course *courses = ds_dynamicarray_course_create();
    DS_DYNAMICARRAY_COURSE_TYPE c = {1001, strdup("Math"), 4};
    ds_dynamicarray_course_push_back(courses, c);
    DS_DYNAMICARRAY_COURSE_DESTROY_ELEMENT(c);

    ds_dynamicarray_student_destroy(roster);
    ds_dynamicarray_course_destroy(courses);
    return 0;
}
```

编译时同时链接两份生成文件：

```bash
gcc -o app main.c ds_dynamicarray_student.c ds_dynamicarray_course.c
```

### 替换原理

脚本使用**两趟占位符替换**算法，对所有与类型相关的标识符（类型名、宏名、函数名、结构体名、`static inline` 辅助函数名、include 路径）系统化地加上后缀：

1. **第一趟**：所有 `old` → 占位符 `@@N@@`（按 old 长度降序，长串优先，避免短串先吃掉长串的部分）
2. **第二趟**：占位符 → `new`（占位符之间互不干扰）

为什么需要占位符？举个例子：`ds_dynamicarray_` → `ds_dynamicarray_student_` 之后，这个新串仍然以 `ds_dynamicarray_` 开头。如果直接做多规则替换，它会被后续规则二次误匹配。占位符不含任何原始标识符片段，彻底隔离了替换规则之间的相互干扰。

后缀可以是任意合法 C 标识符（`student`、`v2`、`configA` 等），算法对所有后缀名安全。

### 各容器生成器差异

不同容器拥有的宏和函数不同，生成器替换的标识符也相应不同：

| 容器 | 脚本 | 主要替换项 |
|---|---|---|
| **DynamicArray** | `generate.py` | DESTROY + CLONE + MATCH + 3 static inline 函数 |
| **Stack / Queue** | `generate.py` | DESTROY + CLONE + 2 static inline 函数（最简） |
| **Deque** | `generate.py` | DESTROY + CLONE + MATCH + 3 static inline 函数 |
| **SinglyLinkedList / DoubleLinkedList** | `generate.py` | DESTROY + CLONE + MATCH + 游标类型 + 3 static inline 函数 |
| **AVLTree / RedBlackTree** | `generate.py` | DESTROY + CLONE + 5 比较宏 + 游标类型 + 2 static inline 函数 |
| **SkipList** | `generate.py` | DESTROY + CLONE + 5 比较宏 + 游标类型 |
| **PriorityQueue (Min/Max)** | `generate_min.py` / `generate_max.py` | DESTROY + CLONE + 5 比较宏（函数已带 `_min`/`_max` 后缀） |
| **HashTable** | `generate.py` | DESTROY + CLONE + MATCH + MATCH_KEY + HASH + HASH_KEY + 6 static inline 函数（最复杂） |
| **BPlusTree** | `generate.py` | key/value 独立类型 + 5 比较宏 + 游标类型（无 DESTROY/CLONE） |

### 单类型场景

如果你不需要在同一个编译单元里对同一种容器使用多种元素类型，**完全忽略 `generate.py` 即可**。原有的"编辑 `_type.h` → 编译 `.c`"流程和之前一模一样，不受任何影响。

---

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

### NULL 安全

所有公共函数在访问任何字段前都会检查 `NULL` 指针，传入 `NULL` 容器会返回错误码（`0`、`-1` 或 `NULL`），不会发生段错误。

### 深拷贝语义

所有 insert、set、clone 操作都通过 `CLONE_ELEMENT` 宏进行深拷贝。容器拥有其内部副本的所有权；调用者保留原始数据的所有权。

### 命名规范

```
ds_<容器>_<操作>
```

例如：`ds_dynamicarray_push_back`、`ds_avltree_insert`、`ds_hashtable_find`。

---

## 进阶功能

### 游标迭代（树 & 链表）

树和链表提供不透明游标类型，避免索引带来的额外开销：

```c
AVLTreeNode *cursor = ds_avltree_find_min(tree);
while (cursor != NULL) {
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key=%d\n", data->key);
    cursor = ds_avltree_successor(tree, cursor);
}
```

### 遍历回调

支持前序、中序、后序、层序四种遍历：

```c
void print_value(DS_AVLTREE_TYPE *value, void *user_data) {
    (void)user_data;
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

---

## 两类容器，两种 erase 返回值约定

本库的容器分为两类，它们的 `erase` / `pop` 返回值的生命周期不同：

### 数组类 — DynamicArray、Deque、Stack、Queue

数据存储在连续内存中。`erase` 返回的指针**直接指向这块内存内部**，不是独立堆块。下次 `push` / `insert` 时该位置会被覆盖，指针随即失效。

```c
DS_DYNAMICARRAY_TYPE *data;
ds_dynamicarray_erase(da, 0, &data);

DS_DYNAMICARRAY_DESTROY_ELEMENT(*data);  // 清理元素内部的堆资源（如有）
// ⚠ 不能 free(data) —— 它指向数组内部
```

### 节点类 — SinglyLinkedList、DoubleLinkedList、HashTable

数据存储在独立 `malloc` 的节点中。`erase` 会将节点内的数据**浅拷贝到一个新 `malloc` 的堆块**，释放原节点，然后把新堆块的指针返回给你。**你用完必须手动 `free(data)`。**

```c
DS_HASHTABLE_TYPE *data;
ds_hashtable_erase(ht, 100, &data);

DS_HASHTABLE_DESTROY_ELEMENT(*data);  // 清理元素内部的堆资源（如有）
free(data);                           // ⚠ 必须 free —— 它是 malloc 分配的新堆块
```

### `_and_destroy` 变体

如需自动清理，可使用 `_and_destroy` 变体，容器负责完成 **DESTROY_ELEMENT + free**（节点类）的全部工作：

```c
// 数组类自动清理
ds_dynamicarray_pop_back_and_destroy(array);

// 节点类自动清理
ds_hashtable_erase_and_destroy(ht, 100);
```

### 磁盘类 — BPlusTree

B+ 树的 key/value 通过 `fread`/`fwrite` 整页搬运于磁盘与内存之间，不涉及 CLONE/DESTROY 宏。`delete` 直接在磁盘页上移除数据，无需用户手动管理内存。游标（`BPlusTreeNode *`）是 `malloc` 的副本，使用完毕需 `free(cursor)`。

---

## 扩容策略

所有基于数组的容器采用**翻倍扩容**策略：初始容量 0 → 首次插入变为 1 → 之后每次翻倍。哈希表在 `size >= capacity`（负载因子 = 1.0）时触发 rehash，桶数翻倍。

---

## 设计取舍

- **不用 `void*` 擦除类型。** 泛型通过 `_type.h` 中的宏实现。编译期类型安全，但宏的天然限制是同一编译单元只能注册一种类型——`generate.py` 代码生成器通过标识符重命名解决了这个问题，在保持类型安全的前提下实现了多类型并存。
- **只靠返回值报告错误。** 不使用 `errno`、`assert` 或 `exit`。所有错误通过返回值体现——无隐藏控制流。
- **单线程。** 无锁、无原子操作，假设单线程环境。
- **无构建系统。** 每个数据结构就是一组独立的 `.c`/`.h` 文件——不需要 Makefile 或 CMake。
- **不透明结构体。** 内部字段对用户隐藏，所有操作均为直接函数调用——无虚表开销。

---

## 编写可移植的宏

`_type.h` 中默认使用 **`static inline` 函数**来实现克隆和销毁逻辑，再由宏调用。这种写法兼容所有 C99 编译器（GCC、Clang、MSVC），且支持断点调试。

默认元素类型仅含标量字段（int 等），无需修改。当你需要增加堆分配字段时，只需在 `_type.h` 中修改结构体定义、编写自己的 clone / destroy 函数，并更新宏调用。

要点：

- **`static`** 避免 `_type.h` 被多个 `.c` 文件包含时出现符号重复定义。
- **`inline`** 允许编译器消除调用开销。
- 宏的**调用语法不变**，所有 `.c` / `.h` 实现文件无需任何修改。
- 核心不变式：**`_type.h` 是用户唯一需要修改的文件。**（若使用代码生成器，同样只需编辑母版 `_type.h`，再运行脚本——母版仍然是唯一需要编辑的 C 文件。）
- **磁盘类容器除外**：B+ 树的 key/value 为定长 POD 类型，通过 `fread`/`fwrite` 整页读写磁盘，不使用 CLONE/DESTROY 宏。

---

## test_project

`test_project/` 目录展示了两种数据结构的组合使用：用 `DynamicArray` 存储 `String` 元素，构建一个简易学生成绩管理系统：

```c
// test_project/main.c — 摘录
DS_DynamicArray *roster = ds_dynamicarray_create();

// 添加学生：String 保存姓名，int 保存成绩
DS_String *name1 = ds_cstr_to_string("Alice");
ds_dynamicarray_push_back(roster, (DS_DYNAMICARRAY_TYPE){.name = name1, .grade = 95});

DS_String *name2 = ds_cstr_to_string("Bob");
ds_dynamicarray_push_back(roster, (DS_DYNAMICARRAY_TYPE){.name = name2, .grade = 87});

// 查找学生
DS_DYNAMICARRAY_TYPE *entry;
if (ds_dynamicarray_find(roster, "Alice", &entry)) {
    printf("%s: %d\n", ds_string_to_cstr(entry->name), entry->grade);
}

ds_dynamicarray_destroy(roster);  // 递归销毁嵌套的 String
```

它展示了当 `DynamicArray` 的元素包含 `DS_String *` 时，`DESTROY_ELEMENT` / `CLONE_ELEMENT` 宏应该如何正确处理嵌套容器的生命周期——调用 `ds_string_destroy` 和 `ds_string_clone`。

---

## 与其他方案对比

| 方案 | 类型安全 | 内存安全 | 上手难度 | 适用场景 |
|---|---|---|---|---|
| **本库** | 编译期（宏） | 深拷贝所有权模型 | 中等（需理解宏系统；多类型场景需运行一次 Python 脚本） | 需要可读、可复用的泛型容器 |
| 手写裸结构体 | 编译期 | 手动管理 | 入门低，做对难 | 一次性、简单场景 |
| `void*` + 函数指针 | 无（运行时强转） | 易出错（"谁来释放？"） | 低 | 快速内部原型 |
| C++ STL | 编译期（模板） | RAII | 低（如果会 C++） | 能用 C++ 编译器的场景 |
| `klib` / `uthash` | 宏（纯头文件） | 各有差异 | 中等 | 极简、只要头文件的场景 |

**简言之：** 如果你在写 C、想要类型安全的泛型容器、在意清晰的所有权语义、并且愿意为每个数据结构配置一个 `_type.h` 文件，那么这个库就是为你准备的。

---

## 许可

本项目基于 MIT 许可证发布，详见 [LICENSE](LICENSE) 文件。
