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
#include "resources/array.h"

Array header_array;
const char output[50];

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

size_t response_callback_header(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t result = size*nmemb;

    if(output[0] == '\0') {
        size_t new_len = s->len + size * nmemb;
        s->ptr = realloc(s->ptr, new_len + 1);
        if (s->ptr == NULL) {
            fprintf(stderr, "realloc() failed\n");
            exit(EXIT_FAILURE);
        }
        memcpy(s->ptr + s->len, ptr, size * nmemb);
        s->ptr[new_len] = '\0';
        s->len = new_len;

        /* For each header received, process the fingerprint_header functions */
        for (int i = 0; i < FINGERPRINT_HEADER_FUNCTIONS_SIZE; i++) {
            struct Fingerprint_function fingerprint_function = fingerprint_header_functions[i];
            fingerprint_function.function(ptr, output);

            /* Skip server detection if response is emtpy */
            if(output[0] == '\0') {
                continue;
            }

            // TODO move to 1 exec location
            /* Create a server list to compare responses to */
            Array server_list;
            get_server_list(&server_list);

            /* Check if the response matches any server */
            // TODO make loop only run on relevant server name */
            if(!is_in_server_list(output, server_list)) {
                printf("output '%s' not in serverlist '", output);
                for(int h = 0; h < server_list.size; h ++) {
                    printf("%s - ", server_list.array[h]);
                }

                printf("'\n");

                signature_file.undefined ++;

                continue;
            }

            printf("[%s::: %s\n", fingerprint_function.signature_handle, output);

            for(int i = 0; i < signature_file.size; i ++) {
                struct Signature_server server = signature_file.servers[i];
                printf("> server to try: %s\n", server.name);

                /* Skip version detection if server type cannot be determined */
                /*if(strcasestr(output, server.name) == NULL) {
                    printf(" > undefined server tag for '%s'\n", output);
                    signature_file.undefined ++;
                    continue;
                }*/

                for(int j = 0; j < server.size; j ++) {
                    struct Signature_version version = server.versions[j];
                    printf(">> version: %s\n", version.name);

                    // functions
                    for(int k = 0; k < FINGERPRINT_FUNCTIONS_SIZE; k ++) {
                        struct Signature_function signature_function = version.functions[k];
                        if(strncmp(signature_function.name, fingerprint_function.signature_handle, 50) == 0) {
                            if(strcasestr(output, signature_function.output) != NULL) {
                                printf("Verdict: %s/%s\n", server.name, version.name);

                                printf("increasing %s/%s\n", server.name, version.name);
                                signature_file.servers[i].versions[j].frequency ++;

                                return result;
                            }
                        }
                    }
                }
            }
        }
    }

    return size*nmemb;
}

int process_header_output() {
    D printf("] %s\n", __func__);
    printf("] current output: '%s'\n", output);

    // TODO map to existing signatures here

    return 0;
}

int fingerprint_start(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    CURLcode res;
    struct string response_body, response_header;

    init_string(&response_body);
    init_string(&response_header);

    array_init(&header_array, 5);

    curl_easy_setopt(curl, CURLOPT_URL, hostname);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, response_callback_header);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_header);

    // TODO get request response and send to fingerprint_execute
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }

    process_header_output();

//    printf("header: \n%s\n", response_header.ptr);
//    printf("body: \n%s\n", response_body.ptr);

    // TODO create fallback for fingerprint base on body


    /* Cleanup */
    free(response_body.ptr);
    free(response_header.ptr);

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

    print_fingerprint_output();

    return 0;
}
