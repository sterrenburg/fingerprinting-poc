/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */
         
#ifndef FINGERPRINTING_POC_HTTP_HEADER_ORDERING_H
#define FINGERPRINTING_POC_HTTP_HEADER_ORDERING_H

int http_header_ordering(char *hostname, CURL *curl);

#endif //FINGERPRINTING_POC_HTTP_HEADER_ORDERING_H
