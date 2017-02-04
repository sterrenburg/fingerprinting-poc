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
#define FINGERPRINT_HEADER_FUNCTIONS_SIZE 1

struct string {
    char *ptr;
    size_t len;
};

struct Fingerprint_function {
    char signature_handle[50];
//    int (*function)(char *hostname, CURL *curl, const char *output);
    int (*function)(void *ptr, const char *output);
};

struct Fingerprint_function fingerprint_functions[FINGERPRINT_FUNCTIONS_SIZE];
struct Fingerprint_function fingerprint_header_functions[FINGERPRINT_HEADER_FUNCTIONS_SIZE];

#endif //FINGERPRINTING_POC_FINGERPRINT_FUNCTIONS_H
