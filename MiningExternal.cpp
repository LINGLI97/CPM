
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
#include <iostream>
#include <unistd.h>    // for realpath
#include <filesystem>  // for std::filesystem::path (C++17)
#include <cstdlib>     // for system
#include <malloc.h>
//stxxl
#include <stxxl/sorter>
#include <stxxl/stats>
#include <stxxl/timer>
#include <stxxl/random>
#include <limits>

//sdsl
#include <sdsl/bit_vectors.hpp>					  // include header for bit vectors
#include <sdsl/rmq_support.hpp>					  //include header for range minimum queries
//#include "stack.h"        					  // include header for stack structure

#ifdef _USE_64
#include <divsufsort64.h>                                         // include header for suffix sort
#endif

#ifdef _USE_32
#include <divsufsort.h>                                           // include header for suffix sort
#endif



using namespace sdsl;
using namespace std;



long long memory_usage() {
    struct mallinfo2 mi = mallinfo2();
    return mi.uordblks + mi.hblkhd;
}





struct TwoInteger
{
    INT i, j;

    TwoInteger() { }

    TwoInteger(INT _i, INT _j)
            : i(_i), j(_j)
    { }
};


struct TwoIntegerComparator
{


    bool operator() (const TwoInteger& a, const TwoInteger& b) const
    {
        return a.i < b.i;

    }

    TwoInteger min_value() const
    {
        return TwoInteger(std::numeric_limits<INT>::min(), std::numeric_limits<INT>::min());
    }

    TwoInteger max_value() const
    {
        return TwoInteger(std::numeric_limits<INT>::max(), std::numeric_limits<INT>::max());
    }


};


struct FourInteger {
    INT i, j, k, l;

    FourInteger() { }

    FourInteger(INT _i, INT _j, INT _k, INT _l)
            : i(_i), j(_j), k(_k), l(_l)
    { }
};


struct FourIntegerComparatorByi {

    bool operator() (const FourInteger& a, const FourInteger& b) const {
        return a.i < b.i;
    }

    FourInteger min_value() const {
        return FourInteger(std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min());
    }

    FourInteger max_value() const {
        return FourInteger(std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max());
    }

};



struct FourIntegerComparatorByl {

    bool operator() (const FourInteger& a, const FourInteger& b) const {
        return a.l < b.l;
    }

    FourInteger min_value() const {
        return FourInteger(std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min());
    }

    FourInteger max_value() const {
        return FourInteger(std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max());
    }

};



struct FiveInteger {
    INT i, j, k, l, m;

    FiveInteger() { }

    FiveInteger(INT _i, INT _j, INT _k, INT _l, INT _m)
            : i(_i), j(_j), k(_k), l(_l), m(_m)
    { }
};


struct FiveIntegerComparator {

    bool operator() (const FiveInteger& a, const FiveInteger& b) const {
        return a.j < b.j;
    }

    FiveInteger min_value() const {
        return FiveInteger(std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min(),
                           std::numeric_limits<INT>::min());
    }

    FiveInteger max_value() const {
        return FiveInteger(std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max(),
                           std::numeric_limits<INT>::max());
    }


};

void reverse_file(const std::string& input_file, const std::string& output_file) {

    std::streamsize BUFFER_SIZE = 4096;
    std::ifstream infile(input_file, std::ios::binary | std::ios::ate);
    if (!infile) {
        std::cerr << "Error opening input file: " << input_file << std::endl;
        return;
    }


    std::streamsize file_size = infile.tellg();


    std::ofstream outfile(output_file, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error opening output file: " << output_file << std::endl;
        return;
    }


    std::streamsize remaining_size = file_size;

    while (remaining_size > 0) {

        std::streamsize read_size = std::min(remaining_size, BUFFER_SIZE);

        infile.seekg(remaining_size - read_size, std::ios::beg);


        std::vector<char> buffer(read_size);
        infile.read(buffer.data(), read_size);

        std::reverse(buffer.begin(), buffer.end());
        outfile.write(buffer.data(), read_size);


        remaining_size -= read_size;
    }

    std::cout << "Successfully reversed the content of " << input_file << " and wrote to " << output_file << std::endl;
}

