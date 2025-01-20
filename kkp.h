////////////////////////////////////////////////////////////////////////////////
// kkp.h
//   The main header for KKP algorithms. Only this file needs to be included
//   to use parsing algorithms.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Juha Karkkainen, Dominik Kempa and Simon J. Puglisi
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#ifndef __KKP_H
#define __KKP_H

#include <string>
#include <vector>
#include <cstring>
#include "malloc.h"
#ifdef _USE_64
typedef int64_t INT;
#endif

#ifdef _USE_32
typedef int32_t INT;

#endif
// Two functions below share the same interface.
// Arguments:
//   X[0..n-1] = input string,
//   SA[0..n-1] = suffix array of X,
//   phrase_start_locations


void printmemory_usage();

void kkp3(unsigned char *X, INT *SA, INT n, std::vector<INT> &phrase_start_locations);
void kkp2(unsigned char *X, INT *SA, INT n,std::vector<INT> &phrase_start_locations);

INT parse_phrase(unsigned char *X, INT n, INT i, INT psv, INT nsv, std::vector<INT> &phrase_start_locations);

unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B, std::vector<INT> &hash_positions);


#endif // __KKP_H
