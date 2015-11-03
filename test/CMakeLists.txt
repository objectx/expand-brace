#
# Copyright (c) YEAR Masashi Fujita
#

cmake_minimum_required (VERSION 3.3)

set (TEST_SOURCE_FILES main.cpp simple.cpp complex.cpp)
set (TEST_HEADER_FILES catch.hpp)

add_executable (test-expand-brace ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})
    target_compile_features (test-expand-brace PRIVATE cxx_auto_type cxx_generic_lambdas)
    target_link_libraries (test-expand-brace PRIVATE expand-brace)

add_test (NAME test-expand-brace COMMAND test-expand-brace -r compact)
