# C_DataStructure

A teaching-grade data structure library in pure C, modeled after the C++ STL.

C has no templates, no RAII, and no iterators — the three pillars of the STL. This library provides a pure-C alternative for each:

- **Macro generics + code generator** replaces templates, delivering compile-time monomorphization.
- **`DESTROY_ELEMENT` / `CLONE_ELEMENT` macros** replace destructors and copy constructors; the container manages element lifetimes automatically.
- **Opaque cursors + index access** replaces iterators.

On this foundation, 14 containers share a uniform naming convention — `push_back` / `pop_back` / `insert` / `erase` / `find` and more. A disk-page B+ tree and a Huffman compression algorithm are also included.

---

## Table of Contents

- [Data Structures at a Glance](#data-structures-at-a-glance)
- [Quick Start](#quick-start)
- [Multi-Type Support (Code Generator)](#multi-type-support-code-generator)
- [Core Design Philosophy](#core-design-philosophy)
- [API Directory](#api-directory)
- [Advanced Features](#advanced-features)
- [Project Structure Guide](#project-structure-guide)
- [Implementation & Design Considerations](#implementation--design-considerations)
- [Comparison with Alternatives](#comparison-with-alternatives)
- [License](#license)

---

## Data Structures at a Glance

14 data structures and 1 algorithm, organized into four tiers:

### 1. Sequential

Elements are ordered by insertion. Traversal is supported.

#### Contiguous Storage

| Container | Role | Insert | Delete | Access | Notes |
|------|------|:--:|:--:|:--:|------|
| **DynamicArray** | Generic dynamic array | O(1)\* / O(n) | O(1)\* / O(n) | O(1) index | Analogous to C++ `std::vector` |
| **String** | Dynamic string | O(1)\* / O(n) | O(n) | O(1) index | Compare, substring, concat, C-string interop |
| **Deque** | Double-ended queue | O(1)\* | O(1) | O(1) index | Circular buffer, random-position insert/erase |

> O(1)\* = amortized O(1).

#### Linked Storage

| Container | Role | Insert | Delete | Access | Notes |
|------|------|:--:|:--:|:--:|------|
| **SinglyLinkedList** | Singly linked list | O(1) head / O(n) | O(1) head / O(n) | O(n) index | Analogous to C++ `std::forward_list` |
| **DoubleLinkedList** | Doubly linked list | O(1) head+tail / O(n) | O(1) head+tail / O(n) | O(n) index | Analogous to C++ `std::list`, bidirectional cursor |

#### Restricted Sequences

| Container | Role | Core Operations | Notes |
|------|------|------|------|
| **Stack** | LIFO stack | O(1) push / pop / peek | Built on dynamic array |
| **Queue** | FIFO queue | O(1) enqueue / dequeue | Circular buffer |

### 2. Associative

Elements are organized and looked up by key, not insertion order.

#### Hash-Based

| Container | Role | Avg. Lookup | Notes |
|------|------|:--:|------|
| **HashTable** | Hash table | O(1) | Separate chaining, FNV-1a hash. `insert` rejects duplicates; `put` overwrites |

#### Ordered Trees

| Container | Role | Lookup | Insert | Delete | Notes |
|------|------|:--:|:--:|:--:|------|
| **AVLTree** | Strictly balanced BST | O(log n) | O(log n) | O(log n) | LL/LR/RL/RR rotations, height difference ≤1 |
| **RedBlackTree** | Red-black tree | O(log n) | O(log n) | O(log n) | ≤2 rotations on insert; deletion is more involved |

#### Probabilistic

| Container | Role | Expected Lookup | Notes |
|------|------|:--:|------|
| **SkipList** | Skip list | O(log n) | 32-level probabilistic balance, concise implementation |

### 3. Heap

Only the extreme value matters; global ordering is irrelevant.

| Container | Role | push | pop | peek | Notes |
|------|------|:--:|:--:|:--:|------|
| **PriorityQueue (Max)** | Max-heap | O(log n) | O(log n) | O(1) | Binary heap |
| **PriorityQueue (Min)** | Min-heap | O(log n) | O(log n) | O(1) | Independent implementation; comparison direction inverted |

### 4. Persistent

Data is persisted to disk (pages are read from and written to disk via `fread` / `fwrite` at runtime).

| Container | Role | Lookup | Insert | Delete | Notes |
|------|------|:--:|:--:|:--:|------|
| **BPlusTree** | Disk B+ tree | O(log n) | O(log n) | O(log n) | 4 KB pages, file persistence, page recycling |

### 5. Algorithm

| Algorithm | Notes |
|------|------|
| **HuffmanCoding** | Huffman compression — min-heap tree construction, 8-bit packing, full encode/decode round-trip |

## Quick Start

### 1. Zero-Friction Demo

```bash
git clone https://github.com/zuozhewangyiping/C_DataStructure.git
cd C_DataStructure
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && ./demo
```

Zero dependencies. No build system. Just `.c` and `.h` files. **Requires C99 or later.**

### 2. Write Your First Program

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

The default element type is `{ int data }`. Up to this point, you don't need to understand any of the generic machinery.

### 3. Configure Your Element Type

If plain `int` isn't enough, edit the container's `ds_xxx_type.h`. This is the only file you need to modify:

```c
// ds_dynamicarray_type.h
typedef struct {
    int   id;
    char *name;        // heap-allocated field
    double score;
} ds_dynamicarray_type;

// Destroy: free name
static inline void destroy_element(ds_dynamicarray_type *e) {
    free(e->name);
    e->name = NULL;
}
#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e) destroy_element(&(e))

// Clone: deep-copy name
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

// Match: find by id
#define DS_DYNAMICARRAY_MATCH_TYPE int
static inline int match_element(const ds_dynamicarray_type *e,
                                DS_DYNAMICARRAY_MATCH_TYPE target) {
    return e->id == target ? 1 : 0;
}
#define DS_DYNAMICARRAY_MATCH(e, t) match_element(&(e), t)
```

### 4. Use the Container

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = { 1, strdup("Alice"), 95.5 };
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);   // container deep-copied it; free your original

    DS_DYNAMICARRAY_TYPE tmp2 = { 2, strdup("Bob"), 87.0 };
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);    // 2

    ds_dynamicarray_destroy(arr);            // frees all elements and the container
    return 0;
}
```

> **IMPORTANT: Caller retains ownership of the original data.**
>
> The container deep-copies data on `push` / `insert` / `set` via `CLONE_ELEMENT`. Ownership of the original remains with the caller — if the element type contains heap fields (e.g. `char *` from `strdup`), the caller must call `DESTROY_ELEMENT` on the local copy after the operation. The container will not free heap resources owned by the caller.
>
> If the element has only scalar fields, a compound literal may be passed directly — no cleanup is required.

### 5. Compile

No build system needed. Just list the `.c` files you use:

```bash
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# Multiple containers
gcc -o myapp myapp.c \
    DynamicArray/ds_dynamicarray.c \
    HashTable/ds_hashtable.c \
    String/ds_string.c
```

All containers are independent — compile only the `.c` files you need.

---

## Multi-Type Support (Code Generator)

### Why a Code Generator

The macros in `ds_xxx_type.h` are global — you can only define one `ds_dynamicarray_type` per compilation unit. If you need both `DynamicArray<Student>` and `DynamicArray<Course>` in the same program, including the header multiple times will fail because type names and macro names collide.

This is an inherent limitation of macro-based generics in C. The code generator overcomes it through systematic identifier renaming.

### Workflow

```bash
# 1. Define the Student type in the mother ds_dynamicarray_type.h (as in the previous section)
# 2. Run the script
cd DynamicArray
python generate.py student     # produces ds_dynamicarray_student_type.h / .h / .c

# 3. Edit _type.h for Course, then generate another copy
python generate.py course      # produces ds_dynamicarray_course_type.h / .h / .c
```

### Using Both Types

```c
#include "ds_dynamicarray_student.h"
#include "ds_dynamicarray_course.h"

int main() {
    // Student array
    DS_DynamicArray_Student *roster = ds_dynamicarray_student_create();
    DS_DYNAMICARRAY_STUDENT_TYPE s = { 1, strdup("Alice"), 95.5 };
    ds_dynamicarray_student_push_back(roster, s);
    DS_DYNAMICARRAY_STUDENT_DESTROY_ELEMENT(s);

    // Course array — a completely independent type
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

### Single-Type Usage

If you don't need multiple element types for the same container in one compilation unit, ignore `generate.py` entirely. The original workflow — edit `_type.h`, compile the `.c` file — works exactly as before.

---

## Core Design Philosophy

### Macro-Based Generics

```
┌──────────────────────────────┐
│  ds_xxx_type.h   (you edit)  │
│  Define element struct + 3   │
│  macros:                     │
│  DESTROY  ← destructor       │
│  CLONE    ← copy             │
│  MATCH    ← lookup           │
└─────────────┬────────────────┘
              │ #include
┌─────────────▼────────────────┐
│  ds_xxx.h       (public API) │
│  #define DS_XXX_TYPE alias   │
│  All function signatures     │
│  reference macro names       │
└─────────────┬────────────────┘
              │
┌─────────────▼────────────────┐
│  ds_xxx.c       (untouched)  │
│  Refer to element type only  │
│  through macros              │
│  Algorithms are fully        │
│  decoupled from types        │
└──────────────────────────────┘
```

This is the same principle as C++ template monomorphization — generating independent, type-safe code for each type. The difference is that C++ compilers do it automatically, whereas this library triggers it manually through `generate.py`.

### Deep-Copy Ownership Model

Every `push` / `insert` / `set` / `clone` operation performs a deep copy through `CLONE_ELEMENT`. The container owns its internal copies completely and calls `DESTROY_ELEMENT` on each element when `destroy`ed. The caller retains ownership of the original data — there is no ambiguity about "who frees what."

### Two Container Families, Two Erase Conventions

#### Array-Based (DynamicArray / Deque / Stack / Queue)

Data is stored in contiguous memory. Pointers returned by `erase` / `pop` point inside this memory or just past it — they are not independent heap blocks. **Never `free()` them.** The next `push` / `insert` / `reserve` will overwrite the slot and invalidate the pointer.

```c
DS_DYNAMICARRAY_TYPE *p;
ds_dynamicarray_erase(da, 0, &p);
DS_DYNAMICARRAY_DESTROY_ELEMENT(*p);   // clean up heap members inside the element (if any)
// Do NOT free(p) — p points inside the array
```

#### Node-Based (SinglyLinkedList / DoubleLinkedList / HashTable)

Data is stored in individually `malloc`'d nodes. `erase` / `pop` shallow-copies the node's data into a newly `malloc`'d block, frees the node, and returns a pointer to the new block. **You must call `DESTROY_ELEMENT` then `free`.**

```c
DS_HASHTABLE_TYPE *p;
ds_hashtable_erase(ht, 100, &p);
DS_HASHTABLE_DESTROY_ELEMENT(*p);   // clean up heap members first
free(p);                            // then free the element itself
```

#### `_and_destroy` Variants

Every `erase` / `pop` / `dequeue` has an `_and_destroy` variant that lets the container handle cleanup.

#### Tree Cursors

Tree cursors (`AVLTreeNode *`, etc.) point directly into tree nodes — never `free` them. B+ tree cursors are `malloc`'d copies — you must `free` them.

### Error Handling

| Returns | Meaning |
|:--:|------|
| `1` | Success |
| `0` | Failure: NULL argument, out of bounds, empty container, malloc failure, not found, duplicate key |
| `-1` | Query function (`size` / `capacity` / `is_empty`) received a NULL container |
| `NULL` + `judge=1` | `clone` succeeded (source was NULL) |
| `NULL` + `judge=0` | `clone` failed (malloc failure) |

### Naming Convention

```
ds_<module>_<operation>[_<variant>]

e.g. ds_dynamicarray_push_back
     ds_avltree_insert
     ds_hashtable_erase_and_destroy
```

Operation names are as uniform as possible across containers. For specific differences, see the next chapter.

---

## API Directory

The following tables list unified operation suffixes by category. You can derive the full function name for any container by applying the prefix rule.

### Legend

Column headers follow the classification from "Data Structures at a Glance." Each abbreviation maps to these containers:

| Abbreviation | Containers |
|------|------|
| Seq-Contiguous | DynamicArray, String, Deque |
| Seq-Linked | SinglyLinkedList, DoubleLinkedList |
| Seq-Restricted | Stack, Queue |
| Assoc-Hash | HashTable |
| Assoc-Tree | AVLTree, RedBlackTree, SkipList |
| Heap | PriorityQueue |
| Persistent | BPlusTree |

● = supported by all containers in the category. A specific container name (e.g. `Deque`, `AVL,RB`) means the operation is not uniform within the category and cannot be marked ●.

The tables list suffixes only, not full signatures. Apply the prefix `ds_<module>_` to obtain the complete function name.

### Create & Destroy

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent | Algorithm |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `create` / `destroy` | ● | ● | ● | ● | ● | ● | ● | ● |
| `build` | | | | | | | | ● |

### State Query

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `size` | ● | ● | ● | ● | SkipList | ● | ● |
| `capacity` | ● | | ● | ● | | ● | |
| `is_empty` | ● | ● | ● | ● | ● | ● | ● |
| `height` | | | | | AVL,RB | | |
| `count` | | | | | AVL,RB | | |
| `level` | | | | | SkipList | | ● |

### Insert

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `push_back` | ● | ● | Deque | | | | |
| `push_front` | Deque | ● | Deque | | | | |
| `push` | | | Stack | | | ● | |
| `enqueue` | | | Queue | | | | |
| `insert` | ● | ● | Deque | ● | ● | | ● |

### Delete

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `pop_back` / `_and_destroy` | ● | ● | Deque | | | | |
| `pop_front` / `_and_destroy` | Deque | ● | Deque | | | | |
| `pop` / `_and_destroy` | | | Stack | | | ● | |
| `dequeue` / `_and_destroy` | | | Queue | | | | |
| `erase` / `_and_destroy` | ● | ● | Deque | ● | | | |
| `delete` | | | | | ● | | ● |

### Access

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `get` | ● | ● | Deque | | | | |
| `set` | ● | ● | Deque | | | | |
| `peek` | | | Stack | | | ● | |
| `peek_front` / `peek_back` | | | Queue,Deque | | | | |
| `node_get_data` | | ● | | | ● | | ● |

### Lookup

| Suffix | Seq-Contiguous | Seq-Linked | Assoc-Hash | Assoc-Tree | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|
| `find` | ● | ● | ● | | |
| `search` | | ● | | ● | ● |
| `find_min` / `find_max` | | | | ● | ● |
| `successor` / `predecessor` | | | | ● | ● |

### Traversal

| Suffix | Seq-Contiguous | Seq-Linked | Assoc-Hash | Assoc-Tree | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|
| `traverse` | ● | ● | ● | SkipList | ● |
| `begin` / `next` | | ● | | ● | ● |
| `prev` / `rbegin` | | DoubleLL | | AVL,RB | |
| `_preorder` / `_inorder` / `_postorder` / `_levelorder` | | | | AVL,RB | |
| `range_query` | | | | ● | ● |

### Capacity Management

| Suffix | Seq-Contiguous | Seq-Restricted | Assoc-Hash | Heap |
|------|:--:|:--:|:--:|:--:|
| `reserve` | ● | ● | ● | ● |
| `shrink_to_fit` | ● | ● | | ● |

### Clone & Concat

| Suffix | Seq-Contiguous | Seq-Linked | Seq-Restricted | Assoc-Hash | Assoc-Tree | Heap | Persistent |
|------|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
| `clone` | ● | ● | ● | ● | ● | ● | ● |
| `concat` | ● | ● | | | | | |

### Cursor Variants (Seq-Linked Only)

| Suffix | Covers | Applies To |
|------|---------|:--:|
| `_after_cursor` / `_and_destroy` | insert / erase | ● |
| `_before_cursor` / `_and_destroy` | insert / erase | DoubleLinkedList |
| `_cursor` / `_and_destroy` | erase | DoubleLinkedList |

### Specialized Operations

| Suffix | Applies To | Notes |
|------|------|------|
| `put` | HashTable | Insert or overwrite (overwrites when key exists, inserts otherwise) |
| `compare` | String | Lexicographic comparison |
| `substring` | String | Extract substring |
| `cstr_to_string` / `string_to_cstr` | String | C string interop |
| `encode` / `decode` | HuffmanCoding | Compression and decompression |

---

## Advanced Features

### Cursor Iteration (Trees & Linked Lists)

Trees and linked lists provide opaque cursor types, avoiding the traversal overhead of index-based access:

```c
AVLTreeNode *cursor = ds_avltree_find_min(tree);
while (cursor != NULL) {
    DS_AVLTREE_TYPE *data;
    ds_avltree_node_get_data(cursor, &data);
    printf("key = %d\n", data->key);
    cursor = ds_avltree_successor(cursor);
}
```

### Traversal Callbacks

Tree containers support preorder, inorder, postorder, and level-order traversal. Linked lists and hash tables support forward traversal:

```c
void print_value(DS_AVLTREE_TYPE *value, void *user_data) {
    (void)user_data;
    printf("key = %d, value = %d\n", value->key, value->value);
}
ds_avltree_traverse_inorder_value(tree, NULL, print_value);
```

### Range Queries (Trees)

```c
ds_avltree_range_query(tree,
    (DS_AVLTREE_TYPE){ .key = 30 },
    (DS_AVLTREE_TYPE){ .key = 70 },
    NULL, visit_callback);
```

### insert vs. put (HashTable)

```c
ds_hashtable_insert(ht, value);   // rejects if key exists, returns 0
ds_hashtable_put(ht, value);      // overwrites if key exists, inserts otherwise
```

### Nested Containers (test_project)

The `test_project/` directory demonstrates composing two data structures: a `DynamicArray` whose elements contain a `DS_String *` field, building a student grade management example.

```c
DS_DynamicArray *roster = ds_dynamicarray_create();

DS_String *name1 = ds_cstr_to_string("Alice");
ds_dynamicarray_push_back(roster,
    (DS_DYNAMICARRAY_TYPE){ .name = name1, .id = 2025001, .score = 95 });

DS_String *name2 = ds_cstr_to_string("Bob");
ds_dynamicarray_push_back(roster,
    (DS_DYNAMICARRAY_TYPE){ .name = name2, .id = 2025002, .score = 87 });

ds_dynamicarray_destroy(roster);   // recursively destroys nested DS_Strings
```

It demonstrates how `DESTROY_ELEMENT` / `CLONE_ELEMENT` macros should call `ds_string_destroy` / `ds_string_clone` to correctly handle nested container lifecycles.

---

## Project Structure Guide

```
C-DataStructure/
│
├── DynamicArray/                ← Start here
│   ├── ds_dynamicarray_type.h   ← User-editable element type
│   ├── ds_dynamicarray.h        ← Public API
│   ├── ds_dynamicarray.c        ← Implementation
│   ├── main.c                   ← Demo
│   └── generate.py              ← Multi-type code generator
│
├── SinglyLinkedList/            ← Then linked lists & cursors
├── DoubleLinkedList/
├── Stack/
├── Queue/
├── Deque/
├── PriorityQueue/               ← Two independent implementations: generate_max.py / generate_min.py
│
├── HashTable/
├── AVLTree/                     ← Self-balancing trees
├── RedBlackTree/
├── SkipList/
├── BPlusTree/                   ← Disk-backed, the largest module
├── String/
│
├── Algorithms/
│   └── HuffmanCoding/
│
└── test_project/                ← Integration demo
```

### Recommended Learning Order

| Stage | Module | Goal |
|------|------|------|
| 1 | **DynamicArray** | Dynamic arrays, doubling growth, macro generics fundamentals |
| 2 | **String** | How the same dynamic array mechanism simplifies when the type is fixed (`char`), yielding a clean API with string-specific operations |
| 3 | **SinglyLinkedList** → **DoubleLinkedList** | Pointer structures, opaque cursors, callback traversal |
| 4 | **Stack** → **Queue** → **Deque** | Restricted interfaces, circular buffers |
| 5 | **PriorityQueue** | Binary heaps, sift-up / sift-down |
| 6 | **HashTable** | Separate chaining, FNV-1a, rehashing |
| 7 | **AVLTree** → **RedBlackTree** | Self-balancing BSTs, rotations and recoloring |
| 8 | **SkipList** | Probabilistic data structures |
| 9 | **BPlusTree** | Disk page format, split & merge, free list |
| 10 | **HuffmanCoding** | Composing data structures to build an algorithm — min-heap tree building, bit-level encoding |
| 11 | **test_project** | Multi-container composition, cascading ownership, real project structure |

---

## Implementation & Design Considerations

### Portable Macros

`ds_xxx_type.h` uses `static inline` functions for clone and destroy logic, invoked by macros. This pattern is compatible with all C99 compilers (GCC, Clang, MSVC) and supports step-through debugging.

Key points:

- **`static`** prevents duplicate-symbol errors when `_type.h` is included by multiple `.c` files.
- **`inline`** lets the compiler eliminate call overhead.
- Macro call syntax remains unchanged — all `.c` / `.h` implementation files stay untouched when you modify `_type.h`.
- Core invariant: **`_type.h` is the only file you ever need to modify.** (When using the code generator, you still only edit the mother `_type.h`, then run the script.)
- Exception — disk-backed containers: B+ tree key/value types are fixed-size POD, read from and written to disk pages via `fread` / `fwrite`. CLONE/DESTROY macros are not used.

### Capacity Growth

All array-based containers use a doubling growth strategy: initial capacity 0 → 1 on first insert → doubles each time thereafter. HashTable triggers a rehash when `size >= capacity` (load factor 1.0), doubling the bucket count.

### Design Trade-offs

- **No `void*` erasure.** Generics via macros in `_type.h`. Compile-time type safety. The inherent limitation of macros (one type per compilation unit) is solved by the `generate.py` code generator.
- **Return values are the only error channel.** No `errno`, `assert`, or `exit`. All errors are reported through return values — no hidden control flow.
- **Single-threaded.** No locks, no atomic operations. Assumes a single-threaded environment.
- **No build system.** Each data structure is a standalone set of `.c` / `.h` files — no Makefile or CMake required.
- **Opaque structs.** Internal fields are hidden from users. All operations are direct function calls — no vtable overhead.

---

## Comparison with Alternatives

| Approach | Type Safety | Ownership Clarity | Learning Curve | Use Case |
|------|:--:|:--:|:--:|------|
| **This library** | Compile-time (macros) | Deep-copy model, clear boundaries | Medium | Learn data structures & C generics design; small projects can adopt |
| `C++ STL` | Compile-time (templates) | RAII | Low (if you know C++) | Projects that can use a C++ compiler |
| `glib` / `libuv` etc. | None (void* + casts) | Error-prone ("who frees?" has no standard answer) | Low | Quick internal prototypes |
| `klib` / `uthash` | Macros (header-only) | Varies by library, no uniform convention | Medium | Minimalist, header-only needs, don't mind macro debugging |
| Hand-rolled structs | Compile-time | Manual management | Low to start, hard to get right | One-off, simple cases |

**In short:** This library is for you if you write C, want to understand generic container and data structure design through readable source code, care about clear ownership semantics, and are willing to configure a `_type.h` file per container.

---

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

---

*If you want to dive deeper after reading this README, every module's `_type.h` and `.h` files are the best entry points — the former contains type customization examples, the latter contains detailed function-level documentation and ownership semantics.*
