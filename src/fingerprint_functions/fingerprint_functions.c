/*
 * Copyright 2017 Thomas Sterrenburg
 *
 * Licensed under the MIT License (the License); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at https://opensource.org/licenses/MIT
 */

#include <curl/curl.h>

#include "fingerprint_functions.h"
#include "banner_report.h"

struct Fingerprint_function fingerprint_header_functions[] = {
        { .signature_handle = "BANNER_REPORTED", .function = &banner_report }
};