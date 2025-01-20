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
//#include <assert.h>
#include <sys/time.h>
#include <unordered_set>

#include <iostream>
#include <fstream>
#include "baseline.h"
#include "cmdline.h"
#include "counting.h"
#include "suffixTree.h"
#include "prefixTree.h"
//#include "kd_tree.h"
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


    parser.parse_check(argc, argv);




    std::string filename = parser.get<std::string>("filepath");
    std::string patternPath = parser.get<std::string>("pattern");

    const int Rstar = 32;

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
    using point5 = bg::model::point<INT, 5, bg::cs::cartesian>;
    using point3 = bg::model::point<INT, 3, bg::cs::cartesian>;

    std::vector<point5> pointsD1;
    std::vector<point5> pointsD2;

    std::unordered_map<INT,bgi::rtree<point3, bgi::rstar<Rstar>>*> preorderID2RTree;


    // Suffix tree construction for original string
    //heavy-light decomposition & calculate preorder id
    suffixTree ST(text_string, text_size);
    ST.initHLD();
//    ST.exportSuffixTreeToDot("count22", true);
//    ST.exportSuffixTreeToDot("count2", false);



    {

    /*Prepared SA, invSA, LCP, LCE, rmq for the original string*/
    SA_LCP_LCE DS_org(text_string, text_size);


    /*Prepared SA_rev, invSA_rev, LCP_rev, LCE_rev, rmq_rev*/

    SA_LCP_LCE DS_rev(text_string_rev, text_size);





    INT cnt =0;
    for (auto& lightNode: ST.lightNodes){
//        cout<<cnt<<endl;
        cnt++;
        std::vector<point3> pointsDl;

        vector <pair<INT, INT>> L; // the idx in \overline(SA), phi

        for (auto &leaf: lightNode->leaves_start_depth) {

//            if (leaf->start > 0){
            INT phi = DS_org.LCE(lightNode->heavyLeaf->start,leaf.first);
            //L :{index, phi}
            L.push_back({DS_rev.invSA[text_size - leaf.first-1], phi});
//            }
        }
        sort(L.begin(),L.end(), compareByFirstEle);
        std::vector<pair<INT,INT>> prefixesStarting;
        for (auto& id : L) {
            //pair {starting positions, phi}
            prefixesStarting.push_back({DS_rev.SA[id.first],id.second});
        }
        if (!prefixesStarting.empty()){
            prefixTree PT (prefixesStarting,DS_rev);
            PT.updatePhi();
            PT.addPoints(pointsD1,pointsD2, pointsDl,lightNode);

//            KDTree* KD_Dl = new KDTree(pointsDl);
//            preorderID2KDTree[lightNode->preorderId] = KD_Dl;


            bgi::rtree<point3, bgi::rstar<Rstar>>* RT_Dl= new bgi::rtree<point3, bgi::rstar<Rstar>>(pointsDl);
            preorderID2RTree[lightNode->preorderId] = RT_Dl;

        }
    }



    }


    bgi::rtree<point5, bgi::rstar<Rstar>> RT_D1(pointsD1);
    bgi::rtree<point5, bgi::rstar<Rstar>> RT_D2(pointsD2);




    auto Construction_end = std::chrono::high_resolution_clock::now();


    double Construction_time = std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_start).count()*0.000001;

    cout<<"Maximum number of elements in nodes is set to "<< Rstar<<endl;

    cout<<"pointsD1.size(): "<<pointsD1.size()<<endl;

    cout<<"pointsD2.size(): "<<pointsD2.size()<<endl;
    cout<<"preorderID2RTree.size(): "<<preorderID2RTree.size()<<endl;



