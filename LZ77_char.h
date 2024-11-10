#ifndef CPM_LZ77_H
#define CPM_LZ77_H

#include <string>

#include <vector>

#include "suffixTree.h"

#include <variant>
#include <cstring>


#include <malloc.h>


void printmemory_usage();

class LZ77 {
public:
    LZ77(unsigned char* text);

    void compress(std::vector<INT> &phrase_start_locations);

    std::pair<INT, INT> find_longest_match(INT start, INT limit);
    std::pair<INT, INT> find_longest_match_brute_force(INT start);
    unsigned char* T;

    suffixTree* ST;
    INT n;

    ~LZ77();

private:
    int hit_factor = 2;
    int k = 7;
    vector<int>* hashes;
    vector<vector<int>>* hits;

};

vector<vector<int>> compute_hits(unsigned char* text, int hit_factor);


unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B, vector<INT> &hash_positions);


unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B1,  INT &B2, vector<INT> &hash_positions);


#endif
