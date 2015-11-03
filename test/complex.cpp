/* -*- mode: c++; coding: utf-8 -*- */
/*
 * complex.cpp:
 *
 * Copyright (c) 2015 Masashi Fujita
 */
#include "catch.hpp"
#include <iterator>
#include <set>
#include <expand-brace.hpp>
#include <iostream>

TEST_CASE ("Complex expansion") {
    SECTION ("Expand \"~/{Downloads,Pictures}/*.{jpg,gif,png}\"") {
        auto && result = expand_brace ("~/{Downloads,Pictures}/*.{jpg,gif,png}") ;
        if (false) {
            for (auto const & s : result) {
                std::cerr << "> " << s << std::endl;
            }
        }
        REQUIRE (result.size () == 6) ;
        REQUIRE (result[0] == "~/Downloads/*.jpg");
        REQUIRE (result[1] == "~/Downloads/*.gif");
        REQUIRE (result[2] == "~/Downloads/*.png");
        REQUIRE (result[3] == "~/Pictures/*.jpg");
        REQUIRE (result[4] == "~/Pictures/*.gif");
        REQUIRE (result[5] == "~/Pictures/*.png");
    }
    SECTION ("Expand \"It{{em,alic}iz,erat}e{d,}, please.\"") {
        auto && result = expand_brace ("It{{em,alic}iz,erat}e{d,}") ;
        if (true) {
            for (auto const & s : result) {
                std::cerr << "> " << s << std::endl;
            }
        }
        REQUIRE (result.size () == 6) ;
        REQUIRE (result [0] == "Itemized, please.") ;
        REQUIRE (result [1] == "Itemize, please.") ;
        REQUIRE (result [2] == "Italicized, please.") ;
        REQUIRE (result [3] == "Italicize, please.") ;
        REQUIRE (result [4] == "Iterated, please.") ;
        REQUIRE (result [5] == "Iterate, please.") ;
    }
}
