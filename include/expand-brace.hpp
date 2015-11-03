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

/**
 * Performs shell style brace expansion.
 *
 * @param src String to expand
 *
 * @return Expanded string
 *
 * @see <a href="https://rosettacode.org/wiki/Brace_expansion">Brace expansion</a>
 */
std::vector<std::string> expand_brace (const std::string &src) ;

#endif /* expand_brace_hpp__2DC3DABF_F926_4915_8B4D_5CFF4F6C39CD */
