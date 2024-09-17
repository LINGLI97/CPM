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
#include "cmdline.h"
#include "baseline.h"
#include <unordered_set>
#include "counting.h"
#include <iostream>
#include <fstream>
#include "suffixTree.h"


#include "prefixTree.h"
//#include "kd_tree.h"
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








Node* newNode(Point point) {
    Node* node = new Node;
    node->point = point;
    node->left = node->right = nullptr;
    return node;
}
double distance(Point a, Point b) {
    double dist = 0;
    for (size_t i = 0; i < a.coords.size(); ++i) {
        dist += (a.coords[i] - b.coords[i]) * (a.coords[i] - b.coords[i]);
    }
    return std::sqrt(dist);
}

Node* insertNode(Node* root, Point point, unsigned depth) {
    if (root == nullptr) {

        return newNode(point);
    }
    unsigned k = point.coords.size();
    unsigned cd = depth % k;
    if (point.coords[cd] < (root->point.coords[cd])) {
        root->left = insertNode(root->left, point, depth + 1);
    } else {
        root->right = insertNode(root->right, point, depth + 1);
    }
    return root;
}


void range_Search(Node* root, Point low, Point high, unsigned depth) {
    if (root == nullptr) return;
    unsigned k = low.coords.size();
    unsigned cd = depth % k;
    if (root->point.coords[cd] >= low.coords[cd]) {
        range_Search(root->left, low, high, depth + 1);
    }
    bool inside = true;
    for (size_t i = 0; i < k; i++) {
        if (root->point.coords[i] < low.coords[i] || root->point.coords[i] > high.coords[i]) {
            inside = false;
            break;
        }
    }
    if (inside) {
        std::cout << "Point: [";
        for (size_t i = 0; i < k - 1; ++i) {
            std::cout << root->point.coords[i] << ", ";
        }
        std::cout << root->point.coords[k - 1] << "]\n";
    }
    if (root->point.coords[cd] <= high.coords[cd]) {
        range_Search(root->right, low, high, depth + 1);
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


void readfile(string &filename, string &patternPath, unsigned char * &text_string, unsigned char * &pattern,unordered_set<unsigned char> &alphabet, INT& text_size, INT& pattern_size){
    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;

    }


    is_text.seekg(0, std::ios::end);
    text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);




    text_string = (unsigned char*) malloc((text_size + 1) * sizeof(unsigned char));

    char c = 0;
    for (INT i = 0; i < text_size; i++) {
        is_text.read(reinterpret_cast<char*>(&c), 1);
        alphabet.insert((unsigned char)c);
        text_string[i] = (unsigned char)c;
    }


    is_text.close();
    text_string[text_size] = '\0';


    is_text.close();


    /* open pattern file and write it into pattern*/
    std::ifstream is_pattern(patternPath, std::ios::binary);
    if (!is_pattern) {
        std::cerr << "Error opening pattern file: " << patternPath << std::endl;
    }

    is_pattern.seekg(0, std::ios::end);
    pattern_size = is_pattern.tellg();
    is_pattern.seekg(0, std::ios::beg);

    pattern = ( unsigned char * ) malloc (  ( pattern_size + 1 ) * sizeof ( unsigned char ) );

    c = 0;
    for (INT i = 0; i < pattern_size; i++)
    {
        is_pattern.read(reinterpret_cast<char*>(&c), 1);
        pattern[i] = (unsigned char) c;
    }

    pattern[pattern_size] = '\0';
    is_pattern.close();
}



