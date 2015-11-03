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

using iterator_t = std::string::const_iterator;

class StringNode final : public BaseNode {
private:
    std::string value_ ;
public:
    ~StringNode () override { /* NO-OP */ }
    StringNode () { /* NO-OP */ }
    StringNode (std::string &&s) : value_ { std::move (s) } {
        /* NO-OP */
    }
    StringNode (const std::string &s) : StringNode { std::string { s } } {
        /* NO-OP */
    }
    std::vector<std::string>    getValue () const override {
        return std::vector<std::string> { value_ } ;
    }
} ;

static void append_vector (std::vector<std::string> &vec, const std::vector<std::string> &v) {
    if (v.empty ()) {
        return ;
    }
    if (vec.empty ()) {
        vec = v ;
        return ;
    }
    if (v.size () == 1) {
        for (auto &rv : vec) {
            rv += v [0] ;
        }
    }
    else {
        std::vector<std::string>    tmp ;
        tmp.reserve (vec.size () * v.size ()) ;

        for (auto const &rv : vec) {
            for (auto const &s : v) {
                tmp.push_back (rv + s) ;
            }
        }
        std::swap (vec, tmp) ;
    }
}


class ListNode final : public BaseNode {
private:
    std::vector<std::shared_ptr<BaseNode>>  values_ ;
public:
    ~ListNode () override { /* NO-OP */ }
    ListNode () {
        /* NO-OP */
    }
    ListNode &  add (std::shared_ptr<BaseNode> value) {
        values_.emplace_back (value) ;
        return *this ;
    }
    std::vector<std::string>    getValue () const override {
        std::vector<std::string>    result ;
        for (auto v : values_) {
            auto && children = v->getValue () ;
            result.insert (cend (result), begin (children), end (children)) ;
        }
        return result ;
    }
} ;


class ConcatNode final : public BaseNode {
private:
    std::vector<std::shared_ptr<BaseNode>>  values_ ;
public:
    ~ConcatNode () override { /* NO-OP */ }
    ConcatNode () { /* NO-OP */ }
    ConcatNode &    add (std::shared_ptr<BaseNode> value) {
        values_.emplace_back (value) ;
        return *this ;
    }
    std::vector<std::string>    getValue () const override {
        std::vector<std::string>    result ;
        for (auto v : values_) {
            auto const &    children = v->getValue () ;
            append_vector (result, children) ;
        }
        return result ;
    }
};

parse_result_t<StringNode> parse_string (iterator_t it, iterator_t it_end) {
    std::string result;
    while (it != it_end) {
        switch (*it) {
        case '{':
        case ',':
        case '}':
            return std::make_tuple (std::make_shared<StringNode> (std::move (result)), it) ;
        default:
            result += *it;
            ++it;
            break;
        }
    }
    return std::make_tuple (std::make_shared<StringNode> (std::move (result)), it) ;
}

parse_result_t<ListNode>   parse_list (iterator_t it, iterator_t it_end) {
    auto result = std::make_shared<ListNode> () ;
    auto beg = it ;

    while (it != it_end) {
        switch (*it) {
        case ',':
            if (it == beg) {
                // Handle "{," case
                result->add (std::make_shared<StringNode> ()) ;
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
                result->add (std::make_shared<StringNode> ()) ;
            }
            return std::make_tuple (std::move (result), it + 1);
        default:
            auto frag = parse_fragment (it, it_end) ;
            result->add (std::get<0> (frag)) ;
            it = std::get<1> (frag) ;
            break ;
        }
    }
    return std::make_tuple (std::move (result), it) ;
}

parse_result_t<BaseNode>   parse_fragment (iterator_t it, iterator_t it_end) {
    if (*it == '{') {
        ++it ;
        return parse_list (it, it_end);
    }
    else {
        return parse_string (it, it_end);
    }
}

parse_result_t<BaseNode>   parse_fragments (iterator_t it, iterator_t it_end) {
    auto result = std::make_shared<ConcatNode> () ;
    while (it != it_end) {
        auto frag = parse_fragment (it, it_end);
        result->add (std::get<0> (frag)) ;
        it = std::get<1> (frag) ;
        if (*it == ',' || *it == '}') {
            return std::make_tuple (result, it) ;
        }
    }
    return std::make_tuple (result, it) ;
}

std::vector<std::string>    expand_brace (const std::string &src) {
    if (src.empty ()) {
        return std::vector<std::string> {} ;
    }
    auto result = parse_fragments (cbegin (src), cend (src)) ;
    if (std::get<1> (result) != cend (src)) {
        throw std::runtime_error { std::string { "Syntax error in \""}.append (src).append ("\"") } ;
    }
    return std::get<0> (result)->getValue () ;
}
