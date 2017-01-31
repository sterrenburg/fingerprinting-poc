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

#include "fingerprint_functions/fingerprint_functions.h"
#include "signatures/signatures.h"
#include "debug.h"

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t response_callback(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

int fingerprint_execute(char *hostname, CURL *curl, const char *output, int i) {
    fingerprint_functions[i].function(hostname, curl, output);
    D printf("] %s: %s > '%s'\n", __func__, fingerprint_functions[i].signature_handle, output);
    return 0;
}

int fingerprint_start(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    CURLcode res;
    struct string response_body, response_header;
    init_string(&response_body);
    init_string(&response_header);

    curl_easy_setopt(curl, CURLOPT_URL, hostname);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_header);

    // TODO get request response and send to fingerprint_execute
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }

    printf("body: %s\n", response_body.ptr);
    printf("header: %s\n", response_header.ptr);

    for(int i = 0; i < FINGERPRINT_FUNCTIONS_SIZE; i ++) {
        const char output[50];
        fingerprint_execute(hostname, curl, output, i);
    }

    free(response_body.ptr);
    free(response_header.ptr);



    /* Cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

CURL *fingerprint_init() {
    D printf("] %s\n", __func__);

    CURL *curl;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(!curl)
        printf("Failed to initialize CURL");

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "zmap-fingerprint-agent/1.0");

    return curl;
}

int main() {
    D printf("] %s\n", __func__);

    char hostname[100] = "http://localhost:8081/random";

    CURL *curl = fingerprint_init();

    fingerprint_start(hostname, curl);

//    signatures(hostname, curl);

    signatures_init("test.tmp");

    return 0;
}
