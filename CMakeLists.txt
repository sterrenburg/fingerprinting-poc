cmake_minimum_required(VERSION 3.6)
project(fingerprinting_poc)

# Setup

set(GCCWARNINGS
        "-Wall"
        )

set(CMAKE_C_FLAGS "${GCCWARNINGS} ${CMAKE_C_FLAGS}")

# Sources

set(FINGERPRINT_FUNCTION_SOURCES
        src/fingerprint_functions/presence_in_reply.c
        src/fingerprint_functions/presence_in_reply.h
        src/fingerprint_functions/http_header_ordering.c
        src/fingerprint_functions/http_header_ordering.h
        src/fingerprint_functions/banner_report.c
        src/fingerprint_functions/banner_report.h
        src/fingerprint_functions/fingerprint_functions.c
        src/fingerprint_functions/fingerprint_functions.h)

set(RESOURCE_SOURCES
        src/resources/http_headers.c
        src/resources/http_headers.h)

set(SIGNATURES_SOURCES
        src/signatures/create_signature.c
        src/signatures/create_signature.h)

set(SOURCE_FILES
        src/main.c
        src/debug.h
        ${RESOURCE_SOURCES}
        ${SIGNATURES_SOURCES}
        ${FINGERPRINT_FUNCTION_SOURCES})

add_executable(fingerprinting_poc ${SOURCE_FILES})

# Libraries

find_package (LibXml2)
if(LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
else()
    MESSAGE("FATAL_ERROR could not find LIBXML2")
endif()

target_link_libraries(fingerprinting_poc curl)
target_link_libraries (fingerprinting_poc ${LIBXML2_LIBRARIES})