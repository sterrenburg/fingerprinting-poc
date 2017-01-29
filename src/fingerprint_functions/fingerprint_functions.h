/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#ifndef FINGERPRINTING_POC_FINGERPRINT_FUNCTIONS_H
#define FINGERPRINTING_POC_FINGERPRINT_FUNCTIONS_H

#define FINGERPRINT_FUNCTIONS_SIZE 1

struct fingerprint_function {
    char signature_handle[50];
    int (*function)(char *hostname, CURL *curl, const char *output);
};

struct fingerprint_function fingerprint_functions[FINGERPRINT_FUNCTIONS_SIZE];

#endif //FINGERPRINTING_POC_FINGERPRINT_FUNCTIONS_H
