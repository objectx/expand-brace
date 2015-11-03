/* -*- mode: c++; coding: utf-8 -*- */
/*
 * node.hpp:
 *
 * Copyright (c) 2015 Masashi Fujita
 */
#pragma once
#ifndef node_hpp__97AD1C80_A899_4857_8C71_A3667AD34CC8
#define node_hpp__97AD1C80_A899_4857_8C71_A3667AD34CC8  1

#include <iosfwd>
#include <string>
#include <vector>

class BaseNode {
public:
    virtual ~BaseNode () {
        /* NO-OP */
    }

    BaseNode () {
        /* NO-OP */
    }

    virtual std::vector <std::string> getValue () const = 0;

    virtual void dump (std::ostream & output, int level = 0) const;
};


class StringNode final : public BaseNode {
private:
    std::string value_;
public:
    ~StringNode () override { /* NO-OP */ }

    StringNode () { /* NO-OP */ }

    StringNode (std::string && s) : value_ { std::move (s) } {
        /* NO-OP */
    }

    StringNode (const std::string & s) : StringNode { std::string { s } } {
        /* NO-OP */
    }

    std::vector<std::string> getValue () const override {
        return std::vector<std::string> { value_ };
    }

    void dump (std::ostream & output, int level) const override ;
};


class ListNode final : public BaseNode {
private:
    std::vector<std::unique_ptr<BaseNode>> values_;
public:
    ~ListNode () override { /* NO-OP */ }

    ListNode () {
        /* NO-OP */
    }

    ListNode (std::vector<std::unique_ptr<BaseNode>> && values) : values_ { std::move (values) } {
        /* NO-OP */
    }

    ListNode & add (std::unique_ptr<BaseNode> && value) {
        values_.emplace_back (std::move (value));
        return *this;
    }

    std::vector<std::string> getValue () const override {
        std::vector<std::string> result;
        for (auto const          & v : values_) {
            auto && children = v->getValue ();
            result.insert (cend (result), begin (children), end (children));
        }
        return result;
    }

    void dump (std::ostream & output, int level) const;
};


class ConcatNode final : public BaseNode {
private:
    std::vector<std::unique_ptr<BaseNode>> values_;
public:
    ~ConcatNode () override { /* NO-OP */ }

    ConcatNode () { /* NO-OP */ }

    ConcatNode (std::vector<std::unique_ptr<BaseNode>> && values) : values_ { std::move (values) } {
        /* NO-OP */
    }

    ConcatNode & add (std::unique_ptr<BaseNode> && value) {
        values_.emplace_back (std::move (value));
        return *this;
    }

    std::vector<std::string> getValue () const;

    void dump (std::ostream & output, int level) const;
};


template <typename T_>
    using parse_result_t = std::tuple<std::unique_ptr<T_>, std::string::const_iterator>;

parse_result_t<StringNode> parse_string ( std::string::const_iterator it
                                        , std::string::const_iterator it_end
                                        , int level);

parse_result_t<ListNode> parse_list ( std::string::const_iterator it
                                    , std::string::const_iterator it_end
                                    , int level);

parse_result_t<BaseNode> parse_fragment ( std::string::const_iterator it
                                        , std::string::const_iterator it_end
                                        , int level);

parse_result_t<BaseNode> parse_fragments ( std::string::const_iterator it
                                         , std::string::const_iterator it_end
                                         , int level);

#endif /* node_hpp__97AD1C80_A899_4857_8C71_A3667AD34CC8 */