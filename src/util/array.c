#include "array.h"

void array_init(array_t* array, size_t size)
{
    array->data = malloc(1000*size);//calloc(1000, size);
    array->size = size;
    array->length = 0;
    array->max = 1000;
}

void array_clear(array_t* array)
{
    free(array->data);
    array_init(array, array->size);
}

void array_expand(array_t* array)
{
    int ns = (array->max * array->size) * 2;
    array->data = realloc(array->data, ns);
    array->max *= 2;
}

void array_push(array_t* array, void* data)
{
    if (array->length >= array->max) {
        array_expand(array);
    }
    memcpy(array->data+array->length*array->size, data, array->size);
    array->length++;
}

void* array_get(array_t* array, int index)
{
    return array->data+index*array->size;
}
