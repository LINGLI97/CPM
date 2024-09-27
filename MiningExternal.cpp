
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
    int i, j;

    TwoInteger() { }

    TwoInteger(int _i, int _j)
            : i(_i), j(_j)
    { }
};


struct TwoIntegerComparator
{
    enum CompareMode { COMPARE_BY_I, COMPARE_BY_J };

    CompareMode mode;

    TwoIntegerComparator(CompareMode _mode = COMPARE_BY_I)
            : mode(_mode)
    { }

    bool operator() (const TwoInteger& a, const TwoInteger& b) const
    {
        if (mode == COMPARE_BY_I)
            return a.i < b.i;
        else
            return a.j < b.j;
    }

    TwoInteger min_value() const
    {
        return TwoInteger(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    }

    TwoInteger max_value() const
    {
        return TwoInteger(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    }

    void set_mode(CompareMode _mode) {
        mode = _mode;
    }
};


struct FourInteger {
    int i, j, k, l;

    FourInteger() { }

    FourInteger(int _i, int _j, int _k, int _l)
            : i(_i), j(_j), k(_k), l(_l)
    { }
};


struct FourIntegerComparator {
    enum CompareMode { COMPARE_BY_I, COMPARE_BY_J, COMPARE_BY_K, COMPARE_BY_L };

    CompareMode mode;

    FourIntegerComparator(CompareMode _mode = COMPARE_BY_I)
            : mode(_mode)
    { }

    bool operator() (const FourInteger& a, const FourInteger& b) const {
        switch (mode) {
            case COMPARE_BY_I: return a.i < b.i;
            case COMPARE_BY_J: return a.j < b.j;
            case COMPARE_BY_K: return a.k < b.k;
            case COMPARE_BY_L: return a.l < b.l;
        }
        return false;
    }

    FourInteger min_value() const {
        return FourInteger(std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min());
    }

    FourInteger max_value() const {
        return FourInteger(std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max());
    }

    void set_mode(CompareMode _mode) {
        mode = _mode;
    }
};




struct FiveInteger {
    int i, j, k, l, m;

    FiveInteger() { }

    FiveInteger(int _i, int _j, int _k, int _l, int _m)
            : i(_i), j(_j), k(_k), l(_l), m(_m)
    { }
};


struct FiveIntegerComparator {
    enum CompareMode { COMPARE_BY_I, COMPARE_BY_J, COMPARE_BY_K, COMPARE_BY_L, COMPARE_BY_M };

    CompareMode mode;

    FiveIntegerComparator(CompareMode _mode = COMPARE_BY_I)
            : mode(_mode)
    { }

    bool operator() (const FiveInteger& a, const FiveInteger& b) const {
        switch (mode) {
            case COMPARE_BY_I: return a.i < b.i;
            case COMPARE_BY_J: return a.j < b.j;
            case COMPARE_BY_K: return a.k < b.k;
            case COMPARE_BY_L: return a.l < b.l;
            case COMPARE_BY_M: return a.m < b.m;
        }
        return false;
    }

    FiveInteger min_value() const {
        return FiveInteger(std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min(),
                           std::numeric_limits<int>::min());
    }

    FiveInteger max_value() const {
        return FiveInteger(std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max(),
                           std::numeric_limits<int>::max());
    }

    void set_mode(CompareMode _mode) {
        mode = _mode;
    }
};


void reverse_file(const std::string& input_file, const std::string& output_file) {

    std::streamsize BUFFER_SIZE = 4096;
    std::ifstream infile(input_file, std::ios::binary | std::ios::ate);
    if (!infile) {
        std::cerr << "Error opening input file: " << input_file << std::endl;
        return;
    }

    // 获取文件大小
    std::streamsize file_size = infile.tellg();

    // 打开输出文件
    std::ofstream outfile(output_file, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error opening output file: " << output_file << std::endl;
        return;
    }

    // 开始从文件末尾倒序读取并写入到输出文件
    std::streamsize remaining_size = file_size;

    while (remaining_size > 0) {
        // 每次读取最大为 BUFFER_SIZE 的块，最后一块可能小于 BUFFER_SIZE
        std::streamsize read_size = std::min(remaining_size, BUFFER_SIZE);

        // 调整文件指针，移动到当前要读取的块的开头位置
        infile.seekg(remaining_size - read_size, std::ios::beg);

        // 读取块数据
        std::vector<char> buffer(read_size);
        infile.read(buffer.data(), read_size);

        // 逆序写入输出文件
        std::reverse(buffer.begin(), buffer.end());
        outfile.write(buffer.data(), read_size);

        // 更新剩余未处理的数据大小
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
//    parser.add<string>("pattern", 'p', "the path to pattern file", false, "pattern.txt");
    parser.add<int>("X", 'x', "the length of context, |X| = x ", false, 1);
    parser.add<int>("Y", 'y', "the length of context, |Y| = y ", false, 1);
    parser.add<int>("RAM", 'r', "the usage of RAM (Mi)", false, 1024);
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



    std::ifstream is_text(filename, std::ios::binary);
    if (!is_text) {
        std::cerr << "Error opening input file: " << filename << std::endl;

    }

// char : file size = the length of n
    is_text.seekg(0, std::ios::end);
    INT text_size = is_text.tellg();
    is_text.seekg(0, std::ios::beg);




    is_text.close();



    // reverse the string by block
    string filename_reversed = filename + "_reverse";
    std::ifstream file(filename_reversed);
    if (!file){
        reverse_file(filename, filename_reversed);
    }








    string sa_fname = index_name + "_SA.sa5";

    ifstream in_SA(sa_fname, ios::binary);

    if(!(in_SA))
    {


        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }


        std::filesystem::path full_path(fullpathstart);
        std::filesystem::path dir = full_path.parent_path();


        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/psascan/construct_sa "
                       << filename << " "
                       << "-m " << ram_use << "Mi "
                       << "-o " << sa_fname;


        std::string commandesa = command_stream.str();


        int outsa = system(commandesa.c_str());


    }


    string lcp_fname = index_name + "_LCP.lcp5";
    ifstream in_LCP(lcp_fname, ios::binary);

    if( !(in_LCP ) )
    {

        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }

        // C++17
        std::filesystem::path full_path(fullpathstart);
        std::filesystem::path dir = full_path.parent_path();


        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/EM-SparsePhi-0.2.0/construct_lcp_parallel "
                       << "-m " << ram_use << "Mi "
                       << "-o " << lcp_fname << " "
                       << "-s " << sa_fname << " "
                       << filename;

        std::string command = command_stream.str();
        int result = system(command.c_str());

    }

    string sa_fname_reverse = index_name +"_reverse_SA.sa5";
    ifstream in_SA_reverse(sa_fname_reverse, ios::binary);

    if ( !(in_SA_reverse)  )
    {
        char fullpathstart_reverse[PATH_MAX];
        if (realpath(argv[0], fullpathstart_reverse) == NULL) {
            perror("realpath");
            return 1;
        }

        std::filesystem::path full_path_reverse(fullpathstart_reverse);
        std::filesystem::path dir_reverse = full_path_reverse.parent_path();

        std::stringstream command_stream_reverse;
        command_stream_reverse << dir_reverse.string()
                               << "/psascan/construct_sa "
                               << filename_reversed << " "
                               << "-m " << ram_use << "Mi "
                               << "-o " << sa_fname_reverse;

        std::string commandesa_reverse = command_stream_reverse.str();

        int outsa_reverse = system(commandesa_reverse.c_str());

    }



    string lcp_fname_reverse = index_name + "_reverse_LCP.lcp5";
    ifstream in_LCP_reverse(lcp_fname_reverse, ios::binary);

    if( !(in_LCP_reverse ) )
    {

        char fullpathstart[PATH_MAX];
        if (realpath(argv[0], fullpathstart) == NULL) {
            perror("realpath");
            return 1;
        }

        // C++17
        std::filesystem::path full_path(fullpathstart);
        std::filesystem::path dir = full_path.parent_path();


        std::stringstream command_stream;
        command_stream << dir.string()
                       << "/EM-SparsePhi-0.2.0/construct_lcp_parallel "
                       << "-m " << ram_use << "Mi "
                       << "-o " << lcp_fname_reverse << " "
                       << "-s " << sa_fname_reverse << " "
                       << filename_reversed;

        std::string command = command_stream.str();
        int result = system(command.c_str());

    }




    stream_reader<uint40>* SA =  new stream_reader <uint40> (sa_fname, ram_use);
    stream_reader<uint40>* LCP =  new stream_reader <uint40> (lcp_fname, ram_use);

    INT currSA = 0;
    INT currLCP = 0;

    INT id_B =0;
    INT id_SB = 0;
    INT id_RB = 0;

    typedef stxxl::sorter<FourInteger, FourIntegerComparator> FourIntegerSorter;


    FourIntegerSorter four_sorter(FourIntegerComparator(FourIntegerComparator::COMPARE_BY_I), ram_use * 1024 * 1024);


    for( INT i = 0; i < text_size; i++ )
    {


        currSA = SA->read();
        currLCP = LCP->read();

        if (currLCP < m + y){

            id_SB ++;

            if (currLCP < m){

                id_B ++;

            }

        }
        four_sorter.push(FourInteger(currSA,i,id_B,id_SB));


    }
    four_sorter.sort();


    stream_reader<uint40>* SA_rev =  new stream_reader <uint40> (sa_fname_reverse, ram_use);
    stream_reader<uint40>* LCP_rev =  new stream_reader <uint40> (lcp_fname_reverse, ram_use);

    INT currSA_rev = 0;
    INT currLCP_rev = 0;
    typedef stxxl::sorter<TwoInteger, TwoIntegerComparator> TwoIntegerSorter;

    TwoIntegerSorter two_sorter(TwoIntegerComparator(TwoIntegerComparator::COMPARE_BY_I), ram_use * 1024 * 1024);



    for( INT i = 0; i < text_size; i++ )
    {


        currSA_rev = SA_rev->read();
        currLCP_rev = LCP_rev->read();


        if (currLCP_rev < x){

            id_RB ++;

        }
        two_sorter.push(TwoInteger(text_size - currSA_rev, id_RB));


    }

    two_sorter.sort();

    typedef stxxl::sorter<FiveInteger, FiveIntegerComparator> FiveIntegerSorter;

    FiveIntegerSorter five_sorter(FiveIntegerComparator(FiveIntegerComparator::COMPARE_BY_J), ram_use * 1024 * 1024);


    ++four_sorter;


    while (!two_sorter.empty() and !four_sorter.empty())
    {

        assert(four_sorter->i == two_sorter->i);
        five_sorter.push(FiveInteger(four_sorter->i,four_sorter->j,four_sorter->k, four_sorter->l,two_sorter->j));
        ++two_sorter;
        ++four_sorter;

    }

    five_sorter.sort();
//    std::cout << "Sorted by j:" << std::endl;
//    while (!five_sorter.empty()) {
//        const FiveInteger& item = *five_sorter;
//        std::cout << "i: " << item.i << ", j: " << item.j << ", k: " << item.k << ", l: " << item.l << ", m: " << item.m << std::endl;
//        ++five_sorter;
//    }

    INT counts = 0;
    INT b_pre = five_sorter->k;
    INT sb_pre =five_sorter->l;
    INT rb_pre =five_sorter->m;



    if (five_sorter->i -x > -1 and five_sorter->i + m + y -1 < text_size){
        counts ++;
    }
    ++five_sorter;

    while (!five_sorter.empty()) {
        INT b = five_sorter->k;
        INT sb = five_sorter->l;
        INT rb = five_sorter->m;

        if (b_pre != b or sb_pre != sb or rb_pre != rb){
            if (five_sorter->i -x > -1 and five_sorter->i + m + y -1< text_size) {
                counts++;
            }
        }
        b_pre = b;
        sb_pre = sb;
        rb_pre = rb;

        ++five_sorter;
    }
    cout<<"There are "<<counts<<" distinct XPY substrings."<<endl;

    /*free the memory*/

    delete(SA);
    delete(LCP);
    delete(SA_rev);
    delete(LCP_rev);




}