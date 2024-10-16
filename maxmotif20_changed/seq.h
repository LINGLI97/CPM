/* SACHICA: Scalable Algorithm for Charactaristic and Homologineous Interval CAluclation */
/* 2004 Takeaki Uno */
// #include"stdlib2.c"

#ifndef _seq_h_
#define _seq_h_

#include"stdlib2.h"
#include"queue.h"

// if the following are defined, the radix sort deal with pperm of variations
// #define SEQ_PPERM_MASK 0x10000000 
// #define SEQ_PPERM_END 8

// for the change of the type, define SEQ_CHAR_SHORT, SEQ_SC_SHORT, SEQ_LONG_LONG, and SEQ_ID_SHORT

#ifndef SEQ_CHAR
#ifdef SEQ_CHAR_LONG
#define SEQ_CHAR unsigned long
  #define SEQ_CHAR_END 10000000
  #define SEQ_CHARF "%ld"
#elif defined SEQ_CHAR_SHORT
#define SEQ_CHAR unsigned short
  #define SEQ_CHAR_END 65536
  #define SEQ_CHARF "%hd"
#else
#define SEQ_CHAR unsigned char
#define SEQ_CHAR_END 256
#define SEQ_CHARF "%hhd"
#endif
#endif

#ifndef SEQ_LONG
#ifdef SEQ_LONG_LONG
#define SEQ_LONG unsigned long long
  #define SEQ_LONG_END LONGHUGE
  #define SEQ_LONGF "%llu"
#else
#define SEQ_LONG unsigned int
#define SEQ_LONG_END (((unsigned int)INTHUGE)*2)
#define SEQ_LONGF "%u"
#endif
#endif
#define SWAP_SEQ_LONG(a,b)  (common_SEQ_LONG=a,a=b,b=common_SEQ_LONG)

#ifndef SEQ_ID
#ifdef SEQ_ID_SHORT
#define SEQ_ID unsigned short
  #define SEQ_ID_END 65536
  #define SEQ_IDF "%hu"
#else
#define SEQ_ID unsigned int  // for string ID
#define SEQ_ID_END (((unsigned int)INTHUGE)*2)
#define SEQ_IDF "%u"
#endif
#endif

#ifndef SEQ_COST
#ifdef SEQ_COST_SHORT
#define SEQ_COST unsigned short
  #define SEQ_COST_END 65536
  #define SEQ_COSTF "%hu"
#else
#define SEQ_COST unsigned char  // for string ID
#define SEQ_COST_END 256
#define SEQ_COSTF "%hhu"
#endif
#endif

/* parameters */
#define SEQ_DEFAULT_END_EXTENSION 100000
#define SEQ_DEFAULT_SUBST_SKIP (16/sizeof(SEQ_CHAR))
#define SEQ_MULTI_BUFSIZ 16384

/* flags */
#define SEQ_OUTPUT_PAIR 4   // output positions of similar substrings
#define SEQ_IGNORE_NEWLINE 8  // ignore '\n'
#define SEQ_MULTI_STRING   16 // multi-strings separated by \n\n (output pairs from distinct strings)
#define SEQ_MULTI_OUTPUT 32   // multicore output mode
#define SEQ_NO_SUBST_ID 64    // do not make substID array
#define SEQ_OUTPUT_STRING 128 // output strings (in each output process)
#define SEQ_OUTPUT_INTERVAL 256 // output strings by its start/end positions

#define SEQ_NORMALIZE 512 // normalize positions of all strings
#define SEQ_OUTPUT_ID 1024   //  output string id for each output pair
#define SEQ_NO_OUTPUT_POSITION 2048  // not output positions (only ID's)
#define SEQ_STR_W 4096       // allocate srting weight array

#define SEQ_MAXIMAL 131072  //  output maximal intervals
#define SEQ_SEGMENT 262144    // segment mode, each segment is a (equal length) line in each string
#define SEQ_PUTC_NUM 1048576  //  regard each 2 or 4 bytes as a number (letter -32 => digits)
#define SEQ_HAMMING_DISTANCE 4194304  //  use Hamming distance for extension
#define SEQ_EDIT_DISTANCE 8388608  //  use edit distance for extension
#define SEQ_GENOME 33554432 // deal only genome letters (a<->t,g<->c for reverse direction)
#define SEQ_EUC 67108864    // read 16bit extended codes
#define SEQ_NUM 134217728    // read number sequences

