/* -*- mode: c++; coding: utf-8 -*- */
/*
 * node.cpp:
 *
 * Copyright (c) 2015 Masashi Fujita
 */
#include <iostream>
#include <iomanip>
#include "node.hpp"
namespace {

    struct indent_ {
        int level_ = 0;

        indent_ (int lvl) : level_ { lvl } {
            /* NO-OP */
        }
    };

    std::ostream & operator << (std::ostream & output, const indent_ & ind) {
        for (int i = 0; i < ind.level_; ++i) {
            output << "  ";
        }
        return output;
    }

    indent_ indent (int lvl) {
        return indent_ { lvl };
    }


    static void append_vector (std::vector<std::string> & vec, const std::vector<std::string> & v) {
        if (v.empty ()) {
            return;
        }
        if (vec.empty ()) {
            vec = v;
            return;
        }
        if (v.size () == 1) {
            for (auto & rv : vec) {
                rv += v[0];
            }
        }
        else {
            std::vector<std::string> tmp;
            tmp.reserve (vec.size () * v.size ());

            for (auto const & rv : vec) {
                for (auto const & s : v) {
                    tmp.push_back (rv + s);
                }
            }
            std::swap (vec, tmp);
        }
    }
}


namespace ExpandBrace {

    void BaseNode::dump (std::ostream & output, int level) const {
        /* NO-OP */
    }

    void StringNode::dump (std::ostream & output, int level) const {
        output << indent (level) << (value_.empty () ? "<empty>" : value_) << std::endl;
    }

    void ListNode::dump (std::ostream & output, int level) const {
        output << indent (level) << "<list>" << std::endl;
        for (auto const & v : values_) {
            v->dump (output, 1 + level);
        }
    }

    std::vector<std::string> ConcatNode::getValue () const {
        std::vector<std::string> result;
        for (auto const & v : values_) {
            auto const & children = v->getValue ();
            append_vector (result, children);
        }
        return result;
    }

    void ConcatNode::dump (std::ostream & output, int level) const {
        output << indent (level) << "<concat>" << std::endl;
        for (auto const & v : values_) {
            v->dump (output, 1 + level);
        }
    }
}
