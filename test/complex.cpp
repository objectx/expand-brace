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

TEST_CASE ("Complex expansion", "[complex]") {
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
    SECTION ("Expand \"~/{Downloads,Pictures/*.{jpg,gif,png}\"") {
        REQUIRE_THROWS_AS (expand_brace ("~/{Downloads,Pictures/*.{jpg,gif,png}"), std::runtime_error) ;
    }
    SECTION ("Expand \"It{{em,alic}iz,erat}e{d,}, please.\"") {
        auto && result = expand_brace ("It{{em,alic}iz,erat}e{d,}, please.") ;
        if (false) {
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
    SECTION ("Expand \"{,{,gotta have{ ,\\, again\\, }}more }cowbell!\"") {
        auto && result = expand_brace ("{,{,gotta have{ ,\\, again\\, }}more }cowbell!") ;
        REQUIRE (result.size () == 4) ;
        REQUIRE (result [0] == "cowbell!") ;
        REQUIRE (result [1] == "more cowbell!") ;
        REQUIRE (result [2] == "gotta have more cowbell!") ;
        REQUIRE (result [3] == "gotta have\\, again\\, more cowbell!") ;
    }
}
