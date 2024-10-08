//
// Created on 11/09/24.
// This is the code of counting version of Contextual pattern mining
// Input: a text $T$ of length $n = |T|$, a pattern $P$ of length $m = |P|$, and two integers $x$, $y$ where $x, y \geq 0$,
// Output: the frequency count of each distinct substring $XPY$ that occurs in $T$, where $|X|=x$, $|Y|=y$.
#include <cstdio>
#include <cmath>
#include <cstdlib>

#include <string>
#include <getopt.h>
#include <assert.h>
#include <sys/time.h>
#include <unordered_set>
#include "krfp.h"
#include <iostream>
#include <fstream>
#include "baseline.h"
#include "cmdline.h"
#include "counting.h"
#include "sparsePrefixTree.h"
#include "sparseSuffixTree.h"

#include "kd_tree.h"
#include <vector>
#include <malloc.h>
#include <algorithm>
#include "unordered_dense.h"
#define THRESHOLD 1500000
#ifdef _USE_64
#include <divsufsort64.h>                                         // include header for suffix sort
#endif

#ifdef _USE_32
#include <divsufsort.h>                                           // include header for suffix sort
#endif


#include "SA_LCP_LCE.h"



#include <sdsl/bit_vectors.hpp>					  // include header for bit vectors
#include <sdsl/rmq_support.hpp>					  //include header for range minimum queries
using namespace sdsl;

using namespace std;
#include <chrono>

long long memory_usage() {
    struct mallinfo2 mi = mallinfo2();
    return mi.uordblks + mi.hblkhd;
}



// Returns a new reversed string without modifying the original
unsigned char* reversePattern(unsigned char* &s) {
    INT length = strlen((char*) s);
    unsigned char* reversed_s = ( unsigned char * ) malloc (  ( length + 1 ) * sizeof ( unsigned char ) );


    for (INT i = 0; i < length; i++) {
        reversed_s[i] = s[length - 1 - i];
    }
    reversed_s[length] = '\0'; // Don't forget to null-terminate the new string

    return reversed_s; // Return the new dynamically allocated reversed string
}
unsigned char* reverseString(unsigned char* &s) {
    INT length = strlen((char*) s);
    unsigned char* reversed_s = ( unsigned char * ) malloc (  ( length + 1 ) * sizeof ( unsigned char ) );


    for (INT i = 0; i < length - 1; i++) {
        reversed_s[i] = s[length  - i -2];
    }
    reversed_s[length - 1] = '$'; // Don't forget to null-terminate the new string

    reversed_s[length] = '\0'; // Don't forget to null-terminate the new string

    return reversed_s; // Return the new dynamically allocated reversed string
}



