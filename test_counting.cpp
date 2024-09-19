#include <cstdio>
#include <cmath>
#include <cstdlib>

#include <string>
#include <getopt.h>
#include <assert.h>
#include <sys/time.h>
#include <unordered_set>

#include <iostream>
#include <fstream>
#include "baseline.h"
#include "cmdline.h"
#include "counting.h"
#include "suffixTree.h"
#include "prefixTree.h"
#include "kd_tree.h"
#include <vector>
#include <algorithm>    // std::sort
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


bool compareByFirstEle(pair<INT, INT>& p1, pair<INT, INT>& p2){
    return p1.first < p2.first;
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
    if (s == e){
        subIntervals.push_back({s,e});

    } else{

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


    }



}

bool areMapsIdentical(const unordered_map<string, vector<INT>>& map1, const unordered_map<string, vector<INT>>& map2) {
    if (map1.size() != map2.size()) {
        return false;
    }

    // Iterate through the first map
    for (const auto& pair : map1) {
        auto it = map2.find(pair.first);
        if (it == map2.end()) {
            return false; // Key not found in the second map
        }

        // Check if the vector for this key is the same in both maps
        const vector<INT>& vec1 = pair.second;
        const vector<INT>& vec2 = it->second;
        if (vec1 != vec2) {
            return false;
        }
    }

    return true; // Maps are identical
}

