/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */
         
#ifndef HTTP_HEADERS_H   /* Include guard */
#define HTTP_HEADERS_H

int http_headers(char *hostname, CURL *curl);

#endif // HTTP_HEADERS_H