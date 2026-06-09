#ifndef DS_STRING_H
#define DS_STRING_H

typedef struct DS_String DS_String;

DS_String *ds_string_create(void);
void ds_string_destroy(DS_String *str);

int ds_string_size(const DS_String *str);     /* return -1 if str is NULL */
int ds_string_capacity(const DS_String *str); /* return -1 if str is NULL */
int ds_string_is_empty(const DS_String *str); /* return 1 if empty, 0 if not empty, -1 if str is NULL */

int ds_string_get(DS_String *str, int index, char **x); /* return 1 if success, 0 if str NULL, x NULL, or index out of range */
/*
    char *ch;

    if (ds_string_get(str, 0, &ch))
    {
        printf("char=%c\n", *ch); // 可以读取
        *ch = 'A';                // 可以修改
    }
*/

int ds_string_set(DS_String *str, int index, char value); /* return 1 if success, 0 if str NULL or index out of range */
/*
    ds_string_set(str, 1, 'X'); // 将索引 1 的字符改为 'X'
*/

int ds_string_find_char(const DS_String *str, char ch); /* return index of first occurrence, or -1 if str NULL or not found */
/*
    int pos = ds_string_find_char(str, 'a');
    if (pos != -1)
    {
        // 在 pos 位置找到了 'a'
    }
*/

int ds_string_push_back(DS_String *str, char value); /* return 1 if success, 0 if realloc failed or str NULL */
/*
    ds_string_push_back(str, '?');
*/

int ds_string_pop_back(DS_String *str); /* return 1 if success, 0 if str NULL, or array empty */
/*
    if (ds_string_pop_back(str))
    {
        // 尾部字符已移除
    }
*/

int ds_string_insert(DS_String *str, int index, char value); /* return 1 if success, 0 if str NULL, index out of range, or realloc failed */
/*
    ds_string_insert(str, 2, '-'); // 在索引 2 处插入 '-'
*/

int ds_string_erase(DS_String *str, int index); /* return 1 if success, 0 if str NULL, or index out of range */
/*
    if (ds_string_erase(str, 0))
    {
        // 索引 0 处字符已移除
    }
*/

DS_String *ds_string_clone(const DS_String *str, int *judge); /* return pointer if success, NULL if malloc failed or judge is NULL; *judge will be 1 if clone successfully (even if str is NULL), 0 if malloc failed */
/*
    int judge;
    DS_String *copy = ds_string_clone(str, &judge);
    if (copy == NULL && judge == 0)
    {
        // malloc 失败，克隆未完成
    }
*/

int ds_string_reserve(DS_String *str, int new_capacity); /* return 1 if success, 0 if realloc failed or new_capacity smaller than current size */
int ds_string_shrink_to_fit(DS_String *str);             /* return 1 if success, 0 if str NULL or realloc failed */
/*
    ds_string_reserve(str, 100);   // 预留 100 字节空间
    ds_string_shrink_to_fit(str);  // 回收多余内存
*/

int ds_string_concat(DS_String *str1, const DS_String *str2); /* return 1 if success, 0 if str1 NULL or realloc failed */
/*
    ds_string_concat(str1, str2); // str1 += str2，str2 不变
    ds_string_concat(str1, str1); // 自拼接：str1 = str1 + str1，支持
*/

int ds_string_compare(const DS_String *s1, const DS_String *s2); /* return 0 if s1 == s2, 1 if s1 > s2, -1 if s1 < s2, -2 if error */
/*
    int cmp = ds_string_compare(s1, s2); // 字典序比较
    // cmp == 0  → s1 等于 s2
    // cmp == 1  → s1 大于 s2
    // cmp == -1 → s1 小于 s2
    // cmp == -2 → 参数无效
*/

DS_String *ds_string_substring(const DS_String *src, int pos, int len); /* return pointer if success, NULL if invalid parameters */
/*
    DS_String *sub = ds_string_substring(str, 2, 5); // 从索引 2 开始取 5 个字符
    if (sub == NULL)
    {
        // 参数无效或 malloc 失败
    }
*/

DS_String *ds_cstr_to_string(const char *cstr); /* return pointer if success, NULL if malloc failed or cstr NULL */
char *ds_string_to_cstr(const DS_String *str);  /* return pointer if success, NULL if malloc failed or str NULL */
/*
    DS_String *str = ds_cstr_to_string("hello"); // C 字符串 → String
    char *cstr = ds_string_to_cstr(str);          // String → C 字符串
    printf("%s\n", cstr);
    free(cstr);                                   // 记得释放 cstr
*/

#endif

/* !WARNING!
 * 数据所有权约定
 * --------------
 * String 存储的是纯 char 元素，无堆资源嵌套，内存管理相对简单：
 *
 *   - get 返回的指针指向 String 内部数据，可直接读取和修改。此指针在下一次导致
 *     扩容的操作（push_back / insert / reserve）后可能失效。
 *
 *   - pop_back / erase 直接逻辑删除（size--），不返回被删除字符。
 *
 *   - clone / cstr_to_string / substring 返回新分配的 DS_String，调用者使用
 *     完毕后需 ds_string_destroy 释放。
 *
 *   - ds_string_to_cstr 返回的 C 字符串由调用者负责 free()。
 *
 *   - concat 只在 str1 容量不足时扩容，str2 保持不变（对标 std::string::operator+=）。
 */
