#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_set>
#include "SA_LCP_LCE.h"


using namespace std;

void SA_LCP_LCE::LCParray()
{
    INT i=0;
    INT j=0;

    this->LCP[0] = 0;
    for ( i = 0; i < this->text_size; i++ ) // compute LCP[ISA[i]]
        if ( this->invSA[i] != 0 )
        {
            if ( i == 0) j = 0;
            else j = (this->LCP[this->invSA[i-1]] >= 2) ? this->LCP[this->invSA[i-1]]-1 : 0;
            while ( this->T[i+j] == this->T[this->SA[this->invSA[i]-1]+j] )
                j++;
            this->LCP[this->invSA[i]] = j;
        }


}


SA_LCP_LCE::SA_LCP_LCE(unsigned char* T,INT &text_size){
    this->T = T;
    this->text_size = text_size;

    this->SA = ( INT * ) malloc( ( text_size ) * sizeof( INT ) );

    if( (this->SA == NULL) )
    {
        fprintf(stderr, " Error: Cannot allocate memory for SA.\n" );
    }
#ifdef _USE_64
    if( divsufsort64(this->T, this->SA,  this->text_size ) != 0 )
        {
            fprintf(stderr, " Error: SA computation failed.\n" );
            exit( EXIT_FAILURE );
        }
#endif

#ifdef _USE_32
    if( divsufsort( this->T, this->SA,  this->text_size ) != 0 )
        {
            fprintf(stderr, " Error: SA computation failed.\n" );
            exit( EXIT_FAILURE );
        }
#endif
/*Compute the inverse SA array */
    this->invSA = ( INT * ) malloc( ( this->text_size ) * sizeof( INT ) );

    if( ( this->invSA == NULL) )
    {
        fprintf(stderr, " Error: Cannot allocate memory for invSA.\n" );
    }

    for ( INT i = 0; i < this->text_size; i ++ )
    {
        this->invSA [this->SA[i]] = i;
    }

    this->LCP = ( INT * ) malloc  ( ( this->text_size ) * sizeof( INT ) );
    if( ( this->LCP == NULL) )
    {
        fprintf(stderr, " Error: Cannot allocate memory for LCP.\n" );
    }

    /* Compute the LCP array */
    this->LCParray();
    int_vector<> v1( text_size , 0 ); // create a vector of length n and initialize it with 0s


    for ( INT i = 0; i < text_size; i ++ )
    {
        v1[i] = this->LCP[i];
    }

    this->rmq = sdsl::rmq_succinct_sct<>(&v1);

    util::clear(v1);

//    this->rmq = sdsl::rmq_succinct_sct<>(&this->LCP);


}

INT SA_LCP_LCE::LCE(INT L, INT R) {
    // L R are the starting positions in SA
    if (L == R)
        return (this->text_size - L);

    //
    INT low = min(this->invSA[L], this->invSA[R]);
    INT high = max(this->invSA[L], this->invSA[R]);

    INT length = this->LCP[this->rmq(low + 1, high)];
    return length;

}

SA_LCP_LCE::~SA_LCP_LCE(){

    free(this->T);
    free(this->SA);
    free(this->invSA);
    free(this->LCP);
}
