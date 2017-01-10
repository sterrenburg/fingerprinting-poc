cmake_minimum_required(VERSION 3.6)
project(fingerprinting_poc)

# Hardening and warnings for building with gcc
set(GCCWARNINGS
        "-Wall"
        )

set(CMAKE_C_FLAGS "${GCCWARNINGS} ${CMAKE_C_FLAGS}")

set(FINGERPRINT_FUNCTION_SOURCES
        fingerprint_functions/http_headers.c)

set(SOURCE_FILES
    main.c
    ${FINGERPRINT_FUNCTION_SOURCES}
)

add_executable(fingerprinting_poc ${SOURCE_FILES})
target_link_libraries(fingerprinting_poc curl)