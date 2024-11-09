#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>  // For strlen and memcpy
#include "LZ77_char.h"


void printmemory_usage() {
    struct mallinfo2 mi = mallinfo2();
    cout<< "Memory usage: " << (mi.uordblks + mi.hblkhd) /(1024.0 * 1024.0)<<endl;
}

LZ77::LZ77(unsigned char* text) {
    this->n = std::strlen((char*)text);

    // add $ to the end of text

    this->T = text;

    this->hashes = new vector<int>(this->n);
    this->hits = new vector<vector<int>>(this->n/this->hit_factor);

    int base = 4;
    int mod = this->n/this->hit_factor;
    cout << "n = " << this->n << endl;
    cout << "mod = " << mod << endl;

    
    int precomputedPower = 1;  // Reset power calculation for this length
    int hash = 0;
    for (int i = 0; i < k; i++) {
        hash = (hash * base + T[i]) % mod;
        precomputedPower = (precomputedPower * base) % mod;
    }

    cout << "Computing KR-hashes for LZ77 parse..." << endl;
    //cout << hash << endl;
    hashes->at(0) = hash;
    hits->at(0).push_back(0);
    for (int i = 1; i < n - k; i++) {
        //cout << "T[i-1] = " << T[i-1] << endl;
        //cout << "T[i+k] = " << T[i+k] << endl;

        // Update the hash by rolling (only if there's another window to check)
        hash = (hash - T[i-1] * precomputedPower % mod + mod) % mod;
        hash = (hash * base + T[i + k]) % mod;
        //cout << i << ":" << hash << endl;
        hits->at(hash).push_back(i);
        hashes->at(i) = hash;
    }
    

    cout << "Done computing hashes" << endl;
    /*
    // print for debugging
    for(int i = 0; i < hits->size(); i++){
        cout << i << ": ";
        for(int j = 0; j < hits->at(i).size(); j++){
            cout << hits->at(i).at(j) << " ";
        }
        cout << endl;
    }
    */

}


int rollHash(int prevHash, char oldChar, char newChar, int k, int power, int base, int mod) {
    prevHash = (prevHash - oldChar * power % mod + mod) % mod;
    prevHash = (prevHash * base + newChar) % mod;
    return prevHash;
}


void LZ77::compress(std::vector<INT> &phrase_start_locations) {

//    std::cout << "Performing LZ77 factorization..." << std::endl;
    size_t i = 0;

    // use hashes where available
    while (i < n-k) {
        //cout << i << endl;
        auto [match_length, match_distance] = find_longest_match(i);
        phrase_start_locations.push_back(i);
        i += (match_length == 0) ? 1 : match_length;
    }
    // brute force remaining
    while(i >= n-k && i < n){
        //cout << i << endl;
        auto [match_length, match_distance] = find_longest_match_brute_force(i);
        phrase_start_locations.push_back(i);
        i += (match_length == 0) ? 1 : match_length;

    }

    printmemory_usage();
}


std::pair<INT, INT> LZ77::find_longest_match_brute_force(INT start) {

    INT match_length = 0;
    INT match_position = -1;
    INT i = start;

    for(int j = 0; j < i; j++){
        //cout << "\tj: " << j << endl;
        int k = 0;
        while(k < n && T[i+k] == T[j+k])
        {
            //cout << "\t\tk: " << k << endl;
            k++;
        }
        if(k > match_length){
            match_length = k;
            match_position = j;
        }
    }
    

    if (match_position != -1 && match_length > 0) {
        return std::make_pair(match_length, start - match_position);
    }
    return std::make_pair(0, 0);
}




std::pair<INT, INT> LZ77::find_longest_match(INT start) {

    INT match_length = 0;
    INT match_position = -1;
    INT i = start;

    int hash = this->hashes->at(i);
    //cout << "hash: " << hash << endl;
    for(int j : this->hits->at(hash)){
        if(j >= i){
            break;
        }
        //cout << "\tj: " << j << endl;
        int k = 0;
        while(k < n && T[i+k] == T[j+k])
        {
            //cout << "\t\tk: " << k << endl;
            k++;
        }
        if(k > match_length){
            match_length = k;
            match_position = j;
        }
    }
    
    if (match_position != -1 && match_length > 0) {
        return std::make_pair(match_length, start - match_position);
    }
    return std::make_pair(0, 0);
}










unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B, vector<INT> &hash_positions) {
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



unsigned char* construct_string_from_boundaries(unsigned char* text, const std::vector<INT>& phrase_starts, INT &B1,  INT &B2, vector<INT> &hash_positions) {
    INT n = std::strlen((char*)text);
    // +1 : $ +1 : \0
    unsigned char* temp_string = new unsigned char[n + 1];  // Temporary buffer (with space for '\0')
    INT temp_string_index = 0;  // Index for appending to temp_string
    bool last_position_was_far = false;
    INT m = phrase_starts.size();
    INT current_boundary_index = 0;

    // Construct the string in temp_string

    bool near_boundary = true;

    for (INT i = 0; i < n; i++) {
        while ( i - phrase_starts[current_boundary_index] >= B1) {
            current_boundary_index++;
        }


        if (i-phrase_starts[current_boundary_index] >= 0 and i - phrase_starts[current_boundary_index] < B1) {
            near_boundary = true;
        } else if(i-phrase_starts[current_boundary_index] < 0 and phrase_starts[current_boundary_index] - i < B2){
            near_boundary = true;
        } else{
            near_boundary = false;
        }

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

    //add $ at the end of new string
//    temp_string[temp_string_index++] = '$';

    temp_string[temp_string_index] = '\0';  // Null-terminate the string


    // Allocate the exact amount of memory needed for the final string
    unsigned char* new_string = new unsigned char[temp_string_index + 1];  // +1 for the null terminator
    std::memcpy(new_string, temp_string, temp_string_index + 1);  // Copy the temp string to the new string

    delete[] temp_string;  // Free the temporary buffer
    return new_string;  // Return the constructed string
}



LZ77::~LZ77() {
    // Destructor to free the dynamically allocated memory for ST
    //delete ST;
    delete hits;
    delete hashes;

}