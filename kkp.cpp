////////////////////////////////////////////////////////////////////////////////
// kkp.cpp
//   Implementation of main parsing functions.
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

#include <stdint.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>


#include "kkp.h"


#define STACK_BITS 16
#define STACK_SIZE (1 << STACK_BITS)
#define STACK_HALF (1 << (STACK_BITS - 1))
#define STACK_MASK ((STACK_SIZE) - 1)

// An auxiliary routine used during parsing.


void printmemory_usage() {
    struct mallinfo2 mi = mallinfo2();
    std::cout<< "Memory usage: " << (mi.uordblks + mi.hblkhd) /(1024.0 * 1024.0)<<std::endl;
}

INT parse_phrase(unsigned char *X, INT n, INT i, INT psv, INT nsv, std::vector<INT> &phrase_start_locations)
{
    INT pos, len = 0;
    if (nsv == -1) {
        while (X[psv + len] == X[i + len]) ++len;
        pos = psv;
    } else if (psv == -1) {
        while (i + len < n && X[nsv + len] == X[i + len]) ++len;
        pos = nsv;
    } else {
        while (X[psv + len] == X[nsv + len]) ++len;
        if (X[i + len] == X[psv + len]) {
            ++len;
            while (X[i + len] == X[psv + len]) ++len;
            pos = psv;
        } else {
            while (i + len < n && X[i + len] == X[nsv + len]) ++len;
            pos = nsv;
        }
    }
    if (len == 0) pos = X[i];
//  if (F) F->push_back(std::make_pair(pos, len));
//  std::cout<<"pos, len: "<<pos<<","<<len<<std::endl;
    phrase_start_locations.push_back(i);
//    std::cout<<"i "<<i<<std::endl;

    return i + std::max( (INT)1, len);
}

void kkp2(unsigned char *X, INT *SA, INT n,std::vector<INT> &phrase_start_locations) {
    INT *CS = new INT[n + 5];
    INT *stack = new INT[STACK_SIZE + 5];
    INT top = 0;
    stack[top] = 0;

    // Compute PSV_text for SA and save into CS.
    CS[0] = -1;
    for (INT i = 1; i <= n; ++i) {
        INT sai = SA[i - 1] + 1;
        while (stack[top] > sai) --top;
        if ((top & STACK_MASK) == 0) {
            if (stack[top] < 0) {
                // Stack empty -- use implicit.
                top = -stack[top];
                while (top > sai) top = CS[top];
                stack[0] = -CS[top];
                stack[1] = top;
                top = 1;
            } else if (top == STACK_SIZE) {
                // Stack is full -- discard half.
                for (INT j = STACK_HALF; j <= STACK_SIZE; ++j)
                    stack[j - STACK_HALF] = stack[j];
                stack[0] = -stack[0];
                top = STACK_HALF;
            }
        }

        INT addr = sai;
        CS[addr] = std::max((INT)0, stack[top]);
        ++top;
        stack[top] = sai;
    }
//    printmemory_usage();

    delete[] stack;

    // Compute the phrases.
    CS[0] = 0;
//    INT nfactors = 0, next = 1, nsv, psv;
    INT next = 1, nsv, psv;

    for (INT t = 1; t <= n; ++t) {
        psv = CS[t];
        nsv = CS[psv];
        if (t == next) {
            next = parse_phrase(X, n, t - 1, psv - 1, nsv - 1, phrase_start_locations) + 1;
//            ++nfactors;
        }
        CS[t] = nsv;
        CS[psv] = t;
    }

    // Clean up.
    delete[] CS;
//    return nfactors;
}

// TODO: current version overwrites SA, this can
// be avoided, similarly as in KKP2.
void kkp3(unsigned char *X, INT *SA, INT n,std::vector<INT> &phrase_start_locations) {
//    if (n == 0) return 0;
    INT *CPSS = new INT[2 * n + 5];

    // This can be avoided too.
    for (INT i = n; i; --i)
        SA[i] = SA[i - 1];
    SA[0] = SA[n + 1] = -1;

    // Compute PSV_text and NSV_text for SA.
    INT top = 0;
    for (INT i = 1; i <= n + 1; ++i) {
        while (SA[top] > SA[i]) {
            INT addr = (SA[top] << 1);
            CPSS[addr] = SA[top - 1];
            CPSS[addr + 1] = SA[i];
            --top;
        }
        SA[++top] = SA[i];
    }

    // Compute the phrases.
//    if (F) F->push_back(std::make_pair(X[0], 0));
    phrase_start_locations.push_back(0);
//    INT i = 1, nfactors = 1;
    INT i = 1;

    while(i < n) {
        INT addr = (i << 1);
        INT psv = CPSS[addr];
        INT nsv = CPSS[addr + 1];
        i = parse_phrase(X, n, i, psv, nsv, phrase_start_locations);
//        ++nfactors;
    }

    // Clean up.
    delete[] CPSS;
//    return nfactors;
}





unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B, std::vector<INT> &hash_positions) {
    INT n = std::strlen((char*)text);
    // +1 : $ +1 : \0
    unsigned char* temp_string = new unsigned char[n + 1];  // Temporary buffer (with space for '\0')
    INT temp_string_index = 0;  // Index for appending to temp_string
    bool last_position_was_far = false;
    INT m = phrase_starts.size();
    INT current_boundary_index = 0;

    // Construct the string in temp_string
    for (INT i = 0; i < n; i++) {
        while (current_boundary_index < m && phrase_starts[current_boundary_index] < i - B + 1) {
            current_boundary_index++;
        }

        bool near_boundary = (
                                     current_boundary_index < m && abs(phrase_starts[current_boundary_index] - i) < B
                             ) || (
                                     current_boundary_index > 0 && abs(phrase_starts[current_boundary_index - 1] - i) < B
                             );

        if (near_boundary) {
            temp_string[temp_string_index++] = text[i];  // Append the original character
            last_position_was_far = false;
        } else {
            if (!last_position_was_far) {
                hash_positions.push_back(temp_string_index);

                temp_string[temp_string_index++] = '#';  // Append the '#' symbol
                last_position_was_far = true;
            }
        }
    }

    temp_string[temp_string_index] = '\0';  // Null-terminate the string


    // Allocate the exact amount of memory needed for the final string
    unsigned char* new_string = new unsigned char[temp_string_index + 1];  // +1 for the null terminator
    std::memcpy(new_string, temp_string, temp_string_index + 1);  // Copy the temp string to the new string

    delete[] temp_string;  // Free the temporary buffer
    return new_string;  // Return the constructed string
}