int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");

    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
    parser.add<int>("X", 'x', "the length of context, |X| = x ", false, 1);
    parser.add<int>("Y", 'y', "the length of context, |Y| = y ", false, 1);
    parser.add<int>("LengthofP", 'm', "the length of P, m = |P| ", false, 1);


    parser.parse_check(argc, argv);




    std::string filename = parser.get<std::string>("filepath");
    std::string patternPath = parser.get<std::string>("pattern");

    INT x = parser.get<int>("X");

    INT y = parser.get<int>("Y");

    /* readfile into text_string and pattern */
    unordered_set<unsigned char> alphabet;
    unsigned char* text_string;
    unsigned char * pattern;
    INT text_size = 0;
    INT pattern_size = 0;
    readfile(filename,patternPath,text_string, pattern, alphabet, text_size, pattern_size);

    /*reversed string*/
    unsigned char* text_string_rev = reverseReturn(text_string);

    /*reversed pattern*/
    unsigned char* pattern_rev = reverseReturn(pattern);




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



    for (auto& lightNode: ST.lightNodes){

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

            prefixesStarting.push_back({DS_rev.SA[id.first],id.second});
        }
        if (!prefixesStarting.empty()){
            prefixTree PT (prefixesStarting,DS_rev);
            PT.updatePhi();

            PT.addPoints(pointsD1,pointsD2, pointsDl,lightNode);

        }


    }

    stNode * up = ST.forward_search(pattern, pattern_size);
    if (up){
        if (up->heavy){

//            Point low_D1 = { {(double) up->preorderId, 0, x, 0, pattern_size +y } };
//            Point high_D1 = {{(double) up->child.rbegin()->second->preorderId, x, text_size,  pattern_size +y, text_size} };
//            KD D1(pointsD1);
//            D1.range_Search(D1.KDroot,low_D1,high_D1,0);
//            cout<<"---------------------------------------"<<endl;
//
//            Point low_D2 = { {(double) up->preorderId, 0, x, 0, pattern_size +y } };
//            Point high_D2 = {{(double) up->child.rbegin()->second->preorderId, x, text_size,  pattern_size +y, text_size} };
//            KD D2(pointsD2);
//            D2.range_Search(D2.KDroot,low_D2,high_D2,0);
//            cout<<"---------------------------------------"<<endl;
//            Point low_Dl = { {0, x, pattern_size + y} };
//            Point high_Dl = { {x, text_size, text_size} };
//            KD Dl(pointsDl);
//            Dl.range_Search(Dl.KDroot,low_Dl,high_Dl,0);
//            cout<<"---------------------------------------"<<endl;
            INT rightpreorderId = up->preorderId;
            stNode * current_up = up;

            while (!current_up->child.empty()){
                stNode * rightchild = current_up->child.rbegin()->second;
                rightpreorderId = rightchild->preorderId;
                current_up = rightchild;
            }
            Point low_D1 = { {(double) up->preorderId, 0, (double) x, 0, (double) pattern_size +y } };
            Point high_D1 = {{(double) rightpreorderId, (double)x , (double)text_size, (double) pattern_size +y, (double) text_size} };
            Node* D1root = nullptr;

            for (const auto& point : pointsD1) {
                D1root = insertNode(D1root, point, 0);
            }

            range_Search(D1root,low_D1,high_D1,0);
            cout<<"---------------------------------------"<<endl;

            Point low_D2 = { {(double) up->preorderId, 0, (double) x, 0, (double) pattern_size +y } };
            Point high_D2 = {{(double) rightpreorderId, (double)x , (double)text_size, (double) pattern_size +y, (double) text_size} };
            Node* D2root = nullptr;


            for (const auto& point : pointsD2) {
                D2root = insertNode(D2root, point, 0);
            }

            range_Search(D2root,low_D2,high_D2,0);
            cout<<"---------------------------------------"<<endl;
            Point low_Dl = { {0, 0, (double) x, (double) pattern_size + y} };
            Point high_Dl = { {(double) up->preorderId, (double) x, (double) text_size, (double) text_size} };
            Node* Dlroot = nullptr;
            for (const auto& point : pointsDl) {
                Dlroot = insertNode(Dlroot, point, 0);
            }

            range_Search(Dlroot,low_Dl,high_Dl,0);
            cout<<"---------------------------------------"<<endl;


            delete D1root;
            delete D2root;
            delete Dlroot;



        } else{
            INT rightpreorderId = up->preorderId;
            stNode * current_up = up;

            while (!current_up->child.empty()){
                stNode * rightchild = current_up->child.rbegin()->second;
                rightpreorderId = rightchild->preorderId;
                current_up = rightchild;
            }

            Point low_D1 = { {(double) up->preorderId, 0, (double) x, 0, (double) pattern_size +y } };
            Point high_D1 = {{(double) rightpreorderId, (double)x , (double)text_size, (double) pattern_size +y, (double) text_size} };
            Node* D1root = nullptr;

            for (const auto& point : pointsD1) {
                D1root = insertNode(D1root, point, 0);
            }

            range_Search(D1root,low_D1,high_D1,0);
            cout<<"---------------------------------------"<<endl;

            Point low_D2 = { {(double) up->preorderId, 0, (double) x, 0, (double) pattern_size +y } };
            Point high_D2 = {{(double) rightpreorderId , (double)x , (double)text_size, (double) pattern_size +y, (double) text_size} };
            Node* D2root = nullptr;


            for (const auto& point : pointsD2) {
                D2root = insertNode(D2root, point, 0);
            }

            range_Search(D2root,low_D2,high_D2,0);
            cout<<"---------------------------------------"<<endl;

            delete D1root;
            delete D2root;

        }


    }else{
        cout<<"The pattern P does not exist!"<<endl;
    }





//    Node* D1root = nullptr;
//    // 插入一些三维点
//    std::vector<Point> points = { { {-1, 6, 2} }, { {17, 15, 9} }, { {13, 15, 8} }, { {6, 12, 5} }, { {9, 1, 2} }, { {2, 7, 4} }, { {10, 19, 11} } };
//
//
//    for (const auto& point : points) {
//        D1root = insertNode(D1root, point, 0);
//    }
//    Point low = { {0, 0, 0} };
//    Point high = { {10, 20, 10} };
//    std::cout << "Points within range:\n";
//    range_Search(D1root, low, high, 0);


#ifdef VERBOSE // print all of them
    printArray("SA_rev", DS_rev.SA, text_size);


    printArray("invSA_rev", DS_rev.invSA, text_size);


    printArray("LCP_rev", DS_rev.LCP, text_size);

    printArray("SA", DS_org.SA, text_size);


    printArray("invSA", DS_org.invSA, text_size);


    printArray("LCP", DS_org.LCP, text_size);

    printstring("SA->string",DS_org.SA, text_size,text_string);
    printstring("SA_rev->string",DS_rev.SA, text_size,text_string_rev);

#endif



    free(pattern);
    free(pattern_rev);

}