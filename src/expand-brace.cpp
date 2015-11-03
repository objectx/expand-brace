/* -*- mode: c++; coding: utf-8 -*- */
/*
 * expand-brace.cpp:
 *
 * Copyright (c) 2015 Masashi Fujita
 */
#include <iterator>
#include <tuple>
#include <iostream>
#include <assert.h>
#include "expand-brace.hpp"
#include "node.hpp"

using iterator_t = std::string::const_iterator;


parse_result_t<StringNode> parse_string (iterator_t it, iterator_t it_end, int level) {
    std::string result;

    while (it != it_end) {
        switch (*it) {
        case '{':
        case '}':
            return std::make_tuple (std::make_unique<StringNode> (std::move (result)), it);
        case ',':
            if (0 < level) {
                return std::make_tuple (std::make_unique<StringNode> (std::move (result)), it);
            }
            /*FALLTHROUGH*/
        default:
            result += *it;
            ++it;
            break;
        }
    }
    return std::make_tuple (std::make_unique<StringNode> (std::move (result)), it) ;
}

parse_result_t<ListNode> parse_list (iterator_t it, iterator_t it_end, int level) {
    auto result = std::make_unique<ListNode> () ;
    auto beg = it ;

    while (it != it_end) {
        switch (*it) {
        case ',':
            if (it == beg) {
                // Handle "{," case
                result->add (std::make_unique<StringNode> ()) ;
            }
            ++it ;
            break ;
        case '}':
            if (it == beg) {
                // Handle "{}" case
                return std::make_tuple (std::move (result), it + 1) ;
            }
            if (*(it - 1) == ',') {
                // Handle "ABC,}" case
                result->add (std::make_unique<StringNode> ()) ;
            }
            return std::make_tuple (std::move (result), it + 1);
        default:
            auto frag = parse_fragments (it, it_end, level);
            result->add (std::move (std::get<0> (frag))) ;
            it = std::get<1> (frag) ;
            break ;
        }
    }
    return std::make_tuple (std::move (result), it) ;
}

parse_result_t<BaseNode> parse_fragment (iterator_t it, iterator_t it_end, int level) {
    if (*it == '{') {
        ++it ;
        return parse_list (it, it_end, level + 1);
    }
    else {
        return parse_string (it, it_end, level);
    }
}

parse_result_t<BaseNode> parse_fragments (iterator_t it, iterator_t it_end, int level) {
    std::vector<std::unique_ptr<BaseNode>>  result ;

    while (it != it_end) {
        std::unique_ptr<BaseNode>   frag ;
        std::tie (frag, it) = parse_fragment (it, it_end, level);
        result.emplace_back (std::move (frag)) ;
        if (*it == '}' || (*it == ',' && 0 < level)) {
            switch (result.size ()) {
            case 0:
                return std::make_tuple (std::make_unique<StringNode> (), it);
            case 1:
                return std::make_tuple (std::move (result[0]), it);
            default:
                break ;
            }
            return std::make_tuple (std::make_unique<ConcatNode> (std::move (result)), it);
        }
    }
    return std::make_tuple (std::make_unique<ConcatNode> (std::move (result)), it) ;
}

std::vector<std::string>    expand_brace (const std::string &src) {
    if (src.empty ()) {
        return std::vector<std::string> {} ;
    }
    std::unique_ptr<BaseNode>   node ;
    iterator_t it ;
    std::tie (node, it) = parse_fragments (cbegin (src), cend (src), 0);
    if (it != cend (src)) {
        throw std::runtime_error { std::string { "Syntax error in \""}.append (src).append ("\"") } ;
    }
    return node->getValue () ;
}

