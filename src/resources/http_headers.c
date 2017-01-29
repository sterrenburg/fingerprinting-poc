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

/* Note: global variable initialization needed since header
     * callback does not accept new parameters */
char **http_headers_filter;
int http_headers_filter_size;

char result[50];

/** Search header strings for presence of header filter attributes
 *
 * @param buffer    the string in which the substring is searched
 * @return          1 if the substring is found, 0 otherwise
 */
int compare_header_filter(char *buffer) {
    if(result[0] == '\0') {
        for (int i = 0; i < http_headers_filter_size; i++) {
            if (strstr(buffer, http_headers_filter[i]) != NULL) {
                strncpy(result, buffer, 50);
                return 1;
            }
        }
    }

    return 0;
}

static size_t http_headers_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    compare_header_filter(buffer);

    return nitems * size;
}

const char *http_headers(char *hostname, CURL *curl, const char **headers, const int headers_size) {
    D printf("] %s\n", __func__);

    http_headers_filter = headers;
    http_headers_filter_size = headers_size;

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, http_headers_callback);

    curl_easy_perform(curl);

    return result;
}