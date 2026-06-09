#include <stdio.h>
#include <stdlib.h>
#include "ds_string.h"

int main(void)
{
    DS_String *s1 = ds_cstr_to_string("hello");
    DS_String *s2 = ds_cstr_to_string("world");

    ds_string_concat(s1, s2);

    char *cstr = ds_string_to_cstr(s1);
    printf("string: %s (size: %d)\n", cstr, ds_string_size(s1));
    free(cstr);

    ds_string_destroy(s1);
    ds_string_destroy(s2);
    return 0;
}
