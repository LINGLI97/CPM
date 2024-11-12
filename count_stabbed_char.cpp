#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "cmdline.h"
#include <fstream>
//#include "LZ77_char_updated.h"
#include "LZ77_char.h"
#include <algorithm>  // std::lower_bound
#include "truncatedSuffixTree_char.h"

#ifdef COUNT
#include "kd_tree_counting.h"

#else
#include "kd_tree_reporting.h"

#endif

#include "truncatedPrefixTree_char.h"
#include "prefixTree.h"
#include "SA_LCP_LCE.h"
#include <malloc.h>


long long memory_usage() {
    struct mallinfo2 mi = mallinfo2();
    return mi.uordblks + mi.hblkhd;
}



void readfile_woDollar(string &filename, string &patternPath, unsigned char * &text_string, std::vector<unsigned char *> &patterns,
                       std::vector<std::pair<INT, INT>> &contextSizes, INT& text_size, vector<INT> & patternSizes, INT &B){
    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return;
    }

    is_text.seekg(0, std::ios::end);
    text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);

    text_string = (unsigned char *)malloc((text_size + 1) * sizeof(unsigned char));

    char c = 0;
    for (INT i = 0; i < text_size; i++) {
        is_text.read(reinterpret_cast<char *>(&c), 1);
        text_string[i] = (unsigned char)c;
    }

    is_text.close();

    text_string[text_size] = '\0';

//    text_string_dollar = (unsigned char *)malloc((text_size + 2) * sizeof(unsigned char));
//
//    std::memcpy(text_string_dollar, text_string, text_size * sizeof(unsigned char));
//
//    text_string_dollar[text_size] = '$';
//    text_string_dollar[text_size + 1] = '\0';

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
        if (pattern_str.size() + x + y > B){
            B = pattern_str.size() + x + y +1;
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



void readfile_wDollar(string &filename, string &patternPath, unsigned char * &text_string, std::vector<unsigned char *> &patterns,
                       std::vector<std::pair<INT, INT>> &contextSizes, INT& text_size, INT& alphabetSize, vector<INT> & patternSizes, INT &B1, INT &B2, INT &B_T){
    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return;
    }

    is_text.seekg(0, std::ios::end);
    text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);
    unordered_set<unsigned char> alphabet;

    text_string = (unsigned char *)malloc((text_size + 2) * sizeof(unsigned char));

    char c = 0;
    for (INT i = 0; i < text_size; i++) {
        is_text.read(reinterpret_cast<char *>(&c), 1);
        text_string[i] = (unsigned char)c;
        alphabet.insert((unsigned char)c);

    }

    is_text.close();
    text_string[text_size] = '$';
    alphabet.insert('$');

    text_string[text_size +1] = '\0';

    text_size  = text_size +1 ;

    alphabetSize = alphabet.size();