typedef struct {
    unsigned char type;   // type
    char *fname;      // input file name
    INT flag;       // flag

    SEQ_CHAR *s;    // pointer to the string data
    SEQ_CHAR *s_org; // original pointer to the string data (for free command)
    INT end_extension;  // the extension of the buffer for s
    INT chr_num, chr_num2, chr_num3;   // number of charactors (of power 2 and 3)
    double match_prob;  // probability that two tandomly chosen letters are the same
    SEQ_LONG *chr_cnt;       // #letters of each character included in s
    SEQ_CHAR *perm, *unperm;  // character permutation of "original -> data" "data->original"
    SEQ_CHAR gperm[6];       // reverse direction permutation for genome mode
    SEQ_CHAR wildcard;  // specify wildcard character
    SEQ_LONG str_len;        // length of string data
    SEQ_LONG str_len_org;        // length of string data in original file
    SEQ_ID str_num, str_num_;    // number of substring in the string (str_num_ considers segments, and strings to be output), NOTICE!! ID of the 1st string is "1"
    SEQ_ID str_half, str_second, str_dup;  // ID of the center string, and the first string of the second file
    SEQ_LONG max_str, min_str;    // maximum/minimum length of substrings
    SEQ_LONG ave_str, ave2_str;    // average lengths of substrings

    SEQ_LONG *head, *tail;  // array for the positions of the heads/tails of each substring
    SEQ_LONG *clms;      // array for #columns of each string, for 2dimension mode
    SEQ_LONG min_clms, max_clms;  // maximum/minimum column number among strings
    SEQ_ID *subst;       // the ID of substring which includes the position of each letter
    //  (represented by the substring ID which includes the block of letters
    INT subst_skip;   // the size of the block, used in the pos
    // ID of j-th letter is subst[j/subst_skip]

    // for string separations and positions
    INT normalized_by;   // positions are normalized by [normalized by]th string
    INT separated_by;   // strings are separeted into two groups, by this number
    INT output_by;      // pairs, sc's of strings smaller than this will be output
    INT duplicated_by;      // strings bigger ID than this will be duplicated
    INT reversed_by;      // strings bigger ID than this will be reversed
    INT matrix_by;       // until this, the strings will be devided into blocks
    INT reduced_by;     // positions before this will be removed if its score has changed (valid for str_sc)
    INT segment_num, segment_end;   // #segments, ID up to output
    SEQ_LONG normalized_pos, separated_pos, output_pos, duplicated_pos, reversed_pos, matrix_pos, reversed2_pos, reduced_pos;   // boundary positions of the above four. reversed2 is for dealing with original and reversed strings as the same
    SEQ_LONG output_pos_fprint_pair;  // output_pos temporary used in fprint_pair
    SEQ_LONG pairwise_max;   // upper bound for the #position to be pairwise comparison
    SEQ_LONG base1, base2;   // numbers to be added to each position/string ID

    char *fname2, *wfname, *wfname2;    // filename of the input files
    WEIGHT *str_w;                     // string weight
    char separator;          // charactor used to separate the numbers in output

    // for pair extension and edit distance
    INT *edit_table;              // the table used by the dynamic programming
    INT edit_table_end;       // size of edit_table
    INT err_max;              // maximum absolute error for extension of similar pair
    INT extend;               // length of the extension of each similar pair
    double err_ratio;            // limit of error ratio on the extension of similar pair

    SEQ_COST **cost;
    INT micro_chr_num;   // a letter is a combination of this size of alphabet;
    INT micro_chr_dist;  // distance threshold for micro char mode

    // for pattern string
    SEQ_CHAR *pat;     // pattern string
    INT plen, plen_org;          // length of pattern string
    INT xlen, ylen;          // horizontal/virtical length of pattern string, in 2D mode
    INT perr, perr_org, perr_org2;          // #error letters can be in plen
    INT p_ext;  // the extension of extension to both dirction, when output a substring
    INT **pperm;       // position (multi) permutations
    INT plen_lb, plen_ub;   // lower/upper bounds for the strings (intervals) to be output
    INT perr_min, perr_max;  // set perr to this and increase it and execute main routine
    INT perr_inc;      // how much increment perr_org in the iteration

    // for interleaving
    INT interleave_len;     // length of interleave
    INT interleave_len_org;   // original length of interleave. operate as interleave mode if >1
//  INT interleave_tmp;   // tmporary used for checking canonicity (previous interleaving pair of opposite one
    INT pos_skip_a, pos_skip_b;      // ratio of array size of positions
} SEQ;

