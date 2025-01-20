#ifndef CPM_SA_LCP_LCE_H
#define CPM_SA_LCP_LCE_H
#include <sdsl/bit_vectors.hpp>					  // include header for bit vectors
#include <sdsl/rmq_support.hpp>					  //include header for range minimum queries
using namespace sdsl;
#ifdef _USE_64
#include <divsufsort64.h>                                         // include header for suffix sort
typedef int64_t INT;
#endif

#ifdef _USE_32
#include <divsufsort.h>                                           // include header for suffix sort
typedef int32_t INT;

#endif

class SA_LCP_LCE
{


public:

    explicit SA_LCP_LCE(unsigned char* T,INT &text_size);
    void LCParray ();

//    INT LCE(rmq_succinct_sct<> &rmq, INT L,INT R);
    unsigned char* T;

    INT * SA;
    INT * invSA;
    INT * LCP;
    sdsl::rmq_succinct_sct<> rmq;  // Default initialization
    INT LCE(INT L,INT R);

    INT text_size;
    ~SA_LCP_LCE();
};

#endif
