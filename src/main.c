#include <stdio.h>
#include <curl/curl.h>

#include "fingerprint_functions/presence_in_reply.h"
#include "fingerprint_functions/http_headers.h"
#include "fingerprint_functions/http_header_ordering.h"
#include "debug.h"

int fun1(char *hostname, CURL *curl) {
    printf("] %s\n", __func__);
    return 11;
}

int fun2(char *hostname, CURL *curl) {
    printf("] %s\n", __func__);
    return 22;
}

int fun3(char *hostname, CURL *curl) {
    printf("] %s\n", __func__);
    return 33;
}

#define FINGERPRINT_FUNCTIONS 3

int (*fingerprint_functions[FINGERPRINT_FUNCTIONS])(char *hostname, CURL *curl) = {http_headers, presence_in_reply, http_header_ordering};

CURL *fingerprint_init() {
    D printf("] %s\n", __func__);

    CURL *curl;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(!curl)
        printf("Failed to initialize CURL");

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-fingerprint-agent/1.0");

    return curl;
}

int fingerprint_start(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    CURLcode res;

    curl_easy_setopt(curl, CURLOPT_URL, hostname);

    /* if hostname is redirected, tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /*for(int i = 0; i < FINGERPRINT_FUNCTIONS; i ++) {
        printf("function %d(%s): %d\n", i, hostname, fingerprint_functions[i](hostname, curl));
    }*/

    fingerprint_functions[2](hostname, curl);

    /* Cleanup */
    curl_easy_cleanup(curl);

    curl_global_cleanup();
}

int main() {
    D printf("] %s\n", __func__);

    char hostname[100] = "http://example.com";

    CURL *curl = fingerprint_init();

    fingerprint_start(hostname, curl);
    return 0;
}
