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
                    ++it ;
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
                if (in_escape) {
                    result += '\\' ;
                    result += *it ;
                }
                else {
                    result += *it ;
                }
                in_escape = false ;
                ++it ;
                break;
            }
        }
        return ParseResult<StringNode> { std::make_unique<StringNode> (std::move (result)), it };
    }

    ParseResult<BaseNode> parse_list (iterator_t it, iterator_t it_end, int level) {
        std::vector<std::unique_ptr<BaseNode>>  result ;
        auto beg    = it;

        int prev_ch = -1 ;
        while (it != it_end) {
            switch (*it) {
            case ',':
                if (it == beg) {
                    // Handle "{," case
                    result.emplace_back (std::make_unique<StringNode> ()) ;
                }
                prev_ch = *it;
                ++it;
                break;
            case '}':
                if (prev_ch == ',') {
                    // Handle "ABC,}" case
                    result.emplace_back (std::make_unique<StringNode> ());
                }
                if (result.size () <= 1) {
                    // Special case
                    result.insert (result.cbegin (), std::make_unique<StringNode> (std::string { "{" })) ;
                    result.emplace_back (std::make_unique<StringNode> (std::string { "}" })) ;
                    return ParseResult<ConcatNode> { std::make_unique<ConcatNode> (std::move (result)), it + 1 } ;
                }
                return ParseResult<ListNode> { std::make_unique<ListNode> (std::move (result)), it + 1 } ;
            default:
                auto frag = parse_fragments (it, it_end, level);
                result.emplace_back (std::move (frag.value)) ;
                prev_ch = -1 ;
                it = frag.next ;
                break;
            }
        }
        std::vector<std::unique_ptr<BaseNode>> tmp ;
        tmp.reserve (1 + 2 * result.size () + 1) ;
        tmp.emplace_back (std::make_unique<StringNode> (std::string { '{' })) ;
        if (! result.empty ()) {
            tmp.emplace_back (std::move (result [0])) ;
            for (int i = 1 ; i < result.size () ; ++i) {
                tmp.emplace_back (std::make_unique<StringNode> (std::string { ',' }));
                tmp.emplace_back (std::move (result [i])) ;
            }
        }
        return ParseResult<ConcatNode> { std::make_unique<ConcatNode> (std::move (tmp)), it };
        //throw std::runtime_error { std::string { "Unmatched brace" } };
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
            //std::cerr << " in> " << std::string (it, it_end) << std::endl;
            auto frag = parse_fragment (it, it_end, level);
            //frag.value->dump (std::cerr);
            //std::cerr << "out> " << std::string (frag.next, it_end) << std::endl;
            result.emplace_back (std::move (frag.value));
            it = frag.next ;
            if (level == 0) {
                // Level 0's '}' and ',' are considered as itself
                if (*it == '}' || *it == ',') {
                    result.emplace_back (std::make_unique<StringNode> (std::string { it, it + 1 })) ;
                    ++it ;
                }
            }
            else {
                if (*it == '}' || *it == ',') {
                    switch (result.size ()) {
                    case 0:
                        return ParseResult<BaseNode> { std::make_unique<StringNode> (), it };
                    case 1:
                        return ParseResult<BaseNode> { std::move (result[0]), it };
                    default:
                        break;
                    }
                    return ParseResult<BaseNode> { std::make_unique<ConcatNode> (std::move (result)), it };
                }
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