/* qsort headers */
QSORT_TYPE_HEADER(SEQ_LONG, SEQ_LONG)
QSORT_TYPE_HEADER(SEQ_ID, SEQ_ID)
QSORT_TYPE_HEADER(SEQ_CHAR, SEQ_CHAR)
QSORT_TYPE_HEADER(SEQ_COST, SEQ_COST)

/* print profile of seq */
void SEQ_prop_print (SEQ *S);
/* initialization of SEQ. This should be called before loading the string data */
void SEQ_init (SEQ *S);
/* before this, dep_max has to be determined independently (and, pos_dep) */
/* before this, partition has to be determined */
void SEQ_set_param (SEQ *S);
/* terminate routine */
void SEQ_end (SEQ *S);

/* setting patern permutation */
void SEQ_set_pperm (SEQ *S);

/* make subst ID table used to get substring ID of given position */
void SEQ_make_substID_array (SEQ *S);

/* compute average length and boundary positions of each string */
void SEQ_compute_length (SEQ *S);

/* compute the possibility of matching randomly chosen letters */
double SEQ_match_letter_prob (SEQ *S);

/* compute the ID of the substring including the letter on the position p */
SEQ_ID SEQ_get_substID (SEQ *S, SEQ_LONG p);
INT SEQ_get_segID (SEQ *S, SEQ_LONG p);

/* put a character to the file */
void SEQ_putc (FILE *fp, SEQ *S, INT c);
void SEQ_FILE2_putc (FILE2 *fp, SEQ *S, INT c);

/* set fixed alphabet perm/unperm for genome mode */
void SEQ_genome_perm (SEQ *S);

/* return 1 if the letter is a genome string,
   or, letter is one of ATGCatgcXxNn */
/* return 1 if the mode is not genome sequence */
INT SEQ_check_genome_letter (SEQ_CHAR *c);

/* return 1 if two letters differ under the consideration of wildcard */
INT SEQ_wildcard_mismatch (SEQ *S, SEQ_CHAR c1, SEQ_CHAR c2);

/* if the position - len is outside the head/tail of the string including
 the position, return the head/tail of the position */
SEQ_LONG SEQ_adopt_head (SEQ *S, SEQ_LONG x, INT len);
SEQ_LONG SEQ_adopt_tail (SEQ *S, SEQ_LONG x, INT len);

// find the length to the end of strings
SEQ_LONG SEQ_len_to_head_ (SEQ *S, SEQ_LONG x, SEQ_ID *sx, SEQ_LONG y, SEQ_ID *sy);
SEQ_LONG SEQ_len_to_tail_ (SEQ *S, SEQ_LONG x, SEQ_ID *sx, SEQ_LONG y, SEQ_ID *sy);
SEQ_LONG SEQ_len_to_head (SEQ *S, SEQ_LONG x, SEQ_LONG y);
SEQ_LONG SEQ_len_to_tail (SEQ *S, SEQ_LONG x, SEQ_LONG y);

/* replace the genome letters by its complement */
void SEQ_opposite_genome (SEQ *S, SEQ_LONG s, SEQ_LONG t);
void SEQ_opposite_genome_ (SEQ_CHAR *s, SEQ_LONG x, SEQ_LONG y);
void SEQ_reverse_string (SEQ_CHAR *s, SEQ_LONG x, SEQ_LONG y);

/* reverse a string from s to t (not t-1) */
void SEQ_reverse_strings (SEQ *S, SEQ_LONG s, SEQ_LONG t);


/* output all strings in S->s */
void SEQ_fprint_all_string (FILE *fp, SEQ *S);