int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<int>("lengthMax", 'R', "the maximal length of text string", false, 100);
    parser.add<int>("lengthMin", 'L', "the minimal length of text string", false, 5);

    parser.add<int>("round", 'r', "the number of test rounds", false, 1000000);


    parser.parse_check(argc, argv);






    INT max_text_length = parser.get<int>("lengthMax");
    INT min_text_length = parser.get<int>("lengthMin");

    INT NumRound = parser.get<int>("round");
    srand(time(NULL));  // Initialize the random seed


    for (INT r =0; r< NumRound; r++){
        cout<<r<<endl;

        // Generate a random length for text_string that is less than max_text_length
        INT text_size = min_text_length + rand() % (max_text_length - min_text_length + 1);

        // Allocate memory for text_string
        unsigned char* text_string = (unsigned char*)malloc((text_size + 1) * sizeof(unsigned char));

        // Fill text_string with random characters
        for (INT i = 0; i < text_size; i++) {
            text_string[i] = 'a' + (rand() % 26);  // Generate a random letter
        }
        text_string[text_size] = '\0';  // Add null terminator to mark the end of the string

        // Generate a random length for pattern

        if (text_size < 2) {
            std::cerr << "Text size is too small to create a pattern." << std::endl;
            continue;
        }
        INT pattern_size = rand() % (text_size - 2) +1 ;

        // Allocate memory for pattern
        unsigned char* pattern = (unsigned char*)malloc((pattern_size + 1) * sizeof(unsigned char));

        // Extract a substring from text_string to use as pattern
        INT start = rand() % (text_size - pattern_size + 1);  // Choose a random starting point
        for (INT i = 0; i < pattern_size; i++) {
            pattern[i] = text_string[start + i];
        }
        pattern[pattern_size] = '\0';  // Add null terminator to mark the end of the pattern

        // Calculate the maximum value of l that satisfies 2*l + |pattern| <= 1000

        if ((text_size - pattern_size) / 2 <= 0) {
            std::cerr << "Invalid operation condition encountered." << std::endl;
            continue;  // or handle as appropriate
        }

        INT half_diff = (text_size - pattern_size) / 2;
        INT x = (half_diff > 0) ? rand() % (half_diff + 1)+1 : 1;
        INT y = (half_diff > 0) ? rand() % (half_diff + 1) +1: 1;

// Allocate memory for the new string with padding
        INT new_text_size = text_size + 2;
        unsigned char* new_text_string = (unsigned char*)malloc((new_text_size + 1) * sizeof(unsigned char));

// Add 'x' number of '$' characters at the beginning
        for (INT i = 0; i < 1; i++) {
            new_text_string[i] = '$';
        }

// Copy the original text_string into the new_text_string
        memcpy(new_text_string + 1, text_string, text_size);

// Add 'y' number of '$' characters at the end
        for (INT i = 0; i < 1; i++) {
            new_text_string[1 + text_size + i] = '$';
        }

        new_text_string[new_text_size] = '\0';  // Null-terminate the new string

// Free the old text_string as it's no longer needed
        free(text_string);

// Update text_string to point to the new padded string and text_size to the new size
        text_string = new_text_string;
        text_size = new_text_size;

        /*reversed pattern*/
        unsigned char* pattern_rev = reverseReturn(pattern);

//        INT n = strlen ( ( char * ) text_string );

        /*reversed string*/
        unsigned char* text_string_rev = reverseReturn(text_string);

        /*Prepared SA, invSA, LCP, LCE, rmq for the original string*/
        SA_LCP_LCE DS_org(text_string, text_size);



        /*Prepared SA_rev, invSA_rev, LCP_rev, LCE_rev, rmq_rev*/

        SA_LCP_LCE DS_rev(text_string_rev, text_size);


// Suffix tree construction
        auto Construction_start = std::chrono::high_resolution_clock::now();

        suffixTree ST(text_string, text_size);

        auto Construction_end = std::chrono::high_resolution_clock::now();
        double time_ST = std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_start).count()*0.000001;

        //heavy-light decomposition & preorder id

        ST.initHLD();


        std::vector<Point> pointsD1;
        std::vector<Point> pointsD2;
        std::vector<Point> pointsDl;

        stNode * up = ST.forward_search(pattern, pattern_size);
        stNode * current_ul = up;
        if (up == NULL){
            cout<<"------------------------Up is Null------------------------"<<endl;


            std::cout << "Text string: " << text_string << std::endl;
            std::cout << "Pattern: " << pattern << std::endl;
            std::cout << "x: " << x << std::endl;

            std::cout << "y: " << y << std::endl;
        }

        if (up->heavy){
            // find the u_l if u_p is heavy (D_l)
            while (current_ul->parent->heavy){
                current_ul = current_ul->parent;
            }
            current_ul = current_ul->parent;
        }

        for (auto& lightNode: ST.lightNodes){
            bool flag_Dl = false;
            vector <pair<INT, INT>> L; // the idx in \overline(SA), phi

            for (auto &leaf: lightNode->leaves){
                if (leaf->start > 0){
                    INT phi = DS_org.LCE(lightNode->heavyLeaf->start,leaf->start);
                    L.push_back({DS_rev.invSA[text_size - leaf->start], phi});
                }
            }
            sort(L.begin(),L.end(), compareByFirstEle);
            std::vector<pair<INT,INT>> prefixesStarting;
            for (auto& id : L) {
                //pair {starting, phi}
                prefixesStarting.push_back({DS_rev.SA[id.first],id.second});
            }
            if (!prefixesStarting.empty()){
                prefixTree PT (prefixesStarting,DS_rev);
                PT.updatePhi();
                if (up->heavy){
                    flag_Dl = (lightNode == current_ul);
                }
                PT.addPoints(pointsD1,pointsD2, pointsDl,lightNode, flag_Dl);

            }
        }


        INT counts=0;
        if (up){
            if (up->heavy){

                //D_1
                KDTree KD_D1(pointsD1);

                // find the preorder ID of rightmost leaf
                INT rightpreorderId = up->preorderId;
                stNode * current_up = up;

                while (!current_up->child.empty()){
                    stNode * rightchild = current_up->child.rbegin()->second;
                    rightpreorderId = rightchild->preorderId;
                    current_up = rightchild;
                }



                vector<pair<double, double>> ranges_D1 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                          {0,(double) pattern_size +y},{(double) pattern_size +y, (double) text_size}};

                vector<Point> result_D1 = KD_D1.rangeSearch(ranges_D1);

//                for (const Point& pt : result_D1) {
//                    for (double coord : pt.coords) {
//                        cout << coord << " ";
//                    }
//                    cout << endl;
//                }
//                cout<<"---------------------------------------"<<endl;

                //D_2
                KDTree KD_D2(pointsD2);



                vector<pair<double, double>> ranges_D2 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                          {0,(double) pattern_size +y},{(double) pattern_size +y, (double) text_size}};

                vector<Point> result_D2 = KD_D2.rangeSearch(ranges_D2);


//                for (const Point& pt : result_D2) {
//                    for (double coord : pt.coords) {
//                        cout << coord << " ";
//                    }
//                    cout << endl;
//                }
//                cout<<"---------------------------------------"<<endl;

                //D_l

                KDTree KD_Dl(pointsDl);

                vector<pair<double, double>> ranges_Dl = {{0, (double) x}, {(double) x,(double) text_size}, { (double) pattern_size + y, (double) text_size}};


                vector<Point> result_Dl = KD_Dl.rangeSearch(ranges_Dl);

