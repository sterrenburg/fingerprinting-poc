/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#ifndef FINGERPRINTING_POC_ARRAY_H
#define FINGERPRINTING_POC_ARRAY_H

#include <stdlib.h>

typedef struct {
    char **array;
    size_t used;
    size_t size;
} Array;

void array_init(Array *a, size_t initialSize);

void array_insert(Array *a, char *element);

void array_free(Array *a);

#endif //FINGERPRINTING_POC_ARRAY_H
