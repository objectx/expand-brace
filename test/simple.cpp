/* -*- mode: c++; coding: utf-8 -*- */
/*
 * simple.cpp:
 *
 * Copyright (c) YEAR Masashi Fujita
 */
#include "catch.hpp"
#include <iterator>
#include <expand-brace.hpp>
#include <iostream>

using namespace std ;

TEST_CASE ("simple expansion", "[expand-brace]") {
    SECTION ("Empty string") {
        auto && result = expand_brace ("");
        REQUIRE (result.size () == 0) ;
    }
    SECTION ("\"abc\"") {
        auto && result = expand_brace ("abc") ;
        REQUIRE (result.size () == 1) ;
        REQUIRE (result [0] == "abc") ;
    }
}
