/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "signatures.h"
#include "../debug.h"
#include "../resources/array.h"

#define SIZE(x)  (sizeof(x) / sizeof((x)[0]))

/* Nginx */
struct Signature_function signature_functions_nginx_generic[] = {
        { .name = "BANNER_REPORTED", .output = "nginx" }
};

struct Signature_function signature_functions_nginx_1_10_2[] = {
        { .name = "BANNER_REPORTED", .output = "nginx/1.10.2" }
};

struct Signature_function signature_functions_nginx_1_11_9[] = {
        { .name = "BANNER_REPORTED", .output = "nginx/1.11.9" }
};

struct Signature_version signature_versions_nginx[] = {
        { .name= "1.10.2", .functions = signature_functions_nginx_1_10_2, .size = SIZE(signature_functions_nginx_1_10_2) , .frequency = 0 },
        { .name= "1.11.9", .functions = signature_functions_nginx_1_11_9, .size = SIZE(signature_functions_nginx_1_11_9) , .frequency = 0 },
        { .name= "generic", .functions = signature_functions_nginx_generic, .size = SIZE(signature_functions_nginx_generic) , .frequency = 0 },
};

/* Apache */
struct Signature_function signature_functions_apache_generic[] = {
        { .name = "BANNER_REPORTED", .output = "Apache" }
};

struct Signature_function signature_functions_apache_2_2_32[] = {
        { .name = "BANNER_REPORTED", .output = "Apache/2.2.32" }
};

struct Signature_function signature_functions_apache_2_4_25[] = {
        { .name = "BANNER_REPORTED", .output = "Apache/2.4.25" }
};

struct Signature_version signature_versions_apache[] = {
        { .name = "2.2.32", .functions = signature_functions_apache_2_2_32, .size = SIZE(signature_functions_apache_2_2_32) , .frequency = 0 },
        { .name= "2.4.25", .functions = signature_functions_apache_2_4_25, .size = SIZE(signature_functions_apache_2_4_25), .frequency = 0 },
        { .name = "generic", .functions = signature_functions_apache_generic, .size = SIZE(signature_functions_apache_generic), .frequency = 0 },
};

/* Lighttpd */
struct Signature_function signature_functions_lighttpd_generic[] = {
        { .name = "BANNER_REPORTED", .output = "lighttpd" }
};

struct Signature_function signature_functions_lighttpd_1_4_39[] = {
        { .name = "BANNER_REPORTED", .output = "lighttpd/1.4.39" }
};

struct Signature_function signature_functions_lighttpd_1_1_1[] = {
        { .name = "BANNER_REPORTED", .output = "lighttpd/1.1.1" }
};

struct Signature_version signature_versions_lighttpd[] = {
        { .name = "1.4.39", .functions = signature_functions_lighttpd_1_4_39, .size = SIZE(signature_functions_lighttpd_1_4_39), .frequency = 0 },
        { .name = "1.1.1", .functions = signature_functions_lighttpd_1_1_1, .size = SIZE(signature_functions_lighttpd_1_1_1) , .frequency = 0 },
        { .name = "generic", .functions = signature_functions_lighttpd_generic, .size = SIZE(signature_functions_lighttpd_generic) , .frequency = 0 },
};

/* Servers, size defines signature_file.size */
struct Signature_server signature_servers[] = {
        { .name = "Nginx", .versions = signature_versions_nginx, .size = SIZE(signature_versions_nginx), .frequency = 0 },
        { .name = "Apache", .versions = signature_versions_apache, .size = SIZE(signature_versions_apache), .frequency = 0 },
        { .name = "Lighttpd", .versions = signature_versions_lighttpd, .size = SIZE(signature_versions_lighttpd), .frequency = 0 }
};

struct Signature_file signature_file = {
    .name = "Signatures",
    .servers = signature_servers,
    .size = SIZE(signature_servers),
    .undefined = 0
};

void get_server_list(Array *array) {
    for(int i = 0; i < signature_file.size; i ++) {
        array_insert(array, signature_file.servers[i].name);
    }
}

int is_in_server_list(const char *needle, Array haystack) {
    int i;
    for (i = 0; i < haystack.size; i++) {
        if (strcasestr(needle, haystack.array[i])) {
            return i;
        }
    }

    return -1;
}

void fingerprint_output() {
    D printf(INFO "%s\n" RESET, __func__);
    for(int i = 0; i < signature_file.size; i ++) {
        printf("-------------------\n");
        struct Signature_server server = signature_file.servers[i];
        printf("%d\t%s (total)\n", server.frequency, server.name);

        for(int j = 0; j < server.size; j ++) {
            struct Signature_version version = server.versions[j];
            printf("%d\t- %s\n", version.frequency, version.name);
        }
    }

    printf("-------------------\n");
    printf("%d\tundefined\n", signature_file.undefined);
}