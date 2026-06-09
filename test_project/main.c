#include <stdio.h>
#include <stdlib.h>
#include "ds_dynamicarray.h"
#include "ds_string.h"

int main()
{
    DS_DynamicArray *grade = ds_dynamicarray_create();
    if (grade == NULL)
    {
        printf("malloc failed!\n");
        exit(0);
    }

    DS_String *student1 = ds_cstr_to_string("Zhangsan");
    DS_String *student2 = ds_cstr_to_string("Wangwu");
    DS_String *student3 = ds_cstr_to_string("Lisi");

    ds_dynamicarray_push_back(grade, (DS_DYNAMICARRAY_TYPE){student1, 2025001, 78});
    ds_dynamicarray_push_back(grade, (DS_DYNAMICARRAY_TYPE){student2, 2025003, 66});
    ds_dynamicarray_insert(grade, 1, (DS_DYNAMICARRAY_TYPE){student3, 2025002, 91});

    ds_string_destroy(student1);
    ds_string_destroy(student2);
    ds_string_destroy(student3);

    int i;
    DS_DYNAMICARRAY_TYPE *inf;
    for (i = 0; i < ds_dynamicarray_size(grade); i++)
    {
        ds_dynamicarray_get(grade, i, &inf);

        char *name = ds_string_to_cstr(inf->name);

        printf("%d %s : %d\n", inf->id, name, inf->score);

        free(name);
    }

    printf("\n");

    int judge = 1;
    DS_DynamicArray *grade_c = ds_dynamicarray_clone(grade, &judge);
    if (judge == 1)
    {
        ds_dynamicarray_erase_and_destroy(grade_c, 2);

        DS_DYNAMICARRAY_TYPE *inf_c;
        for (i = 0; i < ds_dynamicarray_size(grade_c); i++)
        {
            ds_dynamicarray_get(grade_c, i, &inf_c);

            char *name_c = ds_string_to_cstr(inf_c->name);

            printf("%d %s : %d\n", inf_c->id, name_c, inf_c->score);

            free(name_c);
        }

        ds_dynamicarray_destroy(grade_c);
    }

    ds_dynamicarray_destroy(grade);

    return 0;
}