void printArray(const char* name, INT* array, INT size) {
    std::cout << name << ": [";
    for (INT i = 0; i < size; i++) {
        std::cout << array[i];
        if (i < size - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

void printstring(const char* name, INT* array, INT size,  unsigned char* text) {
    std::cout << name << ":"<<endl;
    for (INT i = 0; i < size; i++) {
        for (INT j =array[i];j< size;j++){
            std::cout << text[j];
        }
        if (i < size - 1) {
            std::cout <<endl;
        }
    }
    std::cout << std::endl;
}

bool compareByFirstEle(pair<INT, INT>& p1, pair<INT, INT>& p2){
    return p1.first < p2.first;
}


void readfile(string &filename, string &patternPath, unsigned char * &text_string, std::vector<unsigned char *> &patterns,
              std::vector<std::pair<INT, INT>> &contextSizes, INT& text_size, vector<INT> & patternSizes){
    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;

    }


    is_text.seekg(0, std::ios::end);
    text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);




    text_string = (unsigned char*) malloc((text_size + 1 + 1) * sizeof(unsigned char));

    char c = 0;
    for (INT i = 0; i < text_size; i++) {
        is_text.read(reinterpret_cast<char*>(&c), 1);
        text_string[i] = (unsigned char)c;
    }


    is_text.close();
//    text_string[0] = '$';                      // start with '$'
    text_string[text_size] = '$';           // end with '$'
    text_string[text_size + 1] = '\0';          // null

    text_size = text_size +1;
//    text_size = text_size + 2 ;
    is_text.close();


    /* open pattern file and write it into pattern*/
    std::ifstream is_pattern(patternPath, std::ios::binary);
    if (!is_pattern) {
        std::cerr << "Error opening pattern file: " << patternPath << std::endl;
    }


    std::string line;
    while (std::getline(is_pattern, line)) {
        std::istringstream iss(line);
        std::string pattern_str;
        INT x;
        INT y;

        if (!(iss >> pattern_str >> x >> y)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue;  // Skip the invalid line
        }

        // Allocate memory for the pattern and store it in the vector
        unsigned char *pattern = (unsigned char *)malloc((pattern_str.size() + 1) * sizeof(unsigned char));
        std::copy(pattern_str.begin(), pattern_str.end(), pattern);
        pattern[pattern_str.size()] = '\0';  // null terminator
        patternSizes.push_back(pattern_str.size());
        patterns.push_back(pattern);
        contextSizes.push_back({x, y});
    }

    is_pattern.close();
}




void sample_positions(std::vector<uint64_t>* & ssa, std::vector<uint64_t>* & ssa_list_rev, INT n, INT c) {
    // k = round(log(n) / c) and make sure k>=1
    INT k = std::max(static_cast<uint64_t>(std::round(std::log(n) / c)), uint64_t(1));

    for (INT i = 0; i < n; i += k) {
        ssa->push_back(i);
        ssa_list_rev->push_back(n-i -1);
    }

//    std::cout << "Number of sampled suffixes: " << ssa->size() << std::endl;
}

/* Comparator */
auto compare(unsigned char * sequence, vector<uint64_t> * A, uint64_t lcp )
{
    return [sequence, A, lcp](uint64_t a, uint64_t b)
    {
        return sequence[ A->at(a)+lcp ] < sequence[ A->at(b)+lcp ];
    };
}


struct SSA
{
    uint64_t lcp;
    vector<uint64_t> L;
    SSA() : lcp(0), L() {}
};

/* Compute the KR fingerprint of sequence[ssa..ssa+l-1] using the FP table -- Time is O(min(l,n/s)), where s is the size of the FP table */
uint64_t fingerprint( uint64_t ssa, uint64_t * FP, uint64_t fp_len, uint64_t l, unsigned char * sequence, uint64_t text_size, uint64_t power )
{
    uint64_t fp = 0;
    uint64_t ssa_end = (text_size >= ssa+l) ? ssa+l : text_size; //this is the end of the substring we are interested in PLUS 1
    bool subtract_slow = (text_size >= ssa+l) ? false : true;

    if( l > fp_len )
    {
        uint64_t fp_short = 0;
        if( ssa == 0 ) // then there is not short fragment before the substring
        {
            ; // do nothing
        }
        else if( ssa % fp_len  !=  0 ) // we are in-between two stored prefixes
        {
            uint64_t prefix = ssa / fp_len; // this is the prefix we are in but we have something stored on the left
            uint64_t start = 0;

            if( prefix != 0 )
            {
                fp_short = FP[prefix - 1];
                start = prefix * fp_len;
            }

            for(uint64_t i = start; i<ssa; i++)	fp_short = karp_rabin_hashing::concat( fp_short, sequence[i], 1);

        }
        else 	// we have the fp_short stored and we read it from FP
        {
            uint64_t prefix = ssa / fp_len;
            fp_short = FP[prefix - 1];
        }

        uint64_t fp_long = 0;

        if( ssa_end % fp_len  != 0 )
        {
            uint64_t prefix = ssa_end / fp_len; // this is the prefix we are in but we have something stored on the left
            uint64_t start = 0;

            if( prefix != 0 )
            {
                fp_long = FP[prefix - 1];
                start = prefix * fp_len;
            }
            for(uint64_t i = start; i< ssa_end; i++)	fp_long = karp_rabin_hashing::concat( fp_long, sequence[i] , 1 );
        }
        else
        {
            uint64_t prefix = ssa_end / fp_len;
            fp_long = FP[prefix - 1];
        }

        if( subtract_slow == false )
            fp = karp_rabin_hashing::subtract_fast(fp_long, fp_short, power);
        else fp = karp_rabin_hashing::subtract(fp_long, fp_short, ssa_end - ssa);

    }
    else
    {
        for(uint64_t i=ssa; i< ssa_end; ++i)	fp =  karp_rabin_hashing::concat( fp, sequence[i], 1 );
    }

    return fp;
}



/* Extend the prefixes of grouped suffixes by length l and re-group the computed KR fingerprints -- Time is O(b.min(l,n/s)), where s is the size of the FP table */
uint64_t group( vector<SSA> &B, vector<uint64_t> * A, uint64_t * FP, uint64_t fp_len, uint64_t l, unsigned char * sequence, uint64_t text_size, uint64_t b, uint64_t &m, const uint64_t &z, uint64_t hash_variable )
{
    vector<SSA> * B_prime = new vector<SSA>();
    (*B_prime).reserve(b);

    uint64_t power = karp_rabin_hashing::pow_mod_mersenne(hash_variable, l, 61);

    const auto Bsz = B.size();

    for(uint64_t i = 0; i<Bsz; ++i )
    {
        const uint64_t s = (B)[i].L.size();
        uint64_t k = 0;
        vector<vector<uint64_t>> vec;
        vector<uint64_t> tmp;

        if( s <= (const uint64_t)z )
        {

            vector<pair<uint64_t,uint64_t> > vec_to_sort;
            for(auto it=(B)[i].L.begin();it!=(B)[i].L.end(); ++it)
            {
                if ( (*A)[(*it)]+(B)[i].lcp + l > text_size ) { tmp.push_back((*it)); continue; }
                uint64_t fp = fingerprint( (*A)[(*it)]+(B)[i].lcp, FP, fp_len, l, sequence, text_size, power );
                vec_to_sort.push_back( make_pair(fp,*it) );
            }

            sort(vec_to_sort.begin(),vec_to_sort.end());


            const auto vsz=vec_to_sort.size();
            for(uint64_t i=0;i<vsz;++i)
            {
                if (i == 0 || (i>0 && vec_to_sort[i].first != vec_to_sort[i - 1].first)) //if we have new group
                {
                    vector<uint64_t> new_vec;
                    new_vec.push_back(vec_to_sort[i].second);
                    vec.push_back(new_vec);	 //adds into vec a new vector
                    k++;
                }
                else	vec.back().push_back(vec_to_sort[i].second); //adds the id into the last added vector in vec
            }

        }
        else
        {

            auto groups = ankerl::unordered_dense::map<uint64_t, uint64_t >();
            for(auto it=(B)[i].L.begin();it!=(B)[i].L.end(); ++it)
            {
                if ( (*A)[(*it)]+(B)[i].lcp + l > text_size ) { tmp.push_back((*it)); continue; }
                uint64_t fp = fingerprint( (*A)[(*it)]+(B)[i].lcp, FP, fp_len, l, sequence, text_size, power );
                auto itx = groups.find(fp);
                if(itx == groups.end())
                {
                    groups[fp] = k;
                    vector<uint64_t> v;
                    v.push_back(*it);
                    vec.push_back(v);
                    k++;
                }
                else	vec[itx->second].push_back(*it);
            }
            groups.clear();

        }



        vector<uint64_t>().swap(B[i].L);

        for( uint64_t j = 0; j < k; j++ )
        {
            const auto itsz=vec[j].size();

            if( itsz == s )
            {
                (B)[i].lcp += l;
                for(const auto& value: vec[j])	(B)[i].L.push_back(value);

            }
            else if( itsz >= 2 )
            {
                m++;
                SSA new_ssa;

                for(const auto& value: vec[j])	new_ssa.L.push_back(value);

                new_ssa.lcp = (B)[i].lcp + l;
                B_prime->push_back( new_ssa );
                (B)[i].L.push_back(m);

                A->push_back( (*A)[ vec[j][0] ] );
            }
            else if ( itsz == 1 )
            {
                (B)[i].L.push_back( vec[j][0] );
            }
        }
        for (auto& it : tmp) (B)[i].L.push_back( it );
        vector<uint64_t>().swap(tmp);
        vector<vector<uint64_t>>().swap(vec);


    }


    B.insert(std::end(B), std::begin(*B_prime), std::end(*B_prime));
    delete( B_prime);


    return 0;
}

/* Sort the final group members and infer the SSA and SLCP array -- Time is O(b log b) */
uint64_t order( vector<uint64_t> * final_ssa, vector<uint64_t> * final_lcp, vector<SSA> &B, vector<uint64_t> * A, unsigned char * sequence, uint64_t text_size, uint64_t b )
{

    const uint64_t Bsz=B.size();
    for(uint64_t i = 0; i<Bsz; i++)
    {
#if IPS4 == true
        ips4o::sort((B)[i].L.begin(), (B)[i].L.end(), compare(sequence,A,(B)[i].lcp));
#else
        sort((B)[i].L.begin(), (B)[i].L.end(), compare(sequence,A,(B)[i].lcp));
#endif
    }
    stack<pair<uint64_t,uint64_t>> S;

    S.push( make_pair<uint64_t, uint64_t>((uint64_t)b, 0) );  //b is the correct first index, not b+1

    uint64_t l = 0;
    uint64_t mymax=numeric_limits<uint64_t>::max();
    while( !S.empty() )
    {
        uint64_t i = S.top().first;
        uint64_t l_prime = S.top().second;
        S.pop();

        if( l_prime < l )
            l = l_prime;

        if(i>=b) //it is not one of the initial groups
        {
            uint64_t lcp = (B)[i-b].lcp;
            auto myl=(B)[i-b].L;
            for(vector<uint64_t>::reverse_iterator it=myl.rbegin();it!=myl.rend();++it)
            {
                S.push(make_pair<uint64_t, uint64_t>( (uint64_t) *it, (uint64_t) lcp ));
            }
        }
        else
        {
            final_ssa->push_back( (*A)[i] );
            final_lcp->push_back( l );
            l = mymax;
        }
    }

    return 0;
}



void PA(std::vector<uint64_t>* & final_ssa, std::vector<uint64_t>* & final_lcp,vector<uint64_t> *& ssa_list, unsigned char * &text_string,INT &text_size){

    INT hash_variable = karp_rabin_hashing::init();
    INT b = ssa_list->size();
//    cout<<"Number of suffixes b = " << b << endl;
    INT s = 2*b;
    if ( s > text_size ) s = text_size;
    INT fp_len = text_size / s;
    if ( (text_size - fp_len * s) > text_size/s )
        s = text_size/fp_len;
//    cout<<"Block length = "<<fp_len<<endl;
//    cout<<"Size s of FP table = " << s <<endl<<endl;

    // computing fingerprints
    uint64_t * FP =  ( uint64_t * ) calloc( s , sizeof( uint64_t ) );
    uint64_t fp = 0;


    uint64_t i = 0;
    for(uint64_t j = 0; j < s; ++j )
    {
        for (uint64_t k = 0; k < fp_len; k ++ )	fp = karp_rabin_hashing::concat(fp, text_string[i+k], 1);
        FP[j]=fp;
        i += fp_len;
        if ( i + fp_len > text_size ) break;
    }



    vector<SSA> B;

    vector<uint64_t> * A = new vector<uint64_t>();

    vector<uint64_t> * A_prime = new vector<uint64_t>();
    vector<uint64_t> * P = new vector<uint64_t>();

    vector<uint64_t> L;

    const auto ssa_list_sz=ssa_list->size();
    for(uint64_t i = 0; i<ssa_list_sz; ++i )
    {
        A->push_back( (*ssa_list)[i] );
        L.push_back(i);
    }

    SSA initial;
    initial.lcp = 0;

    initial.L.assign(L.begin(), L.end());

    B.push_back( initial );

    A->push_back( (*ssa_list)[0] );
    uint64_t m = ssa_list->size();

    uint64_t c1 = 1;
    uint64_t initial_l = 1ULL << static_cast<uint64_t>(log2(c1*text_size/b));
    uint64_t next_initial_l = initial_l * 2 - 1;


//    cout<<"First run starts"<<endl;
    while( initial_l > 0 )
    {
//        cout<< "Initial l: " << initial_l <<", nodes: "<< m <<endl;
        group( B, A, FP, fp_len, initial_l, text_string, text_size, b, m, THRESHOLD, hash_variable );
        initial_l=initial_l>>1;
    }



    order( final_ssa, final_lcp, B, A, text_string, text_size, b);

//    cout<<"First run ends"<<endl<<endl;

    for(uint64_t i = 0; i<b; i++)
    {

        if( (*final_lcp)[i] == next_initial_l || ( i < b-1 && (*final_lcp)[i+1] == next_initial_l ) )
        {
            P->push_back(i);
            A_prime->push_back((*final_ssa)[i]);
        }
    }


    vector<uint64_t> * final_ssa_prime = new vector<uint64_t>();
    vector<uint64_t> * final_lcp_prime = new vector<uint64_t>();

    if( P->size() > 0 )
    {

        uint64_t l = 1ULL << static_cast<uint64_t>(log2(text_size));
        vector<SSA>().swap(B);

        b = A_prime->size();
        m = A_prime->size();

        vector<uint64_t> L(m);
        std::iota(L.begin(), L.end(), 0);

        SSA initial;
        initial.lcp = 0;
        initial.L.assign(L.begin(), L.end());

        B.push_back( initial );
        A_prime->push_back( (*A_prime)[0] );

        while( l > 0 )
        {
            group( B, A_prime, FP, fp_len, l, text_string, text_size, b, m, THRESHOLD, hash_variable );
            l=l>>1;
        }

        order( final_ssa_prime, final_lcp_prime, B, A_prime, text_string, text_size, b);



        for(uint64_t i = 0; i<P->size(); ++i)
        {
            (*final_ssa) [(*P)[i] ] = (*final_ssa_prime)[i];
            if( (*final_lcp)[ (*P)[i] ]== next_initial_l )
                (*final_lcp)[ (*P)[i] ] = (*final_lcp_prime)[i];
        }

    }

    free( FP );
    delete( final_lcp_prime );
    delete( final_ssa_prime );
    delete( A );
    delete( A_prime );
    delete( P );

}



int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");

    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
//    parser.add<string>("Suffix", 's', "the path to suffix file", false, "/home/ling/OneDrive/PhD/CPM/SSA/data/suffixes/suffixes1");

    parser.add<int>("LengthofP", 'm', "the length of P, m = |P| ", false, 1);
    parser.add<int>("SampleRatio", 'c', "sample a suffix every log(n) / c distance, c: bigger, sampler more", false, 1);


    parser.parse_check(argc, argv);




    std::string filename = parser.get<std::string>("filepath");
    std::string patternPath = parser.get<std::string>("pattern");
    INT c = parser.get<int>("SampleRatio");


    /* readfile into text_string and pattern */
    unsigned char* text_string;
    std::vector<unsigned char *> patterns;
    std::vector<std::pair<INT, INT>> contextSizes;

    INT text_size = 0;

    vector<INT> patternSizes;

    readfile(filename,patternPath,text_string, patterns, contextSizes, text_size, patternSizes);
    /*reversed string*/

    unsigned char* text_string_rev = reverseString(text_string);



    // sample positions
    vector<uint64_t> * ssa_list = new vector<uint64_t>();
    vector<uint64_t> * ssa_list_rev = new vector<uint64_t>();

    sample_positions(ssa_list,ssa_list_rev,text_size,c);


    /* Read in list of sparse suffixes */
//    ifstream suff_list(parser.get<std::string>("Suffix"), ios::in | ios::binary);
//    suff_list.seekg(0, ios::end);
//    uint64_t file_size = suff_list.tellg();
//
//    c = 0;
//    string line="";
//    suff_list.seekg (0, ios::beg);
//
//    if( file_size == 0 )
//    {
//        cout<<"Empty suffixes list"<<endl;
//        return 1;
//    }
//
//    for (uint64_t i = 0; i < file_size; i++)
//    {
//        suff_list.read(reinterpret_cast<char*>(&c), 1);
//
//        if( ( char) c == '\n' || ( char) c == ' ' )
//        {
//            ssa_list->push_back(stoull(line));
//
//            if( stoull(line) >= text_size )
//            {
//                cout<<"Suffix list contains a suffix larger than sequence length"<<endl;
//                return 1;
//            }
//
//            line = "";
//
//        }
//        else line += c;
//    }
//    suff_list.close();





    uint64_t b = ssa_list->size();
//    cout<<"Number of suffixes b = " << b << endl;


// Construct Sparse SA and LCP

    vector<uint64_t> * final_ssa = new vector<uint64_t>();
    vector<uint64_t> * final_lcp = new vector<uint64_t>();

    PA(final_ssa, final_lcp,ssa_list,text_string,text_size);


    // Construct Sparse SA and LCP for reversed string

    vector<uint64_t> * final_ssa_rev = new vector<uint64_t>();
    vector<uint64_t> * final_lcp_rev = new vector<uint64_t>();

    PA(final_ssa_rev, final_lcp_rev,ssa_list_rev,text_string_rev,text_size);



    int_vector<> v1( b , 0 ); // create a vector of length n and initialize it with 0s


    for ( INT i = 0; i < b; i ++ )
    {
        v1[i] = (*final_lcp)[i];
    }

    rmq_succinct_sct<> rmq(&v1);

    util::clear(v1);



    int_vector<> v2( b , 0 ); // create a vector of length n and initialize it with 0s


    for ( INT i = 0; i < b; i ++ )
    {
        v2[i] = (*final_lcp_rev)[i];
    }

    rmq_succinct_sct<> rmq_rev(&v2);

    util::clear(v2);

    std::vector<Point> pointsD1;
    std::vector<Point> pointsD2;

    std::unordered_map<INT, KDTree*> preorderID2KDTree;



    sparseSuffixTree SST(final_ssa, final_lcp, text_string, text_size);

    SST.initHLD();



//
//    cout<<"ST.lightNodes.size(): " <<SST.lightNodes.size()<<endl;
//    cout<< "text_size: "<<text_size<<endl;


    std::unordered_map<INT, INT> inv_final_ssa;
    for (int i = 0; i < final_ssa->size(); ++i) {
        inv_final_ssa[(*final_ssa)[i]] = i;
    }
    std::unordered_map<INT, INT> inv_final_ssa_rev;
    for (int i = 0; i < final_ssa_rev->size(); ++i) {
        inv_final_ssa_rev[(*final_ssa_rev)[i]] = i;
    }


    INT cnt =0;
    for (auto& lightNode: SST.lightNodes){
//        cout<<cnt<<endl;
        cnt++;
        std::vector<Point> pointsDl;
        vector <pair<INT, INT>> vec_L; // the idx in \overline(SA), phi
        INT L = lightNode->heavyLeaf->start;
        INT phi =0;
        for (auto &leaf: lightNode->leaves){

            INT R = leaf->start;
            if (L != R){

                INT low = min(inv_final_ssa[L], inv_final_ssa[R]);
                INT high = max(inv_final_ssa[L], inv_final_ssa[R]);

                phi = (*final_lcp)[rmq(low + 1, high)];

            } else{

                phi = text_size - L;

            }

            vec_L.push_back({inv_final_ssa_rev[text_size - leaf->start-1], phi});

        }
        sort(vec_L.begin(),vec_L.end(), compareByFirstEle);
        std::vector<pair<INT,INT>> prefixesStarting;
        for (auto& id : vec_L) {
            //pair {starting, phi}
            prefixesStarting.push_back({(*final_ssa_rev)[id.first],id.second});
        }
        if (!prefixesStarting.empty()){
            sparsePrefixTree SPT (prefixesStarting,text_string_rev, text_size,inv_final_ssa_rev,final_lcp_rev,rmq_rev);
            SPT.updatePhi();
            SPT.addPoints(pointsD1,pointsD2, pointsDl,lightNode);

            KDTree* KD_Dl = new KDTree(pointsDl);


            preorderID2KDTree[lightNode->preorderId] = KD_Dl;

        }
    }




    long long KD_start = memory_usage();


    KDTree KD_D1(pointsD1);
//    cout<<pointsD1.size()<<endl;
//    cout<<"KD1"<<endl;
    KDTree KD_D2(pointsD2);
//    cout<<pointsD2.size()<<endl;

//    cout<<"KD2"<<endl;

    long long KD_end = memory_usage();

    long long memory_KD = KD_end- KD_start;





// clear memory

    pointsD1.clear();
    pointsD2.clear();
    pointsD1.shrink_to_fit();
    pointsD2.shrink_to_fit();

    SST.lightNodes.clear();

    SST.clearLeaves();



    auto Construction_end = std::chrono::high_resolution_clock::now();


    cout<<"n:" <<text_size <<"; sample size: "<<b <<endl;

    for (INT i =0; i<patterns.size(); i ++){

        INT x = contextSizes[i].first;

        INT y = contextSizes[i].second;

        unsigned char* pattern_rev = reversePattern(patterns[i]);

        auto query_start = std::chrono::high_resolution_clock::now();

        sstNode * up = SST.forward_search(patterns[i], patternSizes[i]);
        // exit if the u_p is NULL
        if (!up){

            std::cout << "Text string: " << text_string << std::endl;
            std::cout << "Pattern: " << patterns[i]<< std::endl;
            std::cout << "x: " << x << std::endl;

            std::cout << "y: " << y << std::endl;

            std::cerr << "The pattern does not exist in text string!" << std::endl;
            free(pattern_rev);
            break;
        }

        INT counts=0;

        if (up->heavy){

            //D_1

            // find the preorder ID of rightmost leaf
            INT rightpreorderId = up->preorderId;
            sstNode * current_up = up;

            while (!current_up->child.empty()){
                sstNode * rightChild = current_up->child.rbegin()->second;
                rightpreorderId = rightChild->preorderId;
                current_up = rightChild;
            }


            vector<pair<double, double>> ranges_D1 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                      {0,(double) patternSizes[i] +y},{(double) patternSizes[i] +y, (double) text_size}};

            INT result_D1 = KD_D1.rangeSearch(ranges_D1);

#ifdef VERBOSE
            for (const Point& pt : result_D1) {
            for (double coord : pt.coords) {
                cout << coord << " ";
            }
            cout << endl;
        }
        cout<<"---------------------------------------"<<endl;
#endif
            //D_2



            vector<pair<double, double>> ranges_D2 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                      {0,(double) patternSizes[i] +y},{(double) patternSizes[i] +y, (double) text_size}};

            INT result_D2 = KD_D2.rangeSearch(ranges_D2);

#ifdef VERBOSE
            for (const Point& pt : result_D2) {
            for (double coord : pt.coords) {
                cout << coord << " ";
            }
            cout << endl;
        }
        cout<<"---------------------------------------"<<endl;

#endif
            //D_l


            // find the u_l if u_p is heavy (D_l)
            sstNode * lowest_ul = up;
            while (lowest_ul->parent->heavy){
                lowest_ul = lowest_ul->parent;
            }
            lowest_ul = lowest_ul->parent;


            vector<pair<double, double>> ranges_Dl = {{0, (double) x}, {(double) x,(double) text_size}, { (double) patternSizes[i] + y, (double) text_size}};

            INT result_Dl = preorderID2KDTree[lowest_ul->preorderId]->rangeSearch(ranges_Dl);
#ifdef VERBOSE

            for (const Point& pt : result_Dl) {
            for (double coord : pt.coords) {
                cout << coord << " ";
            }
            cout << endl;
        }
        cout<<"---------------------------------------"<<endl;


#endif
            counts = result_D1 + result_D2 + result_Dl;

        } else{



            // find the preorder ID of rightmost leaf
            INT rightpreorderId = up->preorderId;
            sstNode * current_up = up;

            while (!current_up->child.empty()){
                sstNode * rightchild = current_up->child.rbegin()->second;
                rightpreorderId = rightchild->preorderId;
                current_up = rightchild;
            }



            vector<pair<double, double>> ranges_D1 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                      {0,(double) patternSizes[i] +y},{(double) patternSizes[i] +y, (double) text_size}};

            INT result_D1 = KD_D1.rangeSearch(ranges_D1);
