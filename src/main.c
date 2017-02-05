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

#define STATUS_SUCCESS 0
#define STATUS_FAILURE 1
#define STATUS_ERROR   2

Array header_array;
const char output[50];
char *hostname;

int status;

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
                D printf(SUCCESS "%s:" RESET " matched with %s/%s\n\n", hostname, signature_file.servers[server_index].name, signature_file.servers[server_index].versions[version_index].name);

                signature_file.servers[server_index].versions[version_index].frequency ++;
//                status = STATUS_SUCCESS;

                return 0;
            }
        }
    }

    return 1;
}

int process_header_function_server(struct Fingerprint_function fingerprint_function, int server_index) {
    int res, i;
    struct Signature_server server;
    struct Signature_version version;

    server = signature_file.servers[server_index];
    res = -1;

    for(i = 0; i < server.size; i ++) {
        version = server.versions[i];
        D printf(INFO "%s: " RESET "trying to match with %s/%s\n", hostname, server.name, version.name);

        res = process_header_function_server_version(fingerprint_function, server_index, i);
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
//        D printf("%s: output is empty\n", fingerprint_function.signature_handle);
        return 1;
    }

    /* Check if the response matches any server */
    server_index = is_in_server_list(output, server_list);

    // TODO debug
    const char *server_name = (server_index == -1) ? ("none") : (signature_file.servers[server_index].name);
//    D printf("server_index: %d (%s)\n", server_index, server_name);

    if(server_index < 0) {
        // TODO debug
        /*printf("%s: server '%s' not in serverlist '", fingerprint_function.signature_handle, output);
        for(int h = 0; h < server_list.size; h ++) {
            printf("%s - ", server_list.array[h]);
        }

        printf("'\n");*/

        return 1;
    }

    signature_file.servers[server_index].frequency ++;
    status = STATUS_SUCCESS;

    return process_header_function_server(fingerprint_function, server_index);
}

size_t process_header_functions(void *ptr) {
//    D printf(">> %s: %s\n", __func__, ptr);
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
size_t response_callback_body(void *ptr, size_t size, size_t nmemb, struct string *s) {
    D printf("> %s: %s\n", __func__, ptr);
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
//    D printf("> %s: %s\n", __func__, ptr);

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

//    printf("%s: returning %d\n",  __func__, size*nmemb);
    return size*nmemb;
}

int fingerprint_start() {
    D printf("] %s: %s\n", __func__, hostname);

    CURL *curl;
    CURLcode res;
    struct string response_body, response_header;

    init_string(&response_body);
    init_string(&response_header);

    array_init(&header_array, 5);

    curl_easy_setopt(curl, CURLOPT_URL, hostname);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, response_callback_header);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_header);

    // TODO get request response and send to fingerprint_execute
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
//        D printf(ERROR "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }

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

int curl_start() {
    D printf(INFO "%s: " RESET "start\n", hostname);
    CURL *curl;
    CURLcode res;

    /* Reset global output and success variables */
    strncpy(output, "\0", 1);
    status = STATUS_FAILURE;

    struct string response_body, response_header;

    curl = curl_easy_init();

    init_string(&response_body);
    init_string(&response_header);
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "zmap-fingerprinting/1.0");
        curl_easy_setopt(curl, CURLOPT_URL, hostname);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, response_callback_header);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_header);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            printf(ERROR "%s: " RESET "%s\n", hostname, curl_easy_strerror(res));
            status = STATUS_ERROR;
        }

        if(status == STATUS_FAILURE) {
            D printf(WARNING "%s:" RESET " no match with header response, trying body response\n", hostname);

            // TODO implement body response handling
            curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback_body);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

            /*res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                D printf(ERROR "curl_easy_perform() failed: %s\n" RESET, curl_easy_strerror(res));
            }*/
        }


        if(status != STATUS_SUCCESS) {
            D printf(ERROR "%s:" RESET " no signature found\n\n", hostname);
            signature_file.undefined ++;
        }


//        printf("header: \n%s\n", response_header.ptr);
//        printf("body: \n%s\n", response_body.ptr);

        /* always cleanup */
        curl_easy_cleanup(curl);

        free(response_body.ptr);
        free(response_header.ptr);
    }

    return 0;
}

int main() {
//    D printf("] %s\n", __func__);

//    char *hostnames[4] = { "example.com", "localhost:8080", "localhost:8081", "localhost:8082" };
    char *hostnames[5] = { "localhost:8080", "localhost:8081", "localhost:8082", "localhost:8083", "91.184.13.76" };

    curl_global_init(CURL_GLOBAL_ALL);

    for(int i = 0; i < 5; i ++) {
        hostname = hostnames[i];
        curl_start(hostname);
    }

    curl_global_cleanup();

    fingerprint_output();

    return 0;
}
