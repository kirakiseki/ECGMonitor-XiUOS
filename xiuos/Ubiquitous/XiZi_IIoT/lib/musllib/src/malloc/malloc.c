#include <malloc.h>
#include <stdlib.h>
#include "xs_memory.h"

// implement of malloc for XiUOS
void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return (void*)x_malloc(size);
}

void* realloc(void* old, size_t new_len) {
    if (new_len == 0) {
        return NULL;
    }
    return x_realloc(old, new_len);
}

void free(void* ptr) {
    x_free(ptr);
}

