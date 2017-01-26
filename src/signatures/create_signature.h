/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#ifndef FINGERPRINTING_POC_CREATE_SIGNATURE_H
#define FINGERPRINTING_POC_CREATE_SIGNATURE_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define ENCODING "ISO-8859-1"

void testXmlwriterFilename(const char *uri);
void testXmlwriterMemory(const char *file);
void testXmlwriterDoc(const char *file);
void testXmlwriterTree(const char *file);
xmlChar *ConvertInput(const char *in, const char *encoding);

int create_signature(char *hostname, CURL *curl);

#endif //FINGERPRINTING_POC_CREATE_SIGNATURE_H
