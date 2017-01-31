/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include "array.h"

void array_init(Array *a, size_t initialSize) {
    a->array = (char **)malloc(initialSize * sizeof(char *));
    a->used = 0;
    a->size = initialSize;
}

void array_insert(Array *a, char *element) {
    if (a->used == a->size) {
        a->size *= 2;
        a->array = (char **)realloc(a->array, a->size * sizeof(char *));
    }
    a->array[a->used++] = element;
}

void array_free(Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}