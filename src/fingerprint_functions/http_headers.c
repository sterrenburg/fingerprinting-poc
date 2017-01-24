/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */
         
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

#include "http_headers.h"
#include "../debug.h"

const char *header_filter[3] = { "Server", "Accept", "Cache" };
const int header_filter_size  = sizeof(header_filter) / sizeof(header_filter[0]);

/** Search header strings for presence of header filter attributes
 *
 * @param buffer    the string in which the substring is searched
 * @return          1 if the substring is found, or 0 if the substring is not found.
 */
int compare_header_filter(char *buffer) {
    D printf("filter_func for %s", buffer);
    for(int i = 0; i < header_filter_size; i ++) {
        if(strstr(buffer, header_filter[i]) != NULL) {
            D printf("found a match for %s with %s\n", buffer, header_filter[i]);
            return i;
        }
    }

    return 0;
}

static size_t http_headers_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    if(compare_header_filter(buffer) > 0)
        D printf("%s (matching string with %s", buffer, header_filter[0]);

    return nitems * size;
}

int http_headers(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, http_headers_callback);

    curl_easy_perform(curl);

    return 0;
}