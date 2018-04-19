#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct array_t {
    void* data;
    size_t size;
    size_t length;
    size_t max;
} array_t;

void array_init(array_t*, size_t);
void array_clear(array_t*);
void array_expand(array_t*);
void array_push(array_t*, void*);
void* array_get(array_t*, int);

#endif
