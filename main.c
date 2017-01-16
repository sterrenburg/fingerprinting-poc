#include <stdio.h>
#include <curl/curl.h>

#include "fingerprint_functions/presence_in_file.h"
#include "fingerprint_functions/http_headers.h"

int fun1(char *hostname, CURL *curl) {
    printf("%s\n", __func__);
    return 11;
}

int fun2(char *hostname, CURL *curl) {
    printf("%s\n", __func__);
    return 22;
}

int fun3(char *hostname, CURL *curl) {
    printf("%s\n", __func__);
    return 33;
}

#define FINGERPRINT_FUNCTIONS 5

int (*fingerprint_functions[FINGERPRINT_FUNCTIONS])(char *hostname, CURL *curl) = {fun1, fun2, fun3, http_headers, presence_in_file};

CURL *fingerprint_init() {
    printf("%s\n", __func__);

    CURL *curl;

    curl = curl_easy_init();
    if(!curl)
        printf("Failed to initialize CURL");

    return curl;
}

int fingerprint_start(char *hostname, CURL *curl) {
    printf("%s\n", __func__);

    CURLcode res;

    curl_easy_setopt(curl, CURLOPT_URL, hostname);

    /* if hostname is redirected, tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    for(int i = 0; i < FINGERPRINT_FUNCTIONS; i ++) {
        printf("function %d(%s): %d\n", i, hostname, fingerprint_functions[i](hostname, curl));
    }

    /* Perform the request, res will get the return code */
//    res = curl_easy_perform(curl);

    /* Check for errors */
//    if(res != CURLE_OK)
//        fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                curl_easy_strerror(res));

    /* Cleanup */
    curl_easy_cleanup(curl);
}

int main() {
    printf("%s\n", __func__);

    char *hostname = "http://example.com";

    CURL *curl = fingerprint_init();

    fingerprint_start(hostname, curl);
    return 0;
}

/*
int main(void)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.com");
        */
/* example.com is redirected, so we tell libcurl to follow redirection *//*

//        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        */
/* Perform the request, res will get the return code *//*

        res = curl_easy_perform(curl);
        */
/* Check for errors *//*

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        */
/* always cleanup *//*

        curl_easy_cleanup(curl);
    }
    return 0;
}*/