int main (int argc, char *argv[])
{

    /* get the specified parameters from parser */

    cmdline::parser parser;
    parser.add<string>("filepath", 'f', "the path to input file", false, "input.txt");
    parser.add<string>("index", 'i', "the name of index", false, "index");
    parser.add<string>("disk", 'd', "the name of disk file", false, "out");
    parser.add<int>("sizeofDisk", 's', "the size of disk used in stxxl, 200 GB", false, 200);
    parser.add<int>("countofDisk", 'c', "the number of disk used in stxxl", false, 10);

//    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
    parser.add<int>("X", 'x', "the length of context, |X| = x ", false, 1);
    parser.add<int>("Y", 'y', "the length of context, |Y| = y ", false, 1);
//    parser.add<int>("RAM", 'r', "the usage of RAM (Mi)", false, 8);
    parser.add<int>("RAM", 'r', "the usage of RAM (Gi)", false, 8);

//    the size of the block (Mi) B is 2 Mi
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



    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        std::cerr << "Error getting current working directory\n";
        return 1;
    }

    std::string cwdStr(cwd);

    // Find the last slash '/' in the path to get the parent directory
    size_t pos = cwdStr.find_last_of('/');

    std::filesystem::path dir = cwdStr;
    cout<<dir<<endl;

    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;

    }

// char : file size = the length of n
    is_text.seekg(0, std::ios::end);
    INT text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);

    cout<<"Text length = "<<text_size<< endl;

    is_text.close();



    // reverse the string by block
    string filename_reversed = filename + "_reverse";
    std::ifstream file(filename_reversed);
    if (!file){
        cout<<"reversed input does not exist and need to create it."<<endl;
        reverse_file(filename, filename_reversed);
    }








    string sa_fname = index_name + "_SA.sa5";

    ifstream in_SA(sa_fname, ios::binary);
    auto start = std::chrono::high_resolution_clock::now();

    auto SA_start = std::chrono::high_resolution_clock::now();

    if(!(in_SA))
    {


        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }


//        std::filesystem::path full_path(fullpathstart);
//        std::filesystem::path dir = full_path.parent_path();

        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/psascan/construct_sa "
                       << filename << " "
                       << "-m " << ram_use << "Gi "
                       << "-o " << sa_fname;


        std::string commandesa = command_stream.str();
//
//        cout<<commandesa<<endl;
        int outsa = system(commandesa.c_str());

        cout<<"==================================Suffix array of input was constructed successfully!=================================="<<endl;

    }

    auto SA_end = std::chrono::high_resolution_clock::now();

    string lcp_fname = index_name + "_LCP.lcp5";
    ifstream in_LCP(lcp_fname, ios::binary);
    auto LCP_start = std::chrono::high_resolution_clock::now();

    if( !(in_LCP ) )
    {

        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }

        // C++17
//        std::filesystem::path full_path(fullpathstart);
//        std::filesystem::path dir = full_path.parent_path();


        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/SparsePhi/construct_lcp_parallel "
                       << "-m " << ram_use << "Gi "
                       << "-o " << lcp_fname << " "
                       << "-s " << sa_fname << " "
                       << filename;

        std::string command = command_stream.str();
        int result = system(command.c_str());

        cout<<"==================================LCP of input was constructed successfully!=================================="<<endl;


    }
    auto LCP_end = std::chrono::high_resolution_clock::now();

    string sa_fname_reverse = index_name +"_reverse_SA.sa5";
    ifstream in_SA_reverse(sa_fname_reverse, ios::binary);
    auto SA_rev_start = std::chrono::high_resolution_clock::now();

    if ( !(in_SA_reverse)  )
    {
        char fullpathstart_reverse[PATH_MAX];
        if (realpath(argv[0], fullpathstart_reverse) == NULL) {
            perror("realpath");
            return 1;
        }

//        std::filesystem::path full_path_reverse(fullpathstart_reverse);
//        std::filesystem::path dir_reverse = full_path_reverse.parent_path();


        std::stringstream command_stream_reverse;
        command_stream_reverse << dir.string()
                               << "/psascan/construct_sa "
                               << filename_reversed << " "
                               << "-m " << ram_use << "Gi "
                               << "-o " << sa_fname_reverse;

        std::string commandesa_reverse = command_stream_reverse.str();

        int outsa_reverse = system(commandesa_reverse.c_str());
        cout<<"==================================Suffix array of reversed input was constructed successfully!=================================="<<endl;


    }
    auto SA_rev_end = std::chrono::high_resolution_clock::now();



    string lcp_fname_reverse = index_name + "_reverse_LCP.lcp5";
    ifstream in_LCP_reverse(lcp_fname_reverse, ios::binary);
    auto LCP_rev_start = std::chrono::high_resolution_clock::now();

    if( !(in_LCP_reverse ) )
    {

        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }

        // C++17
//        std::filesystem::path full_path(fullpathstart);
//        std::filesystem::path dir = full_path.parent_path();


        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/SparsePhi/construct_lcp_parallel "
                       << "-m " << ram_use << "Gi "
                       << "-o " << lcp_fname_reverse << " "
                       << "-s " << sa_fname_reverse << " "
                       << filename_reversed;

        std::string command = command_stream.str();
        int result = system(command.c_str());
        cout<<"==================================LCP of reversed input was constructed successfully!=================================="<<endl;

    }
    auto LCP_rev_end = std::chrono::high_resolution_clock::now();



