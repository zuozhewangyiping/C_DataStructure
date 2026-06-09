#include <stdlib.h>
#include "ds_string.h"

struct DS_String
{
    char *data;
    int size;
    int capacity;
};

DS_String *ds_string_create(void)
{
    DS_String *str = (DS_String *)malloc(sizeof(DS_String));
    if (str == NULL)
    {
        return NULL;
    }

    str->data = NULL;
    str->capacity = 0;
    str->size = 0;

    return str;
}

void ds_string_destroy(DS_String *str)
{
    if (str == NULL)
    {
        return;
    }

    free(str->data);
    free(str);
}

int ds_string_size(const DS_String *str)
{
    if (str == NULL)
    {
        return -1;
    }

    return str->size;
}

int ds_string_capacity(const DS_String *str)
{
    if (str == NULL)
    {
        return -1;
    }

    return str->capacity;
}

int ds_string_is_empty(const DS_String *str)
{
    if (str == NULL)
    {
        return -1;
    }

    return (str->size == 0 ? 1 : 0);
}

int ds_string_get(DS_String *str, int index, char **x)
{
    if (x == NULL || str == NULL || index < 0 || index >= str->size)
    {
        return 0;
    }

    *x = &(str->data[index]);

    return 1;
}

int ds_string_set(DS_String *str, int index, char value)
{
    if (str == NULL || index < 0 || index >= str->size)
    {
        return 0;
    }

    str->data[index] = value;

    return 1;
}

int ds_string_find_char(const DS_String *str, char ch)
{
    if (str == NULL)
    {
        return -1;
    }

    int i;
    for (i = 0; i < str->size; i++)
    {
        if (str->data[i] == ch)
        {
            return i;
        }
    }

    return -1;
}

static int capacity_expansion(DS_String *str)
{
    if (str == NULL)
    {
        return 0;
    }

    int new_capacity = 0;

    if (str->capacity == 0)
    {
        new_capacity = 1;
    }
    else
    {
        new_capacity = 2 * str->capacity;
    }

    char *temp = (char *)realloc(str->data, new_capacity * sizeof(char));
    if (temp == NULL)
    {
        return 0;
    }

    str->data = temp;
    str->capacity = new_capacity;

    return 1;
}

int ds_string_push_back(DS_String *str, char value)
{
    if (str == NULL)
    {
        return 0;
    }

    if (str->size >= str->capacity)
    {
        if (!capacity_expansion(str))
        {
            return 0;
        }
    }

    str->data[str->size] = value;
    str->size++;

    return 1;
}

int ds_string_pop_back(DS_String *str)
{
    if (str == NULL || str->size == 0)
    {
        return 0;
    }

    str->size--;

    return 1;
}

int ds_string_insert(DS_String *str, int index, char value)
{
    if (str == NULL || index < 0 || index > str->size)
    {
        return 0;
    }

    if (str->size >= str->capacity)
    {
        if (!capacity_expansion(str))
        {
            return 0;
        }
    }

    int i;
    for (i = str->size; i > index; i--)
    {
        str->data[i] = str->data[i - 1];
    }

    str->data[index] = value;
    str->size++;

    return 1;
}

int ds_string_erase(DS_String *str, int index)
{
    if (str == NULL || index < 0 || index >= str->size)
    {
        return 0;
    }

    int i;
    for (i = index; i < str->size - 1; i++)
    {
        str->data[i] = str->data[i + 1];
    }

    str->size--;

    return 1;
}

DS_String *ds_string_clone(const DS_String *str, int *judge)
{
    if (judge == NULL)
    {
        return NULL;
    }

    *judge = 1;

    if (str == NULL)
    {
        *judge = 1;
        return NULL;
    }

    DS_String *new_str = ds_string_create();
    if (new_str == NULL)
    {
        *judge = 0;
        return NULL;
    }

    if (str->capacity == 0)
    {
        *judge = 1;
        return new_str;
    }

    if (!ds_string_reserve(new_str, str->capacity))
    {
        ds_string_destroy(new_str);
        *judge = 0;
        return NULL;
    }

    int i;
    for (i = 0; i < str->size; i++)
    {
        new_str->data[i] = str->data[i];
    }

    new_str->size = str->size;

    return new_str;
}

