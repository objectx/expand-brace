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

namespace ExpandBrace {

    ParseResult<StringNode> parse_string (iterator_t it, iterator_t it_end, int level) {
        std::string result;

        bool in_escape = false ;
        while (it != it_end) {
            switch (*it) {
            case '\\':
                if (in_escape) {
                    goto onward ;
                }
                else {
                    in_escape = true ;
                }
                break ;
            case '{':
            case '}':
                if (in_escape) {
                    goto onward ;
                }
                return ParseResult<StringNode> { std::make_unique<StringNode> (std::move (result)), it } ;
            case ',':
                if (in_escape) {
                    goto onward ;
                }
                if (0 < level) {
                    return ParseResult<StringNode> { std::make_unique<StringNode> (std::move (result)), it } ;
                }
                /*FALLTHROUGH*/
            default:
            onward:
                in_escape = false ;
                result += *it;
                ++it;
                break;
            }
        }
        return ParseResult<StringNode> { std::make_unique<StringNode> (std::move (result)), it };
    }

    ParseResult<ListNode> parse_list (iterator_t it, iterator_t it_end, int level) {
        auto result = std::make_unique<ListNode> ();
        auto beg    = it;

        while (it != it_end) {
            switch (*it) {
            case ',':
                if (it == beg) {
                    // Handle "{," case
                    result->add (std::make_unique<StringNode> ());
                }
                ++it;
                break;
            case '}':
                if (it == beg) {
                    // Handle "{}" case
                    return ParseResult<ListNode> { std::move (result), it + 1 } ;
                }
                if (*(it - 1) == ',') {
                    // Handle "ABC,}" case
                    result->add (std::make_unique<StringNode> ());
                }
                return ParseResult<ListNode> { std::move (result), it + 1 } ;
            default:
                auto frag = parse_fragments (it, it_end, level);
                result->add (std::move (frag.value)) ;
                it = frag.next ;
                break;
            }
        }
        throw std::runtime_error { std::string { "Unmatched brace" } };
    }

    ParseResult<BaseNode> parse_fragment (iterator_t it, iterator_t it_end, int level) {
        if (*it == '{') {
            ++it;
            return parse_list (it, it_end, level + 1);
        }
        else {
            return parse_string (it, it_end, level);
        }
    }

    ParseResult<BaseNode> parse_fragments (iterator_t it, iterator_t it_end, int level) {
        std::vector<std::unique_ptr<BaseNode>> result;

        while (it != it_end) {
            auto frag = parse_fragment (it, it_end, level);
            result.emplace_back (std::move (frag.value));
            it = frag.next ;
            if (*it == '}' || (*it == ',' && 0 < level)) {
                switch (result.size ()) {
                case 0:
                    return ParseResult<BaseNode> { std::make_unique<StringNode> (), it } ;
                case 1:
                    return ParseResult<BaseNode> { std::move (result [0]), it } ;
                default:
                    break;
                }
                return ParseResult<BaseNode> { std::make_unique<ConcatNode> (std::move (result)), it } ;
            }
        }
        return ParseResult<BaseNode> { std::make_unique<ConcatNode> (std::move (result)), it } ;
    }
}

std::vector<std::string>    expand_brace (const std::string &src) {
    if (src.empty ()) {
        return std::vector<std::string> {} ;
    }
    auto result = ExpandBrace::parse_fragments (cbegin (src), cend (src), 0);
    if (result.next != cend (src)) {
        throw std::runtime_error { std::string { "Syntax error in \""}.append (src).append ("\"") } ;
    }
    return result.value->getValue () ;
}

