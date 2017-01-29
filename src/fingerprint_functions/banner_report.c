/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include <curl/curl.h>
#include <ctype.h>
#include <string.h>

#include "banner_report.h"
#include "../resources/http_headers.h"
#include "../debug.h"

const char *headers[1] = {"Server"};
const int headers_size = 1;

char *trimwhitespace(char *str)
{
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    *(end+1) = 0;

    return str;
}

int banner_report(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    int cutoff = strlen(headers[0]) + 2;

    char *banner = http_headers(hostname, curl, headers, headers_size);

    banner = trimwhitespace(banner);

    banner = &banner[cutoff];

    printf("> Banner reported: %s\n", banner);

    return 0;
}