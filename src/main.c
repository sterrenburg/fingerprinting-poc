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
#include <unistd.h>

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

int process_header_function_server_version(struct Fingerprint_function fingerprint_function, int server_index, int version_index) {
    struct Signature_function signature_function;

    for(int i = 0; i < FINGERPRINT_FUNCTIONS_SIZE; i ++) {
        signature_function = signature_file.servers[server_index].versions[version_index].functions[i];

        if(strncmp(signature_function.name, fingerprint_function.signature_handle, 50) == 0) {

            if(strcasestr(output, signature_function.output) != NULL) {
                printf(">>> Verdict: %s/%s\n", signature_file.servers[server_index].name, signature_file.servers[server_index].versions[version_index].name);

                signature_file.servers[server_index].versions[version_index].frequency ++;

                return 0;
            }
        }
    }

    return 1;
}

int process_header_function_server(struct Fingerprint_function fingerprint_function, int server_index) {
    int res;
    struct Signature_server server;
    struct Signature_version version;

    server = signature_file.servers[server_index];

    for(int j = 0; j < server.size; j ++) {
        version = server.versions[j];
        D printf("] %s: trying version %s\n", server.name, version.name);

        res = process_header_function_server_version(fingerprint_function, server_index, j);
        if(res == 0) {
            break;
        }
    }

    return res;
}

int process_header_function(void *ptr, int i, Array server_list) {
    struct Fingerprint_function fingerprint_function;
    int server_index;

    /* Execute current header function */
    fingerprint_function = fingerprint_header_functions[i];
    fingerprint_function.function(ptr, output);

    /* Skip server detection if output is emtpy */
    if(output[0] == '\0') {
        printf("%s: output is empty\n", fingerprint_function.signature_handle);
        return 1;
    }

    /* Check if the response matches any server */
    server_index = is_in_server_list(output, server_list);

    // TODO debug
    const char *server_name = (server_index == -1) ? ("none") : (signature_file.servers[server_index].name);
    printf("server_index: %d (%s)\n", server_index, server_name);

    if(server_index < 0) {
        signature_file.undefined ++;

        // TODO debug
        printf("%s: server '%s' not in serverlist '", fingerprint_function.signature_handle, output);
        for(int h = 0; h < server_list.size; h ++) {
            printf("%s - ", server_list.array[h]);
        }

        printf("', increasing undefined\n");

        return 1;
    }

    signature_file.servers[server_index].frequency ++;

    return process_header_function_server(fingerprint_function, server_index);
}

size_t process_header_functions(void *ptr) {
    int res;
    /* Create a server list to compare responses to */
    Array server_list;
    array_init(&server_list, 3);
    get_server_list(&server_list);

    /* For each header received, process the fingerprint_header functions */
    for (int i = 0; i < FINGERPRINT_HEADER_FUNCTIONS_SIZE; i++) {
        res = process_header_function(ptr, i, server_list);
        if(res == 0) {
            break;
        }
    }

    array_free(&server_list);

    return res;
}

/* Process body response */
size_t response_callback(void *ptr, size_t size, size_t nmemb, struct string *s) {
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

/* Process header response */
size_t response_callback_header(void *ptr, size_t size, size_t nmemb, struct string *s) {
//    D printf("] %s: %s\n", __func__, ptr);

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

//        printf("/* For each header received, process the fingerprint_header functions */\n");
        process_header_functions(ptr);
    }

    return size*nmemb;
}

int process_header_output() {
//    D printf("] %s\n", __func__);
//    printf("] current output: '%s'\n", output);

    // TODO map to existing signatures here

    return 0;
}

CURL *fingerprint_init() {
//    D printf("] %s\n", __func__);

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

int fingerprint_start(char *hostname) {
//    D printf("] %s: %s\n", __func__, hostname);

    CURL *curl = fingerprint_init();
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

    array_free(&header_array);

    curl_easy_cleanup(curl);

    return 0;
}

int main() {
//    D printf("] %s\n", __func__);

//    char hostname[100] = "http://example.com";

//    char *hostnames[4] = { "example.com", "localhost:8080", "localhost:8081", "localhost:8082" };
    char *hostnames[4] = { "localhost:8084", "localhost:8080", "localhost:8081", "localhost:8082" };

//    CURL *curl = fingerprint_init();

    for(int i = 0; i < 4; i ++) {
        printf("Trying for %s\n", hostnames[i]);
        fingerprint_start(hostnames[i]);
//        usleep(1000000);
    }

    curl_global_cleanup();

    print_fingerprint_output();

    return 0;
}
