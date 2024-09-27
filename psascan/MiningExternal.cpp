
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <sys/time.h>
#include "cmdline.h"
#include "baseline.h"
#include <unordered_set>
#include "stream.h"
#include "uint40.h"


#include <stxxl/sorter>
#include <stxxl/stats>
#include <stxxl/timer>
#include <stxxl/random>
#include <limits>
#ifdef _USE_64
#include <divsufsort64.h>                                         // include header for suffix sort
#endif

#ifdef _USE_32
#include <divsufsort.h>                                           // include header for suffix sort
#endif






#include <sdsl/bit_vectors.hpp>					  // include header for bit vectors
#include <sdsl/rmq_support.hpp>					  //include header for range minimum queries
//#include "stack.h"        					  // include header for stack structure

using namespace sdsl;
using namespace std;


#include <iostream>

#include <malloc.h>
long long memory_usage() {
    struct mallinfo2 mi = mallinfo2();
    return mi.uordblks + mi.hblkhd;
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


/* Reversing a string in place in linear time */
void reverse( unsigned char * &s )
{
    INT length = strlen( ( char * ) s ) ;
    unsigned char temp = ' ';
    INT i = 0, j = length-1;

    while( i < j )
    {
        temp = s[j];
        s[j] = s[i];
        s[i] = temp;
        i++; j--;
    }
}


// Returns a new reversed string without modifying the original
unsigned char* reverseReturn(unsigned char* &s) {
    INT length = strlen((char*) s);
    unsigned char* reversed_s = ( unsigned char * ) malloc (  ( length + 1 ) * sizeof ( unsigned char ) );


    for (INT i = 0; i < length; i++) {
        reversed_s[i] = s[length - 1 - i];
    }
    reversed_s[length] = '\0'; // Don't forget to null-terminate the new string

    return reversed_s; // Return the new dynamically allocated reversed string
}


unsigned int LCParray ( unsigned char *text, INT n, INT * SA, INT * ISA, INT * LCP )
{
    INT i=0, j=0;

    LCP[0] = 0;
    for ( i = 0; i < n; i++ ) // compute LCP[ISA[i]]
        if ( ISA[i] != 0 )
        {
            if ( i == 0) j = 0;
            else j = (LCP[ISA[i-1]] >= 2) ? LCP[ISA[i-1]]-1 : 0;
            while ( text[i+j] == text[SA[ISA[i]-1]+j] )
                j++;
            LCP[ISA[i]] = j;
        }

    return ( 1 );
}


/* Computes the length of lcp of two suffixes of two strings */
INT lcp ( unsigned char *  x, INT M, unsigned char * y, INT l, INT a_size, INT w_size )
{
    INT xx = a_size;
    if ( M >= xx ) return 0;
    INT yy = w_size;
    if ( l >= yy ) return 0;

    INT i = 0;
    while ( ( M + i < xx ) && ( l + i < yy ) )
    {
        if ( x[M+i] != y[l+i] )	break;
        i++;
    }
    return i;
}




/* Searching a list of strings using LCP from "Algorithms on Strings" by Crochemore et al. Algorithm takes O(m + log n), where n is the list size and m the length of pattern */
pair<INT,INT> pattern_matching ( unsigned char *  p, unsigned char *  T, INT * SA, INT * LCP, rmq_succinct_sct<> &rmq, INT n, INT p_size )
{

    INT m = p_size; //length of pattern
    INT N = n; //length of string
    INT d = -1;
    INT ld = 0;
    INT f = n;
    INT lf = 0;

    pair<INT,INT> interval;

    while ( d + 1 < f )
    {

        INT i = (d + f)/2;

        /* lcp(i,f) */
        INT lcpif;

        if( f == n )
            lcpif = 0;
        else lcpif = LCP[rmq ( i + 1, f ) ];

        /* lcp(d,i) */
        INT lcpdi;

        if( i == n )
            lcpdi = 0;
        else lcpdi = LCP[rmq ( d + 1, i ) ];

        if ( ( ld <= lcpif ) && ( lcpif < lf ) )
        {
            d = i;
            ld = lcpif;
        }
        else if ( ( ld <= lf ) && ( lf < lcpif ) ) 	f = i;
        else if ( ( lf <= lcpdi ) && ( lcpdi < ld ) )
        {
            f = i;
            lf = lcpdi;
        }
        else if ( ( lf <= ld ) && ( ld < lcpdi ) )	d = i;
        else
        {
            INT l = std::max (ld, lf);
            l = l + lcp ( T, SA[i] + l, p, l, n, p_size );
            if ( l == m ) //lower bound is found, let's find the upper bound
            {
                INT e = i;
                while ( d + 1 < e )
                {
                    INT j = (d + e)/2;

                    /* lcp(j,e) */
                    INT lcpje;

                    if( e == n )
                        lcpje = 0;
                    else lcpje = LCP[rmq ( j + 1, e ) ];

                    if ( lcpje < m ) 	d = j;
                    else 			e = j;
                }

                /* lcp(d,e) */
                INT lcpde;

                if( e == n )
                    lcpde = 0;
                else lcpde = LCP[rmq ( d + 1, e ) ];

                if ( lcpde >= m )	d = std::max (d-1,( INT ) -1 );

                e = i;
                while ( e + 1 < f )
                {
                    INT j = (e + f)/2;

                    /* lcp(e,j) */
                    INT lcpej;

                    if( j == n )
                        lcpej = 0;
                    else lcpej = LCP[rmq ( e + 1, j ) ];

                    if ( lcpej < m ) 	f = j;
                    else 			e = j;
                }

                /* lcp(e,f) */
                INT lcpef;

                if( f == n )
                    lcpef = 0;
                else lcpef = LCP[rmq ( e + 1, f ) ];

                if ( lcpef >= m )	f = std::min (f+1,n);

                interval.first = d + 1;
                interval.second = f - 1;
                return interval;


            }
            else if ( ( l == N - SA[i] ) || ( ( SA[i] + l < N ) && ( l != m ) && ( T[SA[i]+l] < p[l] ) ) )
            {
                d = i;
                ld = l;
            }
            else
            {
                f = i;
                lf = l;
            }
        }
    }


    interval.first = d + 1;
    interval.second = f - 1;
    return interval;
}




struct Range {
    INT s, e;
    Range(INT start, INT end) : s(start), e(end) {}
};


void findSubIntervalsRMQ(INT s, INT e, INT t, INT * LCP, rmq_succinct_sct<> &rmq, std::vector<std::pair<INT, INT>> &subIntervals) {
    //s: starting; e: ending, t: m + l, where m is |P|, l = |X| =|Y|


    if (s > e){

        fprintf(stderr, " Error: starting position is bigger than ending position!\n" );
        exit( EXIT_FAILURE );
    }
    //P only one occurrence

    if (s < e){
        stack<Range> ranges;
        ranges.push(Range(s, e));
        vector<INT> breakPoints;

        while (!ranges.empty()) {
            Range range = ranges.top();
            ranges.pop();

            if (range.s > range.e)
            {

                fprintf(stderr, " Error: starting position is bigger than ending position!\n" );
                exit( EXIT_FAILURE );
            }
//            if (range.s == range.e) {
//                continue;
//            }

            INT p = rmq(range.s, range.e);
            if (LCP[p] < t) {
                if (range.s < p) {
                    ranges.push(Range(range.s, p - 1));
                }

                if (p < range.e){
                    ranges.push(Range(p + 1, range.e));
                }

                breakPoints.push_back(p);
            }

        }
        sort(breakPoints.begin(),breakPoints.end());
        INT current_start = s;
        // get all subintervals
        for (INT i = 1; i < breakPoints.size(); ++i) {
            INT current_breakpoint = breakPoints[i];

            // Create an interval from the current start to the current breakpoint
            subIntervals.push_back({current_start, current_breakpoint-1});

            // Update the start for the next interval, which begins right after the current breakpoint
            current_start = current_breakpoint;
        }

        // If the last breakpoint is not the end, add the final interval
        if (current_start < e+1) {
            subIntervals.push_back({current_start,e});
        }

    } else{
        subIntervals.push_back({s,e});
    }



}

void findSubIntervalsScaning(INT s, INT e, INT t, INT * LCP, std::vector<std::pair<INT, INT>> &subIntervals) {
    //s: starting; e: ending, t: m + l, where m is |P|, l = |X| =|Y|
    if (s > e){
        fprintf(stderr, " Error: starting position is bigger than ending position!\n" );
        exit( EXIT_FAILURE );

    }

    //P only one occurrence
    if (s < e){
        INT current_start = s;

        for (INT i = s + 1; i < e +1; ++i) {
            // Check if the LCP value at this position is less than t (i.e., m+l)
            if (LCP[i] < t) {
                // If we are not already at the start of a new interval, end the current one
                if (current_start < i) {
                    subIntervals.push_back({current_start, i - 1});
                }
                // Start a new interval
                current_start = i;
            }
        }

        // add the last interval if it ends at the 'end'
        if (current_start < e +1) {
            subIntervals.push_back({current_start, e});
        }
    } else{
        subIntervals.push_back({s,e});


    }



}




struct TwoInteger
{
    int i, j;

    TwoInteger()
    { }

    TwoInteger(int _i, int _j)
            : i(_i), j(_j)
    { }
};

struct TwoIntegerComparator
{
    bool operator () (const TwoInteger& a, const TwoInteger& b) const
    {
        return a.i < b.i;
    }

    TwoInteger min_value() const
    {
        return TwoInteger(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    }

    TwoInteger max_value() const
    {
        return TwoInteger(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    }
};




int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");
    parser.add<string>("index", 'i', "the name of index", false, "index");
//    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
    parser.add<int>("X", 'x', "the length of context, |X| = x ", false, 1);
    parser.add<int>("Y", 'y', "the length of context, |Y| = y ", false, 1);
    parser.add<int>("RAM", 'r', "the usage of RAM", false, 1);
    parser.add<int>("LengthofP", 'm', "the length of P, m = |P| ", false, 1);


    // Get the variables from the cmd
    parser.parse_check(argc, argv);

    std::string filename = parser.get<std::string>("filepath");
    std::string index_name = parser.get<std::string>("index");
    INT x = parser.get<int>("X");
    INT y = parser.get<int>("Y");
    INT ram_use = parser.get<int>("RAM");
    INT m = parser.get<int>("LengthofP");
//    INT tau = parser.get<int>("MiningThreshold");



    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;

    }


    is_text.seekg(0, std::ios::end);
    INT text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);




    unsigned char * text_string = (unsigned char*) malloc((text_size + 1) * sizeof(unsigned char));

    char c = 0;
    for (INT i = 0; i < text_size; i++) {
        is_text.read(reinterpret_cast<char*>(&c), 1);
        text_string[i] = (unsigned char)c;
    }


    is_text.close();
//    text_string[0] = '$';                      // start with '$'
    text_string[text_size] = '\0';          // null



    char * output_reverse;
    const char * reversed_text = "_reverse";

    output_reverse = (char *) malloc(strlen(filename.c_str())+9);
    strcpy(output_reverse, filename.c_str());
    strcat(output_reverse, reversed_text);

    std::ofstream output_r;
    output_r.open (output_reverse);

    reverse(text_string);
    output_r << text_string;
    output_r.close();


    string sa_fname = index_name + "_SA.sa5";


    ifstream in_SA(sa_fname, ios::binary);

    if(!(in_SA))
    {

        char commandesa[ sa_fname.length() + 1000 ];
        char * fullpathstart = dirname(realpath(argv[0], NULL));
        char command1[ sa_fname.length() + 1000 ];
        strcpy(command1, fullpathstart);
        strcat(command1, "/psascan/construct_sa %s -m %ldMi -o %s");
        sprintf(commandesa, command1, filename.c_str(), ram_use, sa_fname.c_str());
        int outsa=system(commandesa);

    }


    string lcp_fname = index_name + "_LCP.lcp5";
    ifstream in_LCP(lcp_fname, ios::binary);

    if( !(in_LCP ) )
    {
        char commande[ sa_fname.length() + lcp_fname.length() + 1000];
        char * fullpathstart = dirname(realpath(argv[0], NULL));
        char command2[ sa_fname.length() + lcp_fname.length()  + 1000];
        strcpy(command2, fullpathstart);
        strcat(command2, "/EM-SparsePhi-0.2.0/construct_lcp_parallel -m %ldMi -o %s -s %s %s");
        sprintf(commande, command2, ram_use, lcp_fname.c_str(), sa_fname.c_str(), filename.c_str());
        int out=system(commande);
    }

    string sa_fname_reverse = index_name +"_reverse_SA.sa5";
    ifstream in_SA_reverse(sa_fname_reverse, ios::binary);

    if ( !(in_SA_reverse)  )
    {
        char commandesa_reverse[ sa_fname_reverse.length() + 1000 ];
        char * fullpathstart_reverse = dirname(realpath(argv[0], NULL));
        char command1_reverse[ sa_fname_reverse.length() + 1000 ];
        strcpy(command1_reverse, fullpathstart_reverse);
        strcat(command1_reverse, "/psascan/construct_sa %s -m %ldMi -o %s");
        sprintf(commandesa_reverse, command1_reverse, output_reverse, ram_use, sa_fname_reverse.c_str());
        int outsa_reverse=system(commandesa_reverse);
    }



    string lcp_fname_reverse = index_name + "_reverse_LCP.lcp5";
    ifstream in_LCP_reverse(lcp_fname_reverse, ios::binary);

    if( !(in_LCP_reverse ) )
    {
        char commande_reverse[ sa_fname_reverse.length() + lcp_fname_reverse.length() + 1000];
        char * fullpathstart_reverse = dirname(realpath(argv[0], NULL));
        char command2_reverse[ sa_fname_reverse.length() + lcp_fname_reverse.length() + 1000];
        strcpy(command2_reverse, fullpathstart_reverse);
        strcat(command2_reverse, "/EM-SparsePhi-0.2.0/construct_lcp_parallel -m %ldMi -o %s -s %s %s");
        sprintf(commande_reverse, command2_reverse, ram_use, lcp_fname_reverse.c_str(), sa_fname_reverse.c_str(), output_reverse);
        int out_reverse=system(commande_reverse);
    }




    stream_reader<uint40>* SA =  new stream_reader <uint40> (sa_fname, ram_use);
    stream_reader<uint40>* LCP =  new stream_reader <uint40> (lcp_fname, ram_use);
    INT prevSA = 0;
    INT currSA = 0;
    INT currLCP = 0;

    for( INT i = 0; i < text_size; i++ ) // in lex order
    {

        /* If the ith lex suffix is an anchor then add it to the compacted trie (encoded in arrays RSA and RLCP) */
        prevSA = currSA;

        currLCP = LCP->read();

        cout<<currLCP<<endl;

    }
    cout<<"---------------------"<<endl;

    for( INT i = 0; i < text_size; i++ ) // in lex order
    {

        /* If the ith lex suffix is an anchor then add it to the compacted trie (encoded in arrays RSA and RLCP) */
        prevSA = currSA;
        currSA = SA->read();

        cout<<currSA<<endl;


    }


    typedef stxxl::sorter<TwoInteger, TwoIntegerComparator, 1*1024*1024> sorter_type;

    // create sorter object (CompareType(), MainMemoryLimit)
    sorter_type int_sorter(TwoIntegerComparator(), 1000 * 1024 * 1024);

    stxxl::random_number32 rand32;

    stxxl::timer Timer1;
    Timer1.start();

    // insert random numbers from [0,100000)
    for (size_t i = 0; i < atoi(argv[1]); ++i)
    {
        int_sorter.push(TwoInteger(rand32() % 100000, (int)i));    // fill sorter container
    }

    Timer1.stop();

    STXXL_MSG("push time: " << (Timer1.mseconds() / 1000));

    stxxl::timer Timer2;

    Timer2.start();
    int_sorter.sort();  // switch to output state and sort
    Timer2.stop();

    STXXL_MSG("sort time: " << (Timer2.mseconds() / 1000));

    // echo sorted elements
    while (!int_sorter.empty())
    {
        std::cout << int_sorter->i << " ";  // access value
        ++int_sorter;
    }



/*The following is the mining algorithm*/
//It outputs the distinct XPY, for every P of length m with at least one occurrence in the text








    // scan the LCP_rev from top to bottom
//    std::vector<std::pair<INT, INT>> IntervalsPreprocessed;
//    findSubIntervalsScaning(0, n-1, m, LCP_rev, IntervalsPreprocessed);
//
//
//
//
////  repeat scanning on every subInterval
//    std::vector<std::pair<INT, INT>> subsubIntervals_total;
//
//
//    std::vector<INT> index_array(n);
//
//    for (size_t i = 0; i < n; ++i) {
//        index_array[i] = n - m - x - SA_rev[i];
//    }
//
////    std::sort(index_array.begin(), index_array.end());
//
//    for (pair<INT, INT > subInterval:IntervalsPreprocessed){
//
//        std::vector<std::pair<INT, INT>> subsubIntervals;
//        // only search on the suffixes whose length >= m
//        if (n - SA_rev[subInterval.first] > m -1){
//            findSubIntervalsScaning(subInterval.first, subInterval.second, m + x , LCP_rev, subsubIntervals);
//        } else{
//            continue;
//        }
//
//        std::vector<std::pair<INT, INT>> IntervalsMapped_mining;
//
//        for (pair<INT, INT> interval: subsubIntervals){
//
//            // Find the min and max among the mapped ids, which are the left end and right end of the new mapped interval
//            INT Min = invSA[n - m - x - SA_rev[interval.first]];
//            INT Max = Min;
//
//            for (INT i = interval.first+1; i < interval.second; i++){
//                INT tmp = invSA[n - m - x  - SA_rev[i]];
//                if (tmp>Max){
//                    Max = tmp;
//                }
//                if (tmp< Min){
//                    Min = tmp;
//                }
//            }
//            IntervalsMapped_mining.push_back({Min, Max});
//
//
//
//
//        }
//        for (pair<INT, INT > interval: IntervalsMapped_mining){
//            std::vector<std::pair<INT, INT>> subIntervalsScaningMining;
//
//            findSubIntervalsScaning( interval.first, interval.second, m + x + y , LCP, subIntervalsScaningMining);
//
//            for (pair<INT, INT> subsubInterval: subIntervalsScaningMining){
//                if (n - m - x - y - SA[subsubInterval.first] > -1 ){
//                    subsubIntervals_total.push_back(subsubInterval);
//
//                } else{
//                    continue;
//                }
//            }
//        }
//
//    }
//
//
//
//
//    /*print out information*/
//    cout<< "The experiment for mining every P of length m = "<<m<<" while the length of context x = "<<x<<"; y = "<<y<<": "<<endl;
//    for (const auto& interval : subsubIntervals_total) {
//        std::cout << "SA Interval: [" << interval.first << ", " << interval.second << "]; ";
//        cout<<"occ: ";
//
//        for (INT i = interval.first;i < interval.second + 1; i++){
//            if (i == interval.second){
//                cout<<SA[i];
//
//            } else{
//                cout<<SA[i]<<", ";
//
//            }
//
//        }
//        cout<<": ";
//        for (INT i = 0;i < m + x + y ;i ++){
//            if (i == x or i == m + x){
//                cout<<" ";
//            }
//            cout<<text_string[SA[interval.first] + i];
//        }
//        cout<<std::endl;
//    }



    /*free the memory*/

//    free(text_string);
//    free(text_string_rev);
//    free(pattern);
//    free(pattern_rev);
//
//    free(SA_rev);
//    free(invSA_rev);
//    free(LCP_rev);
//
//    free(SA);
//    free(invSA);
//    free(LCP);
//
//    free(C);

}