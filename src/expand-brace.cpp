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

struct indent_ {
    int level_ = 0 ;

    indent_ (int lvl) : level_ { lvl } {
        /* NO-OP */
    }
} ;

std::ostream &  operator << (std::ostream &output, const indent_ &ind) {
    for (int i = 0 ; i < ind.level_ ; ++i) {
        output << "  " ;
    }
    return output ;
}

indent_ indent (int lvl) {
    return indent_ { lvl } ;
}

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
    void dump (std::ostream &output, int level) const override {
        output << indent (level) << (value_.empty () ? "<empty>" : value_) << std::endl ;
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
    ListNode (std::vector<std::shared_ptr<BaseNode>> &&values) : values_ { std::move (values) } {
        /* NO-OP */
    }

    ListNode (const std::vector<std::shared_ptr<BaseNode>> & values) : values_ { values } {
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
    void dump (std::ostream &output, int level) const override {
        output << indent (level) << "<list>" << std::endl ;
        for (auto const &v : values_) {
            v->dump (output, 1 + level) ;
        }
    }
} ;


class ConcatNode final : public BaseNode {
private:
    std::vector<std::shared_ptr<BaseNode>>  values_ ;
public:
    ~ConcatNode () override { /* NO-OP */ }
    ConcatNode () { /* NO-OP */ }

    ConcatNode (std::vector<std::shared_ptr<BaseNode>> && values) : values_ { std::move (values) } {
        /* NO-OP */
    }

    ConcatNode (const std::vector<std::shared_ptr<BaseNode>> &values) : values_ { values } {
        /* NO-OP */
    }
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

    void dump (std::ostream & output, int level) const override {
        output << indent (level) << "<concat>" << std::endl ;
        for (auto const & v : values_) {
            v->dump (output, 1 + level);
        }
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
            auto frag = parse_fragments (it, it_end) ;
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
    std::vector<std::shared_ptr<BaseNode>>  result ;

    while (it != it_end) {
        std::shared_ptr<BaseNode>   frag ;
        std::tie (frag, it) = parse_fragment (it, it_end) ;
        result.emplace_back (frag) ;
        if (*it == ',' || *it == '}') {
            switch (result.size ()) {
            case 0:
                return std::make_tuple (std::make_shared<StringNode> (), it);
            case 1:
                return std::make_tuple (result[0], it);
            default:
                break ;
            }
            return std::make_tuple (std::make_shared<ConcatNode> (std::move (result)), it);
        }
    }
    return std::make_tuple (std::make_shared<ConcatNode> (std::move (result)), it) ;
}

std::vector<std::string>    expand_brace (const std::string &src) {
    if (src.empty ()) {
        return std::vector<std::string> {} ;
    }
    std::shared_ptr<BaseNode>   node ;
    iterator_t it ;
    std::tie (node, it) = parse_fragments (cbegin (src), cend (src)) ;
    if (it != cend (src)) {
        throw std::runtime_error { std::string { "Syntax error in \""}.append (src).append ("\"") } ;
    }
    return node->getValue () ;
}
