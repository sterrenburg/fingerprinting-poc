/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#ifndef FINGERPRINTING_POC_CREATE_SIGNATURE_H
#define FINGERPRINTING_POC_CREATE_SIGNATURE_H

#include <stdbool.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "../resources/array.h"

#define ENCODING "ISO-8859-1"

struct Signature_function {
    const char *name;
    const char *output;
};

struct Signature_version {
    const char *name;
    struct Signature_function *functions;
    int frequency;
};

struct Signature_server {
    const char *name;
    struct Signature_version *versions;
    const int size;
    int frequency;
};

struct Signature_file {
    const char *name;
    struct Signature_server *servers;
    const int size;
    int undefined;
};

int get_server_list(Array *array);
bool is_in_server_list(const char *needle, Array haystack);

struct Signature_file signature_file;

void testXmlwriterFilename(const char *uri);
void testXmlwriterMemory(const char *file);
void testXmlwriterDoc(const char *file);
void testXmlwriterTree(const char *file);
xmlChar *ConvertInput(const char *in, const char *encoding);

int signatures(char *hostname, CURL *curl);
int signatures_init(const char *uri);
int signatures_parse(const char *uri);
int signatures_update(const char *uri, const char *signature_handle, const char *output);

#endif //FINGERPRINTING_POC_CREATE_SIGNATURE_H