// clear memory

    pointsD1.clear();
    pointsD2.clear();
    pointsD1.shrink_to_fit();
    pointsD2.shrink_to_fit();

    ST.lightNodes.clear();

    ST.clearLeaves();





    long long IndexSpace_end = memory_usage();

    long long memory_Index = IndexSpace_end - IndexSpace_start;
    cout<<"====================================== counting index construction=============================="<<endl;

    cout<<"Index memory of counting: "<< memory_Index / (1024.0 * 1024.0)<<"MB"<<endl;
    cout<<"Index construction time: "<< Construction_time<< " seconds"<<endl;






    // find the node which represents u_p

    /*reversed pattern*/

    cout<<"====================Starting multiple queries saved in "<<patternPath<<"========================"<<endl;

    for (INT i =0; i<patterns.size(); i ++){

        INT x = contextSizes[i].first;

        INT y = contextSizes[i].second;

        unsigned char* pattern_rev = reversePattern(patterns[i]);

        auto query_start = std::chrono::high_resolution_clock::now();

        stNode * up = ST.forward_search(patterns[i], patternSizes[i]);
        // exit if the u_p is NULL
        if (!up){

//            std::cout << "Text string: " << text_string << std::endl;
            std::cout << "Pattern " <<i<<": " << patterns[i]<< std::endl;

            std::cout << "x: " << x << std::endl;

            std::cout << "y: " << y << std::endl;

            std::cerr << "The pattern does not exist in text string!" << std::endl;
            cout<<"---------------------------------------"<<endl;

            free(pattern_rev);
            continue;
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



            point5 min_point_D1, max_point_D1;

            bg::set<0>(min_point_D1, static_cast<INT>(up->preorderId));
            bg::set<1>(min_point_D1, static_cast<INT>(0));
            bg::set<2>(min_point_D1, static_cast<INT>(x));
            bg::set<3>(min_point_D1, static_cast<INT>(0));
            bg::set<4>(min_point_D1, static_cast<INT>(patternSizes[i] + y));

            bg::set<0>(max_point_D1, static_cast<INT>(rightpreorderId));
            bg::set<1>(max_point_D1, static_cast<INT>(x));
            bg::set<2>(max_point_D1, static_cast<INT>(text_size));
            bg::set<3>(max_point_D1, static_cast<INT>(patternSizes[i] + y));
            bg::set<4>(max_point_D1, static_cast<INT>(text_size));




            bg::model::box<point5> query_box_D1(min_point_D1, max_point_D1);
            auto range_D1 = boost::make_iterator_range(bgi::qbegin(RT_D1, bgi::intersects(query_box_D1)), bgi::qend(RT_D1));
//
            INT result_D1 = boost::distance(range_D1);
#ifdef VERBOSE
        cout<<"result_D1 = "<<result_D1<<endl;
        cout<<"---------------------------------------"<<endl;
#endif
            //D_2


            point5 min_point_D2, max_point_D2;

            bg::set<0>(min_point_D2, static_cast<INT>(up->preorderId));
            bg::set<1>(min_point_D2, static_cast<INT>(0));
            bg::set<2>(min_point_D2, static_cast<INT>(x));
            bg::set<3>(min_point_D2, static_cast<INT>(0));
            bg::set<4>(min_point_D2, static_cast<INT>(patternSizes[i] + y));

            bg::set<0>(max_point_D2, static_cast<INT>(rightpreorderId));
            bg::set<1>(max_point_D2, static_cast<INT>(x));
            bg::set<2>(max_point_D2, static_cast<INT>(text_size));
            bg::set<3>(max_point_D2, static_cast<INT>(patternSizes[i] + y));
            bg::set<4>(max_point_D2, static_cast<INT>(text_size));
            bg::model::box<point5> query_box_D2(min_point_D2, max_point_D2);
            auto range_D2 = boost::make_iterator_range(bgi::qbegin(RT_D2, bgi::intersects(query_box_D2)), bgi::qend(RT_D2));
            INT result_D2 = boost::distance(range_D2);
//

#ifdef VERBOSE
            cout<<"result_D2 = "<<result_D2<<endl;

        cout<<"---------------------------------------"<<endl;

#endif
            //D_l


            // find the u_l if u_p is heavy (D_l)
            stNode * lowest_ul = up;
            while (lowest_ul->parent->heavy){
                lowest_ul = lowest_ul->parent;
            }
            lowest_ul = lowest_ul->parent;





            INT result_Dl =0;
            if (preorderID2RTree[lowest_ul->preorderId] != nullptr){
                point3 min_point_Dl, max_point_Dl;

                bg::set<0>(min_point_Dl, static_cast<INT>(0));
                bg::set<1>(min_point_Dl, static_cast<INT>(x));
                bg::set<2>(min_point_Dl, static_cast<INT>(patternSizes[i] + y));

                bg::set<0>(max_point_Dl, static_cast<INT>(x));
                bg::set<1>(max_point_Dl, static_cast<INT>(text_size));
                bg::set<2>(max_point_Dl, static_cast<INT>(text_size));


                bg::model::box<point3> query_box_Dl(min_point_Dl, max_point_Dl);
                auto range_Dl = boost::make_iterator_range(bgi::qbegin(*(preorderID2RTree[lowest_ul->preorderId]), bgi::intersects(query_box_Dl)), bgi::qend(*(preorderID2RTree[lowest_ul->preorderId])));
                // Count the number of matches
                result_Dl = boost::distance(range_Dl);
            }





#ifdef VERBOSE

            cout<<"result_Dl = "<<result_Dl<<endl;

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

            point5 min_point_D1, max_point_D1;

            bg::set<0>(min_point_D1, static_cast<INT>(up->preorderId));
            bg::set<1>(min_point_D1, static_cast<INT>(0));
            bg::set<2>(min_point_D1, static_cast<INT>(x));
            bg::set<3>(min_point_D1, static_cast<INT>(0));
            bg::set<4>(min_point_D1, static_cast<INT>(patternSizes[i] + y));

            bg::set<0>(max_point_D1, static_cast<INT>(rightpreorderId));
            bg::set<1>(max_point_D1, static_cast<INT>(x));
            bg::set<2>(max_point_D1, static_cast<INT>(text_size));
            bg::set<3>(max_point_D1, static_cast<INT>(patternSizes[i] + y));
            bg::set<4>(max_point_D1, static_cast<INT>(text_size));




            bg::model::box<point5> query_box_D1(min_point_D1, max_point_D1);
            auto range_D1 = boost::make_iterator_range(bgi::qbegin(RT_D1, bgi::intersects(query_box_D1)), bgi::qend(RT_D1));
//
            INT result_D1 = boost::distance(range_D1);
#ifdef VERBOSE

            cout<<"result_D1 = "<<result_D1<<endl;

        cout<<"---------------------------------------"<<endl;
#endif

            //D_2
            point5 min_point_D2, max_point_D2;

            bg::set<0>(min_point_D2, static_cast<INT>(up->preorderId));
            bg::set<1>(min_point_D2, static_cast<INT>(0));
            bg::set<2>(min_point_D2, static_cast<INT>(x));
            bg::set<3>(min_point_D2, static_cast<INT>(0));
            bg::set<4>(min_point_D2, static_cast<INT>(patternSizes[i] + y));

            bg::set<0>(max_point_D2, static_cast<INT>(rightpreorderId));
            bg::set<1>(max_point_D2, static_cast<INT>(x));
            bg::set<2>(max_point_D2, static_cast<INT>(text_size));
            bg::set<3>(max_point_D2, static_cast<INT>(patternSizes[i] + y));
            bg::set<4>(max_point_D2, static_cast<INT>(text_size));
            bg::model::box<point5> query_box_D2(min_point_D2, max_point_D2);
            auto range_D2 = boost::make_iterator_range(bgi::qbegin(RT_D2, bgi::intersects(query_box_D2)), bgi::qend(RT_D2));
            INT result_D2 = boost::distance(range_D2);

            counts = result_D1 + result_D2 ;

#ifdef VERBOSE

            cout<<"result_D2 = "<<result_D2<<endl;

        cout<<"---------------------------------------"<<endl;
#endif
        }


        auto query_end = std::chrono::high_resolution_clock::now();
        double query_time = std::chrono::duration_cast < std::chrono::microseconds > (query_end - query_start).count()*0.000001;


        cout<<"Pattern "<< i<<": "<<patterns[i]<<" and x = "<<x <<"; y = "<<y<<endl;


        cout<<"There are "<<counts<< " distinct XPY that occur in T."<<endl;

        cout<<"Time for query: "<<query_time<<endl;
        cout<<"------------------------------------------------------"<<endl;

        free(pattern_rev);

    }







    for (auto& pair : preorderID2RTree) {
        delete pair.second;
    }

    free(text_string);
    free(text_string_rev);
    for (auto &it: patterns){
        free(it);
    }

}