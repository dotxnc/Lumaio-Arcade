#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc32.h"

typedef struct hashmap_t {
    unsigned long key;
    void* value;
    size_t size;
    struct hashmap_t* next;
} hashmap_t;

unsigned long hashmap_hash(const char*);
void hashmap_push(hashmap_t*, const char*, void*, size_t);
void hashmap_pushvalue(hashmap_t*, const char*, void*);
void* hashmap_get(hashmap_t*, const char*);

#endif
