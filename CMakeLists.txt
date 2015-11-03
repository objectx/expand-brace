#
# Copyright (c) 2015 Masashi Fujita
#

cmake_minimum_required (VERSION 3.3)

project (expand-brace
        LANGUAGES CXX C
        VERSION 1.0.0.0)

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

function (setvar_default var_)
    if (NOT DEFINED ${var_})
        set (${var_} ${ARGN} PARENT_SCOPE)
    endif ()
endfunction ()

setvar_default (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
setvar_default (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
setvar_default (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

enable_testing ()

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (SOURCE_FILES src/expand-brace.cpp)
set (HEADER_FILES include/expand-brace.hpp)

add_library (expand-brace STATIC ${SOURCE_FILES} ${HEADER_FILES})
    target_compile_features (expand-brace PUBLIC cxx_auto_type cxx_generic_lambdas)
    target_include_directories (expand-brace PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory (test)