int ds_string_reserve(DS_String *str, int new_capacity)
{
    if (str == NULL || new_capacity < str->size)
    {
        return 0;
    }

    if (new_capacity == str->capacity)
    {
        return 1;
    }

    if (new_capacity == 0)
    {
        free(str->data);
        str->data = NULL;
        str->capacity = 0;

        return 1;
    }

    char *new_data = (char *)realloc(str->data, new_capacity * sizeof(char));
    if (new_data == NULL)
    {
        return 0;
    }

    str->data = new_data;
    str->capacity = new_capacity;

    return 1;
}

int ds_string_shrink_to_fit(DS_String *str)
{
    if (str == NULL)
    {
        return 0;
    }

    return ds_string_reserve(str, str->size);
}

int ds_string_concat(DS_String *str1, const DS_String *str2)
{
    if (str1 == NULL)
    {
        return 0;
    }
    if (str2 == NULL || str2->size == 0)
    {
        return 1;
    }

    if (str1 == str2) // 自拼接
    {
        if (str1->capacity < 2 * str1->size)
        {
            if (!ds_string_reserve(str1, 2 * str1->size))
            {
                return 0;
            }
        }

        int i;
        for (i = 0; i < str1->size; i++)
        {
            str1->data[str1->size + i] = str1->data[i];
        }

        str1->size *= 2;
    }
    else
    {
        if (str1->capacity < str1->size + str2->size)
        {
            if (!ds_string_reserve(str1, str1->size + str2->size))
            {
                return 0;
            }
        }

        int i;
        for (i = 0; i < str2->size; i++)
        {
            str1->data[str1->size + i] = str2->data[i];
        }

        str1->size += str2->size;
    }

    return 1;
}

/*----------------------------------------------------------------------------------------------------*/
/*  String 专属操作                                                                                    */
/*----------------------------------------------------------------------------------------------------*/

static int Min(int a, int b)
{
    return a <= b ? a : b;
}

int ds_string_compare(const DS_String *str1, const DS_String *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return -2;
    }

    int min_length = Min(str1->size, str2->size);

    int i;
    for (i = 0; i < min_length; i++)
    {
        if (str1->data[i] != str2->data[i])
        {
            return ((unsigned char)str1->data[i] > (unsigned char)str2->data[i] ? 1 : -1);
        }
    }

    if (str1->size == str2->size)
    {
        return 0;
    }
    else
    {
        return (str1->size > str2->size ? 1 : -1);
    }
}

DS_String *ds_string_substring(const DS_String *str, int pos, int len)
{
    if (str == NULL || pos < 0 || len < 0 || pos + len > str->size)
    {
        return NULL;
    }

    DS_String *substr = ds_string_create();
    if (substr == NULL)
    {
        return NULL;
    }

    if (!ds_string_reserve(substr, len))
    {
        ds_string_destroy(substr);
        return NULL;
    }

    int i;
    for (i = 0; i < len; i++)
    {
        substr->data[i] = str->data[pos + i];
    }

    substr->size = len;

    return substr;
}

DS_String *ds_cstr_to_string(const char *cstr)
{
    if (cstr == NULL)
    {
        return NULL;
    }

    DS_String *str = ds_string_create();
    if (str == NULL)
    {
        return NULL;
    }

    int cstr_length = 0;

    int i;
    for (i = 0; cstr[i] != '\0'; i++)
    {
        cstr_length++;
    }

    if (!ds_string_reserve(str, cstr_length))
    {
        ds_string_destroy(str);
        return NULL;
    }

    for (i = 0; i < cstr_length; i++)
    {
        str->data[i] = cstr[i];
    }

    str->size = cstr_length;

    return str;
}

char *ds_string_to_cstr(const DS_String *str)
{
    if (str == NULL)
    {
        return NULL;
    }

    char *cstr = (char *)malloc((str->size + 1) * sizeof(char));
    if (cstr == NULL)
    {
        return NULL;
    }

    int i;
    for (i = 0; i < str->size; i++)
    {
        cstr[i] = str->data[i];
    }

    cstr[i] = '\0';

    return cstr;
}