/* output a substring  */
void SEQ_fprint_substring (void *fp, SEQ *S, SEQ_LONG k, INT len, INT flag);

/* output one interval, by a substring, with extensions */
void SEQ_fprint_interval (void *fp, SEQ *S, SEQ_LONG x, SEQ_LONG y, INT flag);

/* read pairs of positions x and y, and print interval string from x to y */
void SEQ_fprint_all_interval (SEQ *S, char *input_fname, char *output_fname);

/* write head positions to the position file */
void SEQ_fprint_headtail (SEQ *S, char *fname);

/* read head/tail positions from the file */
void SEQ_load_headtail (SEQ *S, char *fname);

/*************************************************************/
/* scan/read file and store the data to memory */
/* if S->flag&SEQ_IGNORE_NEWLINE, ignore '\n' (o.w., read it as a usual character) */
/*************************************************************/
void SEQ_file_read (SEQ *S, char *fname, SEQ_LONG *cnt);
void SEQ_scan_file (SEQ *S);
void SEQ_read_file (SEQ *S);
void SEQ_load (SEQ *S);
void SEQ_save_ (SEQ *S, FILE2 *fp, SEQ_ID start, SEQ_ID end);
void SEQ_save (SEQ *S, char *fname, SEQ_ID start, SEQ_ID end);

/* compute mismatches of substrings of length k, starting at i and j */
SEQ_LONG SEQ_Hamming_distance (SEQ *S, SEQ_CHAR *s1, SEQ_CHAR *s2, SEQ_LONG k, SEQ_LONG ub);

/* initialization for edit distance computation */
void SEQ_edit_distance_init (SEQ *S, INT width, INT len);

/* compute edit distance between y and string from x to x+k, k<end, for each k
 the results are stored in the array sc
 if end < 0, from x to x-end.
  attension that "subst" is considered only for string x */
/* width gives the maximum distance. If the distance must be more than width,
 then the computation is stopped, and the current # iteration is returned */
INT SEQ_Ham_ext (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT dir, INT *k);
INT SEQ_CHam_ext (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, SEQ_CHAR *zx, SEQ_CHAR *zy, INT end, INT err_max, INT dir, INT *k);
INT SEQ_edit_ext (SEQ *S, INT *table, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT dir, INT *k);

/* return -1 if the position pair is not canonical */
/* a string is canonical if it is the first pair belonging to the position list */
/* if it is canonical, return the length to the end of continuous small Hamming distance
 substrings */
/* if comparison reaches to the string end, over will be set to non-zero
   if the end is exactly equal to the string end, *over will be 0 */
INT SEQ_Ham_canonicity (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT *k);
INT SEQ_CHam_canonicity (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, INT *k);

/* extension for all similarity measures */
//INT SEQ_extension (SEQ *S, INT *table, SEQ_LONG x, SEQ_LONG y, SEQ_LONG end_l, SEQ_LONG end_r, INT end, INT err_max, double err_ratio, INT *k, INT flag, INT *reach_to_end);
INT SEQ_extension (SEQ *S, INT *table, SEQ_LONG x, SEQ_LONG y, INT end, INT err_max, double err_ratio, INT *k, INT flag);

/* evaluate edit distance from pair of strings in both directions, to check whether
 it is a part of long similar substrings */
INT SEQ_extend_pair (SEQ *S, SEQ_LONG x, SEQ_LONG y, INT end, INT err_max, double err_ratio, INT *k);

/* find the limit of the extension to both side */
/* return -1 if the string is not the canonical in the maximal similar string */
/* return 1,2,3 if the extension reaches to the end of string */
/* *l,*r will be the length to the limit for left/right direction */
/* *k will be the distance of maximalized string pair */
INT SEQ_maximalize_pair (SEQ *S, SEQ_LONG x, SEQ_LONG y, SEQ_LONG end_l, SEQ_LONG end_r, INT err_max, double err_ratio, INT *k, SEQ_LONG *l, SEQ_LONG *r, INT flag);

/* compute the cost function table for the bit-difference(Hamming distance) */
void SEQ_mk_bit_difference_table (SEQ *S);
void SEQ_mk_distance_difference_table (SEQ *S);

#endif
