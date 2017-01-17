#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "presence_in_reply.h"
#include "../debug.h"

const char *presence_filter[3] = { "Server", "Accept", "example" };
const int presence_filter_size  = sizeof(presence_filter) / sizeof(presence_filter[0]);

char *uri = "/admin.php";

struct memory_struct {
    char *memory;
    size_t size;
};

/** Check for the presence of one of the predefined filters in the reply body
 *
 * @param buffer    the reply body
 * @param url       the URL that the body originates from
 * @return          the index of the first filter that is encountered, 0 otherwise
 */
int check_presence(char *buffer, char *url) {
    for(int i = 0; i < presence_filter_size; i ++) {
        if(strstr(buffer, presence_filter[i]) != NULL) {
            D printf("found a presence for '%s' in URL %s\n", presence_filter[i], url);
            return i;
        }
    }

    return 0;
}

static size_t write_memory_callback(void *buffer, size_t size, size_t nitems, void *userdata) {
    size_t realsize = size * nitems;
    struct memory_struct *mem = (struct memory_struct *)userdata;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int *create_url(char *hostname, char *uri, char *url) {
    char dest[50];

    strncpy(dest, hostname, 50);
    strncat(dest, uri, 50);

    strncpy(url, dest, 100);

    return 0;
}

int presence_in_reply(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    D printf("example debug\n");

    CURLcode res;
    struct memory_struct chunk;
    char url[100];

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    create_url(hostname, uri, url);

    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    /* get it! */
    res = curl_easy_perform(curl);

    /* check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }
    else {
        D printf("%lu bytes retrieved\n", (long)chunk.size);

        return check_presence(chunk.memory, url);
    }

    free(chunk.memory);
}