#ifdef VERBOSE
            for (const Point& pt : result_D1) {
            for (double coord : pt.coords) {
                cout << coord << " ";
            }
            cout << endl;
        }
        cout<<"---------------------------------------"<<endl;
#endif

            //D_2


            vector<pair<double, double>> ranges_D2 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                      {0,(double) patternSizes[i] +y},{(double) patternSizes[i] +y, (double) text_size}};

            INT result_D2 = KD_D2.rangeSearch(ranges_D2);


            counts = result_D1 + result_D2;

#ifdef VERBOSE

            for (const Point& pt : result_D2) {
            for (double coord : pt.coords) {
                cout << coord << " ";
            }
            cout << endl;
        }
        cout<<"---------------------------------------"<<endl;
#endif
        }


        auto query_end = std::chrono::high_resolution_clock::now();
        double query_time = std::chrono::duration_cast < std::chrono::microseconds > (query_end - query_start).count()*0.000001;




        cout<<"There are "<<counts<< " distinct XPY that occur in T."<<endl;

        cout<<"Time for query of counting: "<<query_time<<endl;

        free(pattern_rev);

    }






    for (auto& pair : preorderID2KDTree) {
        delete pair.second;
    }


    free(text_string);
    free(text_string_rev);
    for (auto &it: patterns){
        free(it);
    }

    delete( ssa_list );
    delete( final_lcp );
    delete( final_ssa );

    delete( ssa_list_rev );
    delete( final_lcp_rev );
    delete( final_ssa_rev );


}