//    text_string_dollar = (unsigned char *)malloc((text_size + 2) * sizeof(unsigned char));
//
//    std::memcpy(text_string_dollar, text_string, text_size * sizeof(unsigned char));
//
//    text_string_dollar[text_size] = '$';
//    text_string_dollar[text_size + 1] = '\0';

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
        if (pattern_str.size() + y > B1){
            B1 = pattern_str.size() + y;
        }
        if(x > B2){
            B2 = x ;
        }

        if (x + y+ pattern_str.size() > B_T){
            B_T = x + y+ pattern_str.size();
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

bool compareByFirstEle(pair<INT, INT>& p1, pair<INT, INT>& p2){
    return p1.first < p2.first;
}

int main(int argc, char *argv[]) {


    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");

    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");



    parser.parse_check(argc, argv);

    std::string filename = parser.get<std::string>("filepath");
    std::string patternPath = parser.get<std::string>("pattern");


    //B1 > m + y; B2 > x
    INT B1 = 0;

    INT B2 = 0;
    INT B_T =0;


    /* readfile into text_string and pattern */
//    unsigned char *text_string_woDollar;
    unsigned char *text_string_wDollar;

    std::vector<unsigned char *> patterns;
    std::vector<std::pair<INT, INT>> contextSizes;

    INT text_size = 0;
    INT alphabetSize= 0;
    vector<INT> patternSizes;

//    readfile_woDollar(filename, patternPath, text_string_woDollar, patterns, contextSizes, text_size, patternSizes, B);

    readfile_wDollar(filename, patternPath, text_string_wDollar, patterns, contextSizes, text_size, alphabetSize, patternSizes, B1, B2, B_T);

    std::vector<INT> phrase_starts;



    auto LZ77_start = std::chrono::high_resolution_clock::now();



    LZ77* lz77 = new LZ77(text_string_wDollar, alphabetSize);

    lz77->compress(phrase_starts);

    cout<<"phrase_starts.size() = "<<phrase_starts.size()<<endl;

//    printmemory_usage();

    auto LZ77_end = std::chrono::high_resolution_clock::now();
    delete lz77;
    lz77 = nullptr;

    //release all memories
    //add $ at the end of new string

    vector<INT> hash_positions;
    vector<INT> hash_positions_rev;

    unsigned char *new_string = construct_string_from_boundaries(text_string_wDollar, phrase_starts, B_T, hash_positions);

    phrase_starts.clear();
    phrase_starts.shrink_to_fit();
    std::vector<INT>().swap(phrase_starts);
//    cout<<"-----------after LZ77-----------"<<endl;

    INT new_string_size = strlen((const char *) new_string);

    INT num_hash = hash_positions.size();
    for ( INT & i: hash_positions){
        hash_positions_rev.push_back(new_string_size -2 - i);

    }

    std::reverse(hash_positions_rev.begin(), hash_positions_rev.end());

    cout<< "B1 =  "<<B1 <<"; B2 = "<<B2<< "; B for lz77: " << B_T<<endl;
    cout << "The number of hash in T' = " << num_hash << endl;
    // -1: $
    cout << "text size: " << text_size << "; compressed text size: " << new_string_size << endl;
    cout << "Compressed ratio: " << (double) (new_string_size) / text_size << endl;
#ifdef COUNT
    cout<<"using KD counting"<<endl;
#else
    cout<<"using KD report"<<endl;

#endif



    unsigned char *new_string_rev = reverseString(new_string);
//    long long LZ77_end = memory_usage();
//    cout<<"LZ77 memory: "<< (LZ77_end-LZ77_start) / (1024.0 * 1024.0)<<"MB"<<endl;

    long long IndexSpace_start = memory_usage();

    auto Construction_start = std::chrono::high_resolution_clock::now();
//    long long ST_start = memory_usage();


    suffixTree* ST_newString = new suffixTree(new_string, new_string_size);

//    suffixTree ST_newString(new_string, new_string_size);
    ST_newString->initHLD();

//    long long ST_end = memory_usage();

//    long long trunST_start = memory_usage();

    truncatedSuffixTree truncatedST(ST_newString, B1, hash_positions);

    truncatedST.initHLD();


    //    long long trunST_end= memory_usage();


    delete ST_newString;
    ST_newString = nullptr;


//    truncatedST.exportSuffixTreeToDot("count22", true);
//    truncatedST.exportSuffixTreeToDot("count2", false);


//    cout<<"T = "<<text_string_wDollar<<endl;
//    cout<<"T ="<<new_string<<endl;

//    cout<<"T'_rev ="<<new_string_rev<<endl;


    INT KD_l=0;

    std::vector<Point> pointsD1;
    std::vector<Point> pointsD2;

#ifdef COUNT
    std::unordered_map<INT, KDTreeCounting *> preorderID2KDTree;

#else
    std::unordered_map<INT, KDTreeReporting *> preorderID2KDTree;

#endif


    {
    /*Prepared SA, invSA, LCP, LCE, rmq for the original string*/
    SA_LCP_LCE DS_org(new_string, new_string_size);
//        printArray("SA_rev", DS_org.SA, new_string_size);


    /*Prepared SA_rev, invSA_rev, LCP_rev, LCE_rev, rmq_rev*/

    SA_LCP_LCE DS_rev(new_string_rev, new_string_size);
//        printArray("SA_rev", DS_rev.SA, new_string_size);



    //---------------Index---------------//


    INT cnt = 0;
    for (auto &lightNode: truncatedST.lightNodes) {
//        cout<<cnt<<endl;
        cnt++;
        std::vector<Point> pointsDl;
        vector<pair<INT, INT>> L; // the idx in \overline(SA), phi
        for (auto &leaf: lightNode->leaves_start_depth) {
//            if (leaf->start > 0){

            INT phi = DS_org.LCE(lightNode->heavyLeaf->start, leaf.first);

            INT min_legnth = std::min(lightNode->heavyLeaf->depth, leaf.second);
            if (phi > min_legnth) {
                phi = min_legnth;
            }
            L.push_back({DS_rev.invSA[new_string_size - leaf.first - 1], phi});
//            }
        }
        sort(L.begin(), L.end(), compareByFirstEle);
        std::vector<pair<INT, INT>> prefixesStarting;
        for (auto &id: L) {
            //pair {starting, phi}
            prefixesStarting.push_back({DS_rev.SA[id.first], id.second});
        }
        if (!prefixesStarting.empty()) {
            prefixTree PT(prefixesStarting, DS_rev);
            PT.updatePhi();
            truncatedPrefixTree truncatedPT(PT, B2, hash_positions_rev);

            truncatedPT.addPoints(pointsD1, pointsD2, pointsDl, lightNode);

            if (!pointsDl.empty()){



#ifdef COUNT
                KDTreeCounting *KD_Dl = new KDTreeCounting(pointsDl);

#else
                KDTreeReporting *KD_Dl = new KDTreeReporting(pointsDl);

#endif


//                long long KD_start = memory_usage();

                preorderID2KDTree[lightNode->preorderId] = KD_Dl;
//                long long KD_end = memory_usage();

//                KD_Dl = KD_Dl + KD_end-KD_start;



            }



        }
    }

    }


//    cout<<"pointsD1: "<<endl;
//
//    for (const auto& point : pointsD1) {
//        cout << "Point: ";
//        for (int i = 0; i < point.dimension(); ++i) {
//            cout << point[i] << " ";
//        }
//        cout << endl;
//    }
//
//    cout<<"pointsD2: "<<endl;
//    for (const auto& point : pointsD2) {
//        cout << "Point: ";
//        for (int i = 0; i < point.dimension(); ++i) {
//            cout << point[i] << " ";
//        }
//        cout << endl;
//    }

    auto Construction_middle1 = std::chrono::high_resolution_clock::now();

    truncatedST.lightNodes.clear();
    truncatedST.clearLeaves();

    auto Construction_middle2 = std::chrono::high_resolution_clock::now();


//    long long KD_start = memory_usage();


#ifdef COUNT
    KDTreeCounting KD_D1(pointsD1);
    KDTreeCounting KD_D2(pointsD2);

#else

    KDTreeReporting KD_D1(pointsD1);
    KDTreeReporting KD_D2(pointsD2);
#endif




//    cout<<KD_D2.isTreeBalancedBySize(KD_D2.root)<<endl;

    //    long long KD_end = memory_usage();



    auto Construction_end = std::chrono::high_resolution_clock::now();

    cout<<"pointsD1.size(): "<<pointsD1.size()<<endl;

    cout<<"pointsD2.size(): "<<pointsD2.size()<<endl;
    cout<<"preorderID2KDTree.size(): "<<preorderID2KDTree.size()<<endl;

    double Construction_time = std::chrono::duration_cast < std::chrono::microseconds > (Construction_middle1 - Construction_start).count()*0.000001 + std::chrono::duration_cast < std::chrono::microseconds > (Construction_end - Construction_middle2).count()*0.000001;


    double LZ77_time = std::chrono::duration_cast < std::chrono::microseconds > (LZ77_end - LZ77_start).count()*0.000001;

    pointsD1.clear();
    pointsD2.clear();
    pointsD1.shrink_to_fit();
    pointsD2.shrink_to_fit();

    std::vector<Point>().swap(pointsD1);
    std::vector<Point>().swap(pointsD2);


    long long IndexSpace_end = memory_usage();

    long long memory_Index = IndexSpace_end - IndexSpace_start;
//    long long memory_ST = ST_end - ST_start;
//    long long memory_trunST = trunST_end - trunST_start;
//    long long memory_KD = KD_l +KD_end - KD_start;

    cout<<"====================================== counting index construction=============================="<<endl;

    cout<<"Index memory of counting: "<< memory_Index / (1024.0 * 1024.0)<<"MB"<<endl;
//    cout<<"ST memory: "<< memory_ST / (1024.0 * 1024.0)<<"MB"<<endl;
//    cout<<"truncated ST memory: "<< memory_trunST / (1024.0 * 1024.0)<<"MB"<<endl;
//    cout<<"KD memory: "<< memory_KD / (1024.0 * 1024.0)<<"MB"<<endl;

    cout<<"Index construction time: "<< Construction_time<< " seconds"<<endl;

    cout<<"LZ77 time: "<< LZ77_time<< " seconds"<<endl;
    cout<<"Total construction time: "<< Construction_time + LZ77_time<< " seconds"<<endl;



    // find the node which represents u_p

    /*reversed pattern*/

    cout<<"====================Starting multiple queries saved in "<<patternPath<<"========================"<<endl;

    const INT patternsVecSize = patterns.size();
    for (INT i =0; i< patternsVecSize; i ++){

        INT x = contextSizes[i].first;

        INT y = contextSizes[i].second;

        unsigned char* pattern_rev = reversePattern(patterns[i]);

        auto query_start = std::chrono::high_resolution_clock::now();

        stNode * up = truncatedST.forward_search(patterns[i], patternSizes[i]);
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

            vector<pair<INT, INT>> ranges_D1 = { { patternSizes[i] +y,  new_string_size},{ x, new_string_size},{ up->preorderId,  rightpreorderId}, {0, patternSizes[i] +y},{0, x}};

//            vector<pair<INT, INT>> ranges_D1 = {{ up->preorderId,  rightpreorderId}, {0, x}, { x, new_string_size},
//                                                {0, patternSizes[i] +y},{ patternSizes[i] +y,  new_string_size}};

//            for (const auto& range : ranges_D1) {
//                cout << "{" << range.first << ", " << range.second << "}" << endl;
//            }


            INT result_D1 = KD_D1.rangeSearch(ranges_D1);
#ifdef VERBOSE
        cout<<"result_D1 = "<<result_D1<<endl;

        cout<<"---------------------------------------"<<endl;
#endif
            //D_2


            vector<pair<INT, INT>> ranges_D2 = {{ patternSizes[i] +y,  new_string_size},{ x, new_string_size},{ up->preorderId,  rightpreorderId},  {0, patternSizes[i] +y}, {0, x}};

//            vector<pair<INT, INT>> ranges_D2 = {{ up->preorderId,  rightpreorderId}, {0, x}, { x, new_string_size},
//                                                {0, patternSizes[i] +y},{ patternSizes[i] +y,  new_string_size}};

//            for (const auto& range : ranges_D2) {
//                cout << "{" << range.first << ", " << range.second << "}" << endl;
//            }


            INT result_D2 = KD_D2.rangeSearch(ranges_D2);

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


            vector<pair<INT, INT>> ranges_Dl = {{  patternSizes[i] + y,  new_string_size}, { x, new_string_size},{0,  x} };

//            for (const auto& range : ranges_Dl) {
//                cout << "{" << range.first << ", " << range.second << "}" << endl;
//            }


            INT result_Dl =0;
            if (preorderID2KDTree[lowest_ul->preorderId] != nullptr){
                result_Dl = preorderID2KDTree[lowest_ul->preorderId]->rangeSearch(ranges_Dl);
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



//            vector<pair<INT, INT>> ranges_D1 = {{ up->preorderId,  rightpreorderId}, {0, x}, { x, new_string_size},
//                                                {0, patternSizes[i] +y},{ patternSizes[i] +y,  new_string_size}};
//            vector<pair<INT, INT>> ranges_D1 = {{0, x},{0, patternSizes[i] +y}, { up->preorderId,  rightpreorderId}, { x, new_string_size},
//                                                { patternSizes[i] +y,  new_string_size}};
            vector<pair<INT, INT>> ranges_D1 = { { patternSizes[i] +y,  new_string_size},{ x, new_string_size},{ up->preorderId,  rightpreorderId}, {0, patternSizes[i] +y},{0, x}};


//            for (const auto& range : ranges_D1) {
//                cout << "{" << range.first << ", " << range.second << "}" << endl;
//            }


            INT result_D1 = KD_D1.rangeSearch(ranges_D1);
#ifdef VERBOSE
            cout<<"result_D1 = "<<result_D1<<endl;

        cout<<"---------------------------------------"<<endl;
#endif

            //D_2


//            vector<pair<INT, INT>> ranges_D2 = {{0, x}, {0, patternSizes[i] +y},{ up->preorderId,  rightpreorderId}, { x, new_string_size},
//                                                { patternSizes[i] +y,  new_string_size}};
            vector<pair<INT, INT>> ranges_D2 = {{ patternSizes[i] +y,  new_string_size},{ x, new_string_size},{ up->preorderId,  rightpreorderId},  {0, patternSizes[i] +y}, {0, x}};



//            for (const auto& range : ranges_D2) {
//                cout << "{" << range.first << ", " << range.second << "}" << endl;
//            }

            INT result_D2 = KD_D2.rangeSearch(ranges_D2);


            counts = result_D1 + result_D2;

#ifdef VERBOSE

            cout<<"result_D2 = "<<result_D2<<endl;

        cout<<"---------------------------------------"<<endl;
#endif
        }


        auto query_end = std::chrono::high_resolution_clock::now();
        double query_time = std::chrono::duration_cast < std::chrono::microseconds > (query_end - query_start).count()*0.000001;


        cout<<"Pattern "<< i<<": "<<patterns[i]<<" and x = "<<x <<"; y = "<<y<<endl;


        cout<<"There are "<<counts<< " distinct XPY that occur in T."<<endl;

        cout<<"Time for query of counting: "<<query_time<<endl;
        cout<<"------------------------------------------------------"<<endl;

        free(pattern_rev);

    }







    for (auto& pair : preorderID2KDTree) {
        delete pair.second;
    }
    free(new_string_rev);

    free(text_string_wDollar);

    delete [] new_string;

    for (auto &it: patterns){
        free(it);
    }

    return 0;


}





