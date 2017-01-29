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

int getTime(char *output) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );

    sprintf(output, "%d-%d-%d %d:%d:%d",timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return 0;
}

/**
 * ConvertInput:
 * @in: string in a given encoding
 * @encoding: the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar *
ConvertInput(const char *in, const char *encoding)
{
    xmlChar *out;
    int ret;
    int size;
    int out_size;
    int temp;
    xmlCharEncodingHandlerPtr handler;

    if (in == 0)
        return 0;

    handler = xmlFindCharEncodingHandler(encoding);

    if (!handler) {
        printf("ConvertInput: no encoding handler found for '%s'\n",
               encoding ? encoding : "");
        return 0;
    }

    size = (int) strlen(in) + 1;
    out_size = size * 2 - 1;
    out = (unsigned char *) xmlMalloc((size_t) out_size);

    if (out != 0) {
        temp = size - 1;
        ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
        if ((ret < 0) || (temp - size + 1)) {
            if (ret < 0) {
                printf("ConvertInput: conversion wasn't successful.\n");
            } else {
                printf
                        ("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
                         temp);
            }

            xmlFree(out);
            out = 0;
        } else {
            out = (unsigned char *) xmlRealloc(out, out_size + 1);
            out[out_size] = 0;  /*null terminating out */
        }
    } else {
        printf("ConvertInput: no mem\n");
    }

    return out;
}

// NEW
/**
 * example4:
 * @filename:		the input XML filename.
 * @xpathExpr:		the xpath expression for evaluation.
 * @value:		the new node content.
 *
 * Parses input XML file, evaluates XPath expression and update the nodes
 * then print the result.
 *
 * Returns 0 on success and a negative value otherwise.
 */
int
example4(const char* filename, const xmlChar* xpathExpr, const xmlChar* value) {
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;

    /* Load XML document */
    doc = xmlParseFile(filename);
    if (doc == NULL) {
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", filename);
        return(-1);
    }

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc);
        return(-1);
    }

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return(-1);
    }

    /* update selected nodes */
    update_xpath_nodes(xpathObj->nodesetval, value);


    /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    /* dump the resulting document */
    xmlDocDump(stdout, doc);


    /* free the document */
    xmlFreeDoc(doc);

    return(0);
}

/**
 * update_xpath_nodes:
 * @nodes:		the nodes set.
 * @value:		the new value for the node(s)
 *
 * Prints the @nodes content to @output.
 */
void
update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar* value) {
    int size;
    int i;

    size = (nodes) ? nodes->nodeNr : 0;

    /*
     * NOTE: the nodes are processed in reverse order, i.e. reverse document
     *       order because xmlNodeSetContent can actually free up descendant
     *       of the node and such nodes may have been selected too ! Handling
     *       in reverse order ensure that descendant are accessed first, before
     *       they get removed. Mixing XPath and modifications on a tree must be
     *       done carefully !
     */
    for(i = size - 1; i >= 0; i--) {

        xmlNodeSetContent(nodes->nodeTab[i], value);
        /*
         * All the elements returned by an XPath query are pointers to
         * elements from the tree *except* namespace nodes where the XPath
         * semantic is different from the implementation in libxml2 tree.
         * As a result when a returned node set is freed when
         * xmlXPathFreeObject() is called, that routine must check the
         * element type. But node from the returned set may have been removed
         * by xmlNodeSetContent() resulting in access to freed data.
         * This can be exercised by running
         *       valgrind xpath2 test3.xml '//discarded' discarded
         * There is 2 ways around it:
         *   - make a copy of the pointers to the nodes from the result set
         *     then call xmlXPathFreeObject() and then modify the nodes
         * or
         *   - remove the reference to the modified nodes from the node set
         *     as they are processed, if they are not namespace nodes.
         */
        if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            nodes->nodeTab[i] = NULL;
    }
}
// \NEW

int signatures_update(const char *uri, const char *signature_handle, const char *output) {
    example4("test.tmp", "//BANNER_DEDUCED", "NEWSTUFF");

    return 0;
}

int signatures_init(const char *uri) {
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    char timeString[100];
    getTime(timeString);

    writer = xmlNewTextWriterFilename(uri, 0);
    if(writer == NULL) {
        fprintf(stderr, "textXmlWriterFilename: Error creating xml writer\n");
        return -1;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, ENCODING, NULL);
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterStartDocument\n");
        return -1;
    }

    /* Start an element named "SIGNATURES". Since this is the first
     * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "SIGNATURES");
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterStartElement\n");
        return -1;
    }

    /* Write an element named "DATE" as child of SIGNATURE. */
    tmp = ConvertInput(timeString, ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "DATE", tmp);
    if (rc < 0) {
        printf
                ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return -1;
    }
    if (tmp != NULL) xmlFree(tmp);

    rc = xmlTextWriterEndDocument(writer);
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterEndDocument\n");
        return -1;
    }

    return 0;
}

int writeSignature(const char *uri) {
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    char timeString[100];
    getTime(timeString);

    writer = xmlNewTextWriterFilename(uri, 0);
    if(writer == NULL) {
        fprintf(stderr, "textXmlWriterFilename: Error creating xml writer\n");
        return -1;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, ENCODING, NULL);
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterStartDocument\n");
        return -1;
    }

    /* Start an element named "SIGNATURES". Since this is the first
     * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "SIGNATURES");
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterStartElement\n");
        return -1;
    }

    /* Start an element named "SIGNATURE" as child of SIGNATURES. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "SIGNATURE");
    if (rc < 0) {
        printf
                ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return -1;
    }

    /* Write an element named "BANNER_REPORTED" as child of SIGNATURE. */
    tmp = ConvertInput("nginx/1.10.0 (Ubuntu)", ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "BANNER_REPORTED", tmp);
    if (rc < 0) {
        printf
                ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return -1;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* Write an element named "BANNER_DEDUCED" as child of SIGNATURE. */
    tmp = ConvertInput("nginx/1.10.0", ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "BANNER_DEDUCED", tmp);
    if (rc < 0) {
        printf
                ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return -1;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* Write an element named "DATE" as child of SIGNATURE. */
    tmp = ConvertInput(timeString, ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "DATE", tmp);
    if (rc < 0) {
        printf
                ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return -1;
    }
    if (tmp != NULL) xmlFree(tmp);

    rc = xmlTextWriterEndDocument(writer);
    if(rc < 0) {
        fprintf(stderr, "textXmlWriterFilename: Error at xmlTextWriterEndDocument\n");
        return -1;
    }

    return 0;
}

int signatures(char *hostname, CURL *curl) {
    D printf("] %s\n", __func__);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /* first, the file version */
//    testXmlwriterFilename("writer1.tmp");

    /* next, the memory version */
//    testXmlwriterMemory("writer2.tmp");

    /* next, the DOM version */
//    testXmlwriterDoc("writer3.tmp");

    /* next, the tree version */
//    testXmlwriterTree("writer4.tmp");

    writeSignature("test.tmp");

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    return 0;
}