//
    stream_reader<uint40>* SA =  new stream_reader <uint40> (sa_fname, ram_use * 1024 / 6);
    stream_reader<uint40>* LCP =  new stream_reader <uint40> (lcp_fname, ram_use * 1024 /6);



//    stream_reader<uint40>* SA =  new stream_reader <uint40> (sa_fname, ram_use/4);
//    stream_reader<uint40>* LCP =  new stream_reader <uint40> (lcp_fname, ram_use/4);
    INT currSA = 0;
    INT currLCP = 0;

    INT id_B =0;
    INT id_SB = 0;
    INT id_RB = 0;



    std::string disk_prefix = parser.get<std::string>("disk");

    INT sizeofDisk = parser.get<int>("sizeofDisk");
    long long total_memory_gb = sizeofDisk * 1024 * 1024 * 1024;


    int disk_count = parser.get<int>("countofDisk");
    long long memory_per_disk_gb = total_memory_gb / disk_count;

    stxxl::config* cfg = stxxl::config::get_instance();

    // multiple disks
    for (int i = 0; i < disk_count; ++i) {
        std::string full_disk_path = std::string(cwd) + "/" + disk_prefix + "_" + std::to_string(i+1);
        cfg->add_disk(stxxl::disk_config(full_disk_path, memory_per_disk_gb, "linuxaio unlink"));
    }

    unsigned long long int memory_limit =ram_use * 1024 * 1024 * 1024 / 6;

//    unsigned long long int memory_limit =ram_use  * 1024 * 1024 / 4;

    cout<< "1 /6 Ram memory_limit: "<<memory_limit<< endl;
    typedef stxxl::sorter<FourInteger, FourIntegerComparatorByi> FourIntegerSorterByi;


    FourIntegerSorterByi four_sorterByi(FourIntegerComparatorByi(), memory_limit);



    auto sorting_start = std::chrono::high_resolution_clock::now();



    for( INT i = 0; i < text_size; i++ )
    {


        currSA = SA->read();
        currLCP = LCP->read();
//        if (currSA < 0){
//            cout<<i <<": "<<currSA<<endl;
//        }
        if (currLCP < m + y){

            id_SB ++;

            if (currLCP < m){

                id_B ++;

            }

        }
        four_sorterByi.push(FourInteger(currSA,i,id_B,id_SB));

//            std::cout <<currSA << " , " << i << ", " << id_B << ", " << id_SB << std::endl;


    }

    delete(SA);
    delete(LCP);
    four_sorterByi.sort();

    cout<<"four_sorter.sort() was finished"<<endl;


    stream_reader<uint40>* SA_rev =  new stream_reader <uint40> (sa_fname_reverse, ram_use* 1024 / 6);
    stream_reader<uint40>* LCP_rev =  new stream_reader <uint40> (lcp_fname_reverse, ram_use* 1024 /6);
//    stream_reader<uint40>* SA_rev =  new stream_reader <uint40> (sa_fname_reverse, ram_use/4);
//    stream_reader<uint40>* LCP_rev =  new stream_reader <uint40> (lcp_fname_reverse, ram_use/4);
    INT currSA_rev = 0;
    INT currLCP_rev = 0;
    typedef stxxl::sorter<TwoInteger, TwoIntegerComparator> TwoIntegerSorter;

    TwoIntegerSorter two_sorter(TwoIntegerComparator(),  memory_limit);



    for( INT i = 0; i < text_size; i++ )
    {


        currSA_rev = SA_rev->read();
        currLCP_rev = LCP_rev->read();


        if (currLCP_rev < x){

            id_RB ++;

        }
        two_sorter.push(TwoInteger(text_size - currSA_rev, id_RB));


    }

    delete(SA_rev);
    delete(LCP_rev);

    two_sorter.sort();
    cout<<"two_sorter.sort() was finished"<<endl;

    typedef stxxl::sorter<FiveInteger, FiveIntegerComparator> FiveIntegerSorter;

    FiveIntegerSorter five_sorter(FiveIntegerComparator(),  memory_limit);


    ++four_sorterByi;



    while (!two_sorter.empty() and !four_sorterByi.empty())
    {

//        if (four_sorter->i != two_sorter->i){
//            cout<<four_sorter->i <<" : "<<two_sorter->i<<endl;
//            return -1;
//        }
//        assert(four_sorterByi->i == two_sorter->i);

        five_sorter.push(FiveInteger(four_sorterByi->i,four_sorterByi->j,four_sorterByi->k, four_sorterByi->l,two_sorter->j));
//        std::cout << four_sorterByi->i<<", " << four_sorterByi->j <<", " << four_sorterByi->k<<", " << four_sorterByi->l <<", " << two_sorter->j<< std::endl;

        ++two_sorter;
        ++four_sorterByi;


    }

    // clear two_sorter and four_sorter immediately
    two_sorter.finish_clear();
    four_sorterByi.finish_clear();


    five_sorter.sort();
    cout<<"five_sorter.sort() was finished"<<endl;

