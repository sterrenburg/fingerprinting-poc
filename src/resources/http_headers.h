/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */
         
#ifndef FINGERPRINTING_POC_HTTP_HEADERS_H
#define FINGERPRINTING_POC_HTTP_HEADERS_H

const char *http_headers(char *hostname, CURL *curl, const char **headers, const int headers_size);

#endif // FINGERPRINTING_POC_HTTP_HEADERS_H