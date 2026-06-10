#include <stdio.h>
#include <stdlib.h>
#include "ds_string.h"

int main(void)
{
    DS_String *s = ds_string_create();

    printf("is_empty: %d, size: %d, capacity: %d\n",
           ds_string_is_empty(s), ds_string_size(s), ds_string_capacity(s));

    ds_string_push_back(s, 'a');
    ds_string_push_back(s, 'b');
    ds_string_push_back(s, 'c');
    printf("after push_back('a','b','c'): size=%d\n", ds_string_size(s));

    ds_string_insert(s, 1, 'x');
    printf("after insert(1, 'x'): size=%d\n", ds_string_size(s));

    char *ch;
    ds_string_get(s, 0, &ch);
    printf("get[0]=%c\n", *ch);

    ds_string_set(s, 0, 'z');

    int pos = ds_string_find_char(s, 'x');
    printf("find_char('x'): %d\n", pos);

    ds_string_reserve(s, 32);
    printf("after reserve(32): capacity=%d\n", ds_string_capacity(s));

    ds_string_erase(s, 1);
    printf("after erase(1): size=%d\n", ds_string_size(s));

    ds_string_pop_back(s);
    printf("after pop_back: size=%d\n", ds_string_size(s));

    ds_string_shrink_to_fit(s);
    printf("after shrink_to_fit: capacity=%d\n", ds_string_capacity(s));

    int judge;
    DS_String *clone = ds_string_clone(s, &judge);
    printf("clone size=%d\n", ds_string_size(clone));

    DS_String *s2 = ds_cstr_to_string("hello");
    DS_String *s3 = ds_cstr_to_string("world");
    ds_string_concat(s2, s3);
    char *cstr = ds_string_to_cstr(s2);
    printf("concat('hello','world'): %s\n", cstr);
    free(cstr);

    int cmp = ds_string_compare(s2, s3);
    printf("compare(s2, 'world'): %d\n", cmp);

    DS_String *sub = ds_string_substring(s2, 0, 5);
    cstr = ds_string_to_cstr(sub);
    printf("substring(0,5): %s\n", cstr);
    free(cstr);

    DS_String *from_cstr = ds_cstr_to_string("test");
    cstr = ds_string_to_cstr(from_cstr);
    printf("cstr_to_string / string_to_cstr: %s\n", cstr);
    free(cstr);

    ds_string_destroy(s);
    ds_string_destroy(s2);
    ds_string_destroy(s3);
    ds_string_destroy(sub);
    ds_string_destroy(from_cstr);
    ds_string_destroy(clone);
    return 0;
}
