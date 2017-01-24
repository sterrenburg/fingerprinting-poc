/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */
         
#include <curl/curl.h>
#include "http_header_ordering.h"
#include "../debug.h"

int http_header_ordering(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);
    return 5;
}