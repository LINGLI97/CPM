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

#include <iostream>
#include <fstream>
#include "baseline.h"
#include "cmdline.h"
#include "counting.h"
#include "suffixTree.h"
#include "prefixTree.h"
#include "kd_tree.h"
#include <vector>
#include <malloc.h>
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



int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");

    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
    parser.add<int>("LengthofP", 'm', "the length of P, m = |P| ", false, 1);


    parser.parse_check(argc, argv);




    std::string filename = parser.get<std::string>("filepath");
    std::string patternPath = parser.get<std::string>("pattern");


    /* readfile into text_string and pattern */
    unsigned char* text_string;
    std::vector<unsigned char *> patterns;
    std::vector<std::pair<INT, INT>> contextSizes;

    INT text_size = 0;

    vector<INT> patternSizes;

    readfile(filename,patternPath,text_string, patterns, contextSizes, text_size, patternSizes);
    /*reversed string*/



    unsigned char* text_string_rev = reverseString(text_string);


    auto Construction_start = std::chrono::high_resolution_clock::now();




    long long IndexSpace_start = memory_usage();

    //---------------Index---------------//
    std::vector<Point> pointsD1;
    std::vector<Point> pointsD2;

    std::unordered_map<INT, KDTree*> preorderID2KDTree;


    long long ST_start = memory_usage();


    // Suffix tree construction for original string
    //heavy-light decomposition & calculate preorder id
    suffixTree ST(text_string, text_size);
    ST.initHLD();
//    ST.exportSuffixTreeToDot("count22", true);
//    ST.exportSuffixTreeToDot("count2", false);
//



    long long ST_end = memory_usage();

    long long memory_ST = ST_end-ST_start;


    cout<<"ST: "<<memory_ST*0.001*0.001<<endl;

//------------------------------------------


    long long memory_KD_l =0;

    {

    /*Prepared SA, invSA, LCP, LCE, rmq for the original string*/
    SA_LCP_LCE DS_org(text_string, text_size);


    /*Prepared SA_rev, invSA_rev, LCP_rev, LCE_rev, rmq_rev*/

    SA_LCP_LCE DS_rev(text_string_rev, text_size);






    cout<<"ST.lightNodes.size(): " <<ST.lightNodes.size()<<endl;
    cout<< "text_size: "<<text_size<<endl;

    INT cnt =0;
    for (auto& lightNode: ST.lightNodes){
//        cout<<cnt<<endl;
        cnt++;
        std::vector<Point> pointsDl;
        vector <pair<INT, INT>> L; // the idx in \overline(SA), phi

        for (auto &leaf: lightNode->leaves){
//            if (leaf->start > 0){
            INT phi = DS_org.LCE(lightNode->heavyLeaf->start,leaf->start);
            L.push_back({DS_rev.invSA[text_size - leaf->start-1], phi});
//            }
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
            PT.addPoints(pointsD1,pointsD2, pointsDl,lightNode);

            KDTree* KD_Dl = new KDTree(pointsDl);


            preorderID2KDTree[lightNode->preorderId] = KD_Dl;

        }
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

    ST.lightNodes.clear();

    ST.clearLeaves();



    auto Construction_end = std::chrono::high_resolution_clock::now();





    // find the node which represents u_p

    /*reversed pattern*/


    for (INT i =0; i<patterns.size(); i ++){

        INT x = contextSizes[i].first;

        INT y = contextSizes[i].second;

        unsigned char* pattern_rev = reversePattern(patterns[i]);

        auto query_start = std::chrono::high_resolution_clock::now();

        stNode * up = ST.forward_search(patterns[i], patternSizes[i]);
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
            stNode * current_up = up;

            while (!current_up->child.empty()){
                stNode * rightChild = current_up->child.rbegin()->second;
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
            stNode * lowest_ul = up;
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
            stNode * current_up = up;

            while (!current_up->child.empty()){
                stNode * rightchild = current_up->child.rbegin()->second;
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



    cout<<"----------------------------------------------------"<<endl;

    double Construction_time = std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_start).count()*0.000001;


    long long IndexSpace_end = memory_usage();

    long long memory_Index = IndexSpace_end - IndexSpace_start;
    cout<<"Time for construction of counting: "<<Construction_time<<endl;
    cout<<"Index memory of counting: "<< memory_Index / (1024.0 * 1024.0)<<"MB"<<endl;






    for (auto& pair : preorderID2KDTree) {
        delete pair.second;
    }

    free(text_string);
    free(text_string_rev);
    for (auto &it: patterns){
        free(it);
    }

}