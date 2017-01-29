/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <curl/curl.h>

#include "fingerprint_functions/banner_report.h"
#include "signatures/create_signature.h"
#include "debug.h"

#define FINGERPRINT_FUNCTIONS 1

int (*fingerprint_functions[FINGERPRINT_FUNCTIONS])(char *hostname, CURL *curl) = {banner_report};
//int (*fingerprint_functions[FINGERPRINT_FUNCTIONS])(char *hostname, CURL *curl) = {http_headers, presence_in_reply, http_header_ordering};

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

int fingerprint_start(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    curl_easy_setopt(curl, CURLOPT_URL, hostname);

    /* if hostname is redirected, tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    for(int i = 0; i < FINGERPRINT_FUNCTIONS; i ++) {
        fingerprint_functions[i](hostname, curl);
    }


    /* Cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

int main() {
    D printf("] %s\n", __func__);

    char hostname[100] = "http://raspbian";

    CURL *curl = fingerprint_init();

    fingerprint_start(hostname, curl);

    create_signature(hostname, curl);
    return 0;
}
