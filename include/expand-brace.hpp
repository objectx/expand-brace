/*
 * expand-brace.hpp:
 *
 * Copyright (c) 2015 Masashi Fujita
 */
#pragma once
#ifndef expand_brace_hpp__2DC3DABF_F926_4915_8B4D_5CFF4F6C39CD
#define expand_brace_hpp__2DC3DABF_F926_4915_8B4D_5CFF4F6C39CD

#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <iosfwd>

std::vector<std::string>    expand_brace (const std::string &src) ;

class BaseNode {
public:
    virtual ~BaseNode () {
        /* NO-OP */
    }

    BaseNode () {
        /* NO-OP */
    }

    virtual std::vector<std::string> getValue () const = 0;
    virtual void dump (std::ostream &output, int level = 0) const {
        /* NO-OP */
    }
};

class ConcatNode ;
class ListNode ;
class StringNode ;

template <typename T_>
    using parse_result_t = std::tuple<std::shared_ptr<T_>, std::string::const_iterator> ;
parse_result_t<StringNode> parse_string (std::string::const_iterator it, std::string::const_iterator it_end, int level);

parse_result_t<ListNode> parse_list (std::string::const_iterator it, std::string::const_iterator it_end, int level);

parse_result_t<BaseNode> parse_fragment (std::string::const_iterator it, std::string::const_iterator it_end, int level);

parse_result_t<BaseNode> parse_fragments (std::string::const_iterator it, std::string::const_iterator it_end, int level);

#endif /* expand_brace_hpp__2DC3DABF_F926_4915_8B4D_5CFF4F6C39CD */
