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
#include <stdlib.h>
#include <stdbool.h>

#include "banner_report.h"
#include "../debug.h"

const char *server_header = "Server";

char *trim_whitespace(const char *str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    *(end+1) = 0;

    return str;
}

/** Search header strings for presence of Server attribute
 *
 * @param ptr       the pointer of the obtained header
 * @param result    the pointer in which to store the result
 * @return          1 if the substring is found, 0 otherwise
 */
int find_server_banner(void *ptr) {
        if (strstr(ptr, "Server") != NULL) {
            return true;
        }

    return false;
}

int banner_report(void *ptr, const char *output) {
    D printf("] %s\n", __func__);

    int cutoff = strlen(server_header) + 2;

    if(find_server_banner(ptr)) {
        ptr = trim_whitespace(ptr);

        strncpy(output, ptr + cutoff, 50);

        output = output + cutoff;

    }

    return 0;
}