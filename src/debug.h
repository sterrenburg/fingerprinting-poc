/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#ifndef FINGERPRINTING_POC_DEBUG_H
#define FINGERPRINTING_POC_DEBUG_H

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define SUCCESS GREEN
#define WARNING MAGENTA
#define ERROR   RED
#define INFO    CYAN

#define DEBUG

#ifdef DEBUG
    #define D
#else
    #define D for(;0;)
#endif

#endif //FINGERPRINTING_POC_DEBUG_H
