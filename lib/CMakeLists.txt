# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

include(FetchContent)

if (${SE_OSS_SUPPORT_CBOR})
    message(STATUS "[se-log] Support for CBOR enabled")
    set(WITH_FLOATING_POINT ON CACHE BOOL "" FORCE)
    set(WITH_CBOR2JSON OFF CACHE BOOL "" FORCE)
    set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            tinycbor
            GIT_REPOSITORY https://github.com/intel/tinycbor.git
            GIT_TAG v7.0 # need to use main, because cmake version is no release yet
    )
    FetchContent_MakeAvailable(tinycbor)
endif ()

if (${SE_OSS_TESTS})
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif ()
