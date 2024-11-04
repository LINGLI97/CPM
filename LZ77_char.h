#ifndef CPM_LZ77_H
#define CPM_LZ77_H

#include <string>

#include <vector>

#include "suffixTree.h"

#include <variant>
#include <cstring>

class LZ77 {
public:
    LZ77(unsigned char* text);

    void compress(std::vector<std::pair<INT, std::variant<INT, unsigned char>>> &result, std::vector<INT> &phrase_start_locations);

    std::pair<INT, INT> find_longest_match(INT start);
    unsigned char* T;

    suffixTree* ST;
    INT n;

    ~LZ77();

};

unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B, vector<INT> &hash_positions);


unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B1,  INT &B2, vector<INT> &hash_positions);


#endif
