/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <curl/curl.h>

#include "fingerprint_functions/fingerprint_functions.h"
#include "signatures/signatures.h"
#include "debug.h"

int fingerprint_execute(char *hostname, CURL *curl, const char *output, int i) {
    fingerprint_functions[i].function(hostname, curl, output);
    D printf("] %s: %s > '%s'\n", __func__, fingerprint_functions[i].signature_handle, output);
//    signatures_update("test.tmp", fingerprint_functions[i].signature_handle, output);
    return 0;
}

int fingerprint_start(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    curl_easy_setopt(curl, CURLOPT_URL, hostname);

    /* if hostname is redirected, tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    for(int i = 0; i < FINGERPRINT_FUNCTIONS_SIZE; i ++) {
        const char output[50];
        fingerprint_execute(hostname, curl, output, i);
    }


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

    char hostname[100] = "http://example.com";

    CURL *curl = fingerprint_init();

    fingerprint_start(hostname, curl);

//    signatures(hostname, curl);

    signatures_init("test.tmp");

    return 0;
}
