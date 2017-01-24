cmake_minimum_required(VERSION 3.)
project(fingerprinting_poc)

# Hardening and warnings for building with gcc
set(GCCWARNINGS
        "-Wall"
        )

set(CMAKE_C_FLAGS "${GCCWARNINGS} ${CMAKE_C_FLAGS}")

# Add fingerprint functions here
set(FINGERPRINT_FUNCTION_SOURCES
        fingerprint_functions/http_headers.c
        fingerprint_functions/http_headers.h
        fingerprint_functions/presence_in_reply.c
        fingerprint_functions/presence_in_reply.h)

set(SOURCE_FILES
    main.c
    debug.h
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

MESSAGE("done")