//                for (const Point& pt : result_Dl) {
//                    for (double coord : pt.coords) {
//                        cout << coord << " ";
//                    }
//                    cout << endl;
//                }
//                cout<<"---------------------------------------"<<endl;

                counts = result_D1.size()+ result_D2.size()+ result_Dl.size();


            } else{


                KDTree KD_D1(pointsD1);

                // find the preorder ID of rightmost leaf
                INT rightpreorderId = up->preorderId;
                stNode * current_up = up;

                while (!current_up->child.empty()){
                    stNode * rightchild = current_up->child.rbegin()->second;
                    rightpreorderId = rightchild->preorderId;
                    current_up = rightchild;
                }



                vector<pair<double, double>> ranges_D1 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                          {0,(double) pattern_size +y},{(double) pattern_size +y, (double) text_size}};

                vector<Point> result_D1 = KD_D1.rangeSearch(ranges_D1);

//                for (const Point& pt : result_D1) {
//                    for (double coord : pt.coords) {
//                        cout << coord << " ";
//                    }
//                    cout << endl;
//                }
//                cout<<"---------------------------------------"<<endl;

                //D_2
                KDTree KD_D2(pointsD2);



                vector<pair<double, double>> ranges_D2 = {{(double) up->preorderId, (double) rightpreorderId}, {0, (double)x}, { (double)x, (double)text_size},
                                                          {0,(double) pattern_size +y},{(double) pattern_size +y, (double) text_size}};

                vector<Point> result_D2 = KD_D2.rangeSearch(ranges_D2);

//                for (const Point& pt : result_D2) {
//                    for (double coord : pt.coords) {
//                        cout << coord << " ";
//                    }
//                    cout << endl;
//                }
//                cout<<"---------------------------------------"<<endl;
                counts = result_D1.size()+ result_D2.size();

            }


        }else{
            cout<<"The pattern P does not exist!"<<endl;
        }







        /*Naive checkers*/

        unordered_map<string, vector<INT>> resultsNaive;
        pair<INT,INT> intervalNaive= pattern_matching (pattern, text_string, DS_org.SA,  DS_org.LCP, DS_org.rmq, text_size,  pattern_size);

        for (INT i = intervalNaive.first; i < intervalNaive.second + 1; i++){
            INT start = DS_org.SA[i] - x;
            INT end = DS_org.SA[i] + pattern_size + y;
            if (start < 0 ){
                continue;
            }
            if (end > text_size ){
                continue;
            }
            string subString(reinterpret_cast<char*>(text_string + start), end - start);

            resultsNaive[subString].push_back(start);

        }
//        cout<<"------------------------Naive------------------------"<<endl;
//
//        // Example of printing results
//        for (const auto& entry : resultsNaive) {
//            cout << "Substring: " << entry.first << " at positions: ";
//            for (int pos : entry.second) {
//                cout << pos << " ";
//            }
//            cout << endl;
//        }

        if (resultsNaive.size() != counts){


            cout<<"------------------------Input------------------------"<<endl;


            std::cout << "Text string: " << text_string << std::endl;
            std::cout << "Pattern: " << pattern << std::endl;
            std::cout << "x: " << x << std::endl;

            std::cout << "y: " << y << std::endl;
//            cout<<"------------------------RMQ------------------------"<<endl;
//
//
//            cout<< "The experiment for given P = ";
//            for ( INT i = 0; i < pattern_size; i ++ ){
//                cout<<pattern[i];
//            }
//            cout<<" while the length of context l = "<<l<<": "<<endl;
//            // Print the generated intervals
//            for (const auto& interval : subIntervalsRMQNonNeg) {
//                std::cout << "SA Interval: [" << interval.first << ", " << interval.second << "]; ";
//                cout<<"occ: ";
//
//                for (INT i = interval.first;i < interval.second + 1; i++){
//                    if (i == interval.second){
//                        cout<<SA[i];
//
//                    } else{
//                        cout<<SA[i]<<", ";
//
//                    }
//
//                }
//                cout<<": ";
//                for (INT i = 0;i < pattern_size + 2 * l ;i ++){
//                    if (i == l or i == pattern_size + l){
//                        cout<<" ";
//                    }
//                    cout<<text_string[SA[interval.first] + i];
//                }
//                cout<<std::endl;
//            }

            cout<<"------------------------Naive------------------------"<<endl;

            for (const auto& entry : resultsNaive) {
                cout << "Substring: " << entry.first << " at positions: ";
                for (INT pos : entry.second) {
                    cout << pos << " ";
                }
                cout << endl;
            }
            exit( EXIT_FAILURE );

        }




        /*free the memory*/

//        free(text_string);
//        free(text_string_rev);
        free(pattern);
        free(pattern_rev);



    }
    cout<<NumRound<<" tests are finished successfully"<<endl;

    cout<<endl;





}







