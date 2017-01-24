#include <curl/curl.h>
#include "http_header_ordering.h"
#include "../debug.h"

int http_header_ordering(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);
    return 5;
}