//    std::cout << "Sorted by j:" << std::endl;
//    while (!five_sorter.empty()) {
//        const FiveInteger& item = *five_sorter;
//        std::cout << "i: " << item.i << ", j: " << item.j << ", k: " << item.k << ", l: " << item.l << ", m: " << item.m << std::endl;
//        ++five_sorter;
//    }

    INT counts = 0;
    INT SA_pre = five_sorter->i;

    INT b_pre = five_sorter->k;
    INT sb_pre =five_sorter->l;
    INT rb_pre =five_sorter->m;


    typedef stxxl::sorter<FourInteger,FourIntegerComparatorByl > FourIntegerSorterByl;

    FourIntegerSorterByl four_sorterByl(FourIntegerComparatorByl(), memory_limit);



    four_sorterByl.push(FourInteger(SA_pre, b_pre, sb_pre, rb_pre));
    INT cnt = 1;
    ++five_sorter;

    while (!five_sorter.empty()) {
        INT SA = five_sorter->i;
        INT b = five_sorter->k;
        INT sb = five_sorter->l;
        INT rb = five_sorter->m;

        if (b != b_pre || sb != sb_pre) {
            // if the third and fourth element are different

            if (cnt == 1){
                if (SA_pre - x > -1 and SA_pre + m + y - 1 < text_size) {
                    counts++;
                }
            }else{
                four_sorterByl.sort();
                INT rb_pre = four_sorterByl->l;
                if (four_sorterByl->i -x > -1 and four_sorterByl->i + m + y -1< text_size) {
                    counts++;
                }
                ++four_sorterByl;
                while (!four_sorterByl.empty()) {
                    if (rb_pre != four_sorterByl->l) {

                        if (four_sorterByl->i -x > -1 and four_sorterByl->i + m + y -1< text_size) {

                            counts++;
                        }
                    }
                    rb_pre = four_sorterByl->l;
                    ++four_sorterByl;
                }
            }

            four_sorterByl.clear(); // clear the four_sorterByl
            four_sorterByl.push(FourInteger(SA, b, sb, rb));
            cnt = 1;

        } else {
            // if the third and fourth element are total
            four_sorterByl.push(FourInteger(SA, b, sb, rb));
            cnt++;
        }

        SA_pre = SA;

        b_pre = b;
        sb_pre = sb;

        ++five_sorter;
    }

    // The last group

    if (cnt == 1){
        if (SA_pre - x > -1 and SA_pre + m + y - 1 < text_size) {
            counts++;
        }
    }else {
        four_sorterByl.sort();
        INT rb_pre_last = four_sorterByl->l;
        if (four_sorterByl->i - x > -1 and four_sorterByl->i + m + y - 1 < text_size) {
            counts++;
        }
        ++four_sorterByl;

        while (!four_sorterByl.empty()) {
            if (rb_pre_last != four_sorterByl->l) {
                if (four_sorterByl->i - x > -1 and four_sorterByl->i + m + y - 1 < text_size) {

                    counts++;
                }
            }
            rb_pre_last = four_sorterByl->l;
            ++four_sorterByl;
        }
    }
    four_sorterByl.finish_clear();


    auto sorting_end = std::chrono::high_resolution_clock::now();


    auto end = std::chrono::high_resolution_clock::now();

    cout<<"There are "<<counts<<" distinct XPY substrings."<<endl;

    double SA_time = std::chrono::duration_cast < std::chrono::microseconds > (SA_end - SA_start).count()*0.000001;
    double LCP_time = std::chrono::duration_cast < std::chrono::microseconds > (LCP_end - LCP_start).count()*0.000001;
    double SA_rev_time = std::chrono::duration_cast < std::chrono::microseconds > (SA_rev_end - SA_rev_start).count()*0.000001;
    double LCP_rev_time = std::chrono::duration_cast < std::chrono::microseconds > (LCP_rev_end - LCP_rev_start).count()*0.000001;
    double sorting_time = std::chrono::duration_cast < std::chrono::microseconds > (sorting_end - sorting_start).count()*0.000001;


    cout<< "Time for SA construction "<<SA_time<<endl;
    cout<< "Time for LCP construction "<<LCP_time<<endl;
    cout<< "Time for SA_rev construction "<<SA_rev_time<<endl;
    cout<< "Time for LCP_rev construction "<<LCP_rev_time<<endl;
    cout<< "Time for sorting "<<sorting_time<<endl;
    cout<< "Time for whole algorithm "<<SA_time + LCP_time + SA_rev_time + LCP_rev_time + sorting_time <<endl;






}