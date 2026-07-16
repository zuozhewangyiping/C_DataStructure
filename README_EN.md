# C Data Structure Library

A learning-oriented yet fully functional C data structure library — type-safe generic containers via compile-time macros, deep-copy ownership, opaque pointer APIs, and a byte-level Huffman compression algorithm.

[:cn: 中文版本](README_CN.md)

---

## 30-Second Try

```bash
git clone https://github.com/zuozhewangyiping/C_DataStructure.git
cd C_DataStructure

# Linux / macOS
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && ./demo

# Windows (CMD / PowerShell)
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c && demo

# Output: is_empty: 1, size: 0, capacity: 0
# ...（more output follows）
```

No dependencies. No build system. Just `.c` and `.h` files. **Requires C99 or later.**

---

## Table of Contents

- [Data Structures at a Glance](#data-structures-at-a-glance)
  - [In-Memory Containers](#in-memory-containers)
  - [Algorithms](#algorithms)
  - [Disk-Based](#disk-based)
- [Algorithms](#algorithms)
- [Quick Start](#quick-start)
- [API Conventions](#api-conventions)
- [Advanced Features](#advanced-features)
- [Two Container Families, Two Erase Conventions](#two-container-families-two-erase-conventions)
- [Capacity Growth](#capacity-growth)
- [Design Trade-offs](#design-trade-offs)
- [Writing Portable Macros](#writing-portable-macros)
- [test_project](#test_project)
- [Comparison with Alternatives](#comparison-with-alternatives)
- [License](#license)

---

## Data Structures at a Glance

### In-Memory Containers

#### Array-Based (Contiguous Storage)

| Structure | Insert | Delete | Lookup | Notes |
|---|---|---|---|---|
| **DynamicArray** | O(1)* / O(n) | O(1)* / O(n) | O(1) / O(n) | Generic resizable array (C++ `std::vector` equivalent) |
| **Stack** | O(1)* | O(1) | O(1) peek | LIFO, built on dynamic array |
| **Deque** | O(1)* | O(1) | O(1) | Double-ended queue, circular buffer |
| **Queue** | O(1)* | O(1) | O(1) peek | FIFO, circular buffer |
| **String** | O(1)* / O(n) | O(n) | O(1) | Char-oriented dynamic array with string ops |

> O(1)* = amortized O(1). Lookup shows both indexed O(1) and search O(n).

#### Node-Based (Linked Storage)

| Structure | Insert | Delete | Lookup | Notes |
|---|---|---|---|---|
| **SinglyLinkedList** | O(1) head / O(n) | O(1) head / O(n) | O(n) | Index-based and cursor-based ops |
| **DoubleLinkedList** | O(1) head+tail / O(n) | O(1) head+tail / O(n) | O(n) | Bidirectional traversal |
| **SkipList** | O(log n) prob. | O(log n) prob. | O(log n) prob. | Probabilistically balanced, multi-level index, cursor ops |

#### Tree-Based (Self-Balancing BST)

| Structure | Insert | Delete | Search | Min/Max | Traverse | Notes |
|---|---|---|---|---|---|---|
| **AVLTree** | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | LL/LR/RL/RR rotations |
| **RedBlackTree** | O(log n) | O(log n) | O(log n) | O(log n) | O(n) | Standard fixup rules |

#### Specialized

| Structure | Insert | Delete | Search | Peek | Notes |
|---|---|---|---|---|---|
| **PriorityQueue (Min)** | O(log n) | O(log n) pop | — | O(1) | Binary min-heap |
| **PriorityQueue (Max)** | O(log n) | O(log n) pop | — | O(1) | Binary max-heap |
| **HashTable** | O(1) avg | O(1) avg | O(1) avg | — | Separate chaining, FNV-1a hash |

### Algorithms

| Algorithm | Notes |
|---|---|
| **HuffmanCoding** | Byte-level Huffman compression with 8-bit packed output, internal min-heap tree construction, full encode/decode round-trip |

### Disk-Based (Persistent Storage)

| Structure | Insert | Delete | Search | Notes |
|---|---|---|---|---|
| **BPlusTree** | O(log n) | O(log n) | O(log n) | 4KB-page B+ tree, data persisted to file, key/value are fixed-size POD |

---

## Quick Start

### 1. Configure Your Element Type

Edit the `_type.h` file for your chosen data structure. This is the **only file you need to modify**:

```c
// ds_dynamicarray_type.h
typedef struct {
    int id;
    char *name;       // heap-allocated field
    double score;
} ds_dynamicarray_type;

#define DS_DYNAMICARRAY_DESTROY_ELEMENT(e)  \
    do { free((e).name); (e).name = NULL; } while (0)

// Write your clone function, then have the macro call it
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
#define DS_DYNAMICARRAY_MATCH(e, target) ((e).id == target ? 1 : 0)
```

### 2. Use the Data Structure

```c
#include "ds_dynamicarray.h"

int main() {
    DS_DynamicArray *arr = ds_dynamicarray_create();

    DS_DYNAMICARRAY_TYPE tmp1 = {1, strdup("Alice"), 95.5};
    ds_dynamicarray_push_back(arr, tmp1);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp1);  // container owns a deep copy now

    DS_DYNAMICARRAY_TYPE tmp2 = {2, strdup("Bob"), 87.0};
    ds_dynamicarray_push_back(arr, tmp2);
    DS_DYNAMICARRAY_DESTROY_ELEMENT(tmp2);

    int size = ds_dynamicarray_size(arr);  // 2

    ds_dynamicarray_destroy(arr);  // frees all elements + container
    return 0;
}
```

> **IMPORTANT: Clean Up Your Own Copy**
>
> The container deep-copies your data on insert / set / push. **You remain the owner**
> of the original you passed in. If your element type has heap fields (like `char *name`
> from `strdup`), you **must** call `DESTROY_ELEMENT` on your local copy afterward.
>
> If your element has only scalar fields (int, double, etc.), passing `(type){10}`
> needs no cleanup — nothing on the heap.

### 3. Compile

```bash
# Compile a single data structure with its demo
gcc -o demo DynamicArray/ds_dynamicarray.c DynamicArray/main.c

# Compile the integration test project
gcc -o test_project test_project/ds_dynamicarray.c test_project/ds_string.c test_project/main.c
```

All data structures are independent — compile only the `.c` files you need.

---

## API Conventions

### Return Values

| Category | Returns |
|---|---|
| `create` / `clone` | Pointer on success, `NULL` on failure |
| `destroy` | `void` (safe to pass `NULL`) |
| Query (`size`, `capacity`, `is_empty`) | Non-negative on success, `-1` for `NULL` input |
| Mutation (`push`, `insert`, `set`, `erase`...) | `1` on success, `0` on failure |
| Element retrieval (`get`, `find`, `peek`, `pop`) | `1` on success, `0` on failure; element via output param |
| Tree cursors (`search`, `find_min`, `successor`...) | Cursor pointer, `NULL` if not found / exhausted |

### Output Parameter Pattern

Functions that return elements use a **pointer-to-pointer** output parameter:

```c
DS_DYNAMICARRAY_TYPE *data;
if (ds_dynamicarray_get(array, 0, &data)) {
    printf("value = %d\n", data->value);
    data->value = 999;  // can modify in place
}
```

### NULL Safety

All public functions handle `NULL` container pointers by returning an error code (`0`, `-1`, or `NULL`). No segfaults.

### Deep Copy Semantics

All insert, set, and clone operations deep-copy elements via `CLONE_ELEMENT`. The container owns its copies; the caller retains ownership of the original data.

### Naming Convention

```
ds_<container>_<operation>
```

Examples: `ds_dynamicarray_push_back`, `ds_avltree_insert`, `ds_hashtable_find`.

---

## Advanced Features

### Cursor-Based Iteration (Trees & Linked Lists)

Trees and linked lists provide opaque cursor types, avoiding index-based overhead:

```c
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
    (void)user_data;
    printf("key=%d, value=%d\n", value->key, value->value);
}
ds_avltree_traverse_inorder_value(tree, NULL, print_value);
```

Supported traversal orders: preorder, inorder, postorder, level-order.

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
ds_hashtable_insert(ht, value);  // fails if key already exists (returns 0)
ds_hashtable_put(ht, value);     // overwrites if key exists, inserts otherwise
```

---

## Two Container Families, Two Erase Conventions

This library's containers fall into two categories with different `erase` / `pop` lifetime semantics:

### Array-Based — DynamicArray, Deque, Stack, Queue

Data lives in contiguous memory. `erase` returns a pointer **directly into that memory** — not a separate heap block. The next `push` / `insert` overwrites that slot, invalidating the pointer.

```c
DS_DYNAMICARRAY_TYPE *data;
ds_dynamicarray_erase(da, 0, &data);

DS_DYNAMICARRAY_DESTROY_ELEMENT(*data);  // free heap members (if any)
// Do NOT free(data) — it points into the container
```

### Node-Based — SinglyLinkedList, DoubleLinkedList, HashTable

Data lives in separately allocated nodes. `erase` **shallow-copies the node's data into a new `malloc`'d block**, frees the node, and returns the new block. **You are responsible for `free(data)`.**

```c
DS_HASHTABLE_TYPE *data;
ds_hashtable_erase(ht, 100, &data);

DS_HASHTABLE_DESTROY_ELEMENT(*data);  // free heap members (if any)
free(data);                           // Must free — it was malloc'd for you
```

### `_and_destroy` Variants

Don't want to manage this manually? Use the `_and_destroy` variants — the container handles **DESTROY_ELEMENT + free** (node-based) for you:

```c
// Array-based automatic cleanup
ds_dynamicarray_pop_back_and_destroy(array);

// Node-based automatic cleanup
ds_hashtable_erase_and_destroy(ht, 100);
```

---

## Capacity Growth

All array-based containers use a **doubling growth strategy**: initial capacity 0 → 1 on first insert → doubles each time thereafter. HashTable triggers rehashing when `size >= capacity` (load factor = 1.0), doubling the bucket count.

---

## Design Trade-offs

- **No `void*` erasure.** Generics via macros in `_type.h`. Compile-time type safety at the cost of recompilation when the element type changes.
- **No error codes beyond return values.** No `errno`, `assert`, or `exit`. All errors reported through return values — no hidden control flow.
- **Single-threaded.** No locking or atomic operations. Assumes a single-threaded environment.
- **No build system.** Each data structure is a standalone set of `.c`/`.h` files — no Makefile or CMake required.
- **Opaque structs.** Internal fields hidden from users. Every operation is a direct function call — no vtable overhead.

---

## Writing Portable Macros

The `_type.h` files use **`static inline` functions** for clone and destroy logic, invoked via macros. This pattern is compatible with all C99 compilers (GCC, Clang, MSVC) and supports step-through debugging.

The default element types contain only scalar fields (int, etc.) and work out of the box. When adding heap-allocated fields, simply edit the struct definition in `_type.h`, write your own clone / destroy functions, and update the macros.

Key points:

- **`static`** avoids duplicate-symbol errors when `_type.h` is included by multiple `.c` files.
- **`inline`** lets the compiler eliminate call overhead.
- The macro **call syntax stays the same** — all `.c` / `.h` files remain untouched.
- Core invariant: **`_type.h` is the only file you ever need to modify.**

---

## test_project

The `test_project/` directory demonstrates composing two data structures: `DynamicArray` storing `String` elements to build a student grade management system:

```c
// test_project/main.c — excerpt
DS_DynamicArray *roster = ds_dynamicarray_create();

// Add a student: String for name, int for grade
DS_String *name1 = ds_cstr_to_string("Alice");
ds_dynamicarray_push_back(roster, (DS_DYNAMICARRAY_TYPE){.name = name1, .grade = 95});

DS_String *name2 = ds_cstr_to_string("Bob");
ds_dynamicarray_push_back(roster, (DS_DYNAMICARRAY_TYPE){.name = name2, .grade = 87});

// Look up a student
DS_DYNAMICARRAY_TYPE *entry;
if (ds_dynamicarray_find(roster, "Alice", &entry)) {
    printf("%s: %d\n", ds_string_to_cstr(entry->name), entry->grade);
}

ds_dynamicarray_destroy(roster);  // recursively destroys nested Strings
```

It demonstrates how `DESTROY_ELEMENT` and `CLONE_ELEMENT` macros should handle nested container lifecycles — when your `DynamicArray` element contains a `DS_String *`, the macros must call `ds_string_destroy` and `ds_string_clone` respectively.

---

## Comparison with Alternatives

| Approach | Type Safety | Memory Safety | Learning Curve | When to Use |
|---|---|---|---|---|
| **This library** | Compile-time (macros) | Deep-copy ownership model | Medium (learn the macro system) | You want readable, reusable generic containers |
| Hand-rolled structs | Compile-time | Manual (you own it) | Low to start, high to get right | One-off, simple cases |
| `void*` + function pointers | None (runtime casts) | Error-prone ("who frees what?") | Low | Quick internal prototyping |
| C++ STL | Compile-time (templates) | RAII | Low (if you know C++) | You can use a C++ compiler |
| `klib` / `uthash` | Macros (header-only) | Varies | Medium | Minimalist, header-only needs |

**Bottom line:** This library is for you if you're writing C, want type-safe generic containers, care about clear ownership semantics, and are willing to configure a `_type.h` file per data structure.

---

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.
