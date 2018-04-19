#include "hashmap.h"

unsigned long hashmap_hash(const char* key)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash<<5)+hash) + c;
    }
    return hash;
}

void hashmap_push(hashmap_t* map, const char* key, void* data, size_t size)
{
    unsigned long hash = hashmap_hash(key);
    hashmap_t* current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = malloc(sizeof(hashmap_t));
    current->next->key = hash;
    current->next->value = malloc(size);
    current->next->next = NULL;
    memcpy(current->next->value, data, size);
}

void hashmap_pushvalue(hashmap_t* map, const char* key, void* ptr)
{
    unsigned long hash = hashmap_hash(key);
    hashmap_t* current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(hashmap_t));
    current->next->key = hash;
    current->next->value = ptr;
    current->next->next = NULL;
}

void* hashmap_get(hashmap_t* map, const char* key)
{
    unsigned long hash = hashmap_hash(key);
    hashmap_t* current = map;
    while (current != NULL) {
        if (current->key == hash) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}
