/* (multi) sequence library */
/* 2004 Takeaki Uno */

#ifndef _seq_c_
#define _seq_c_

#include"seq.h"
#include"stdlib2.c"
#include"queue.c"

QSORT_TYPE(SEQ_LONG, SEQ_LONG)
QSORT_TYPE(SEQ_ID, SEQ_ID)
QSORT_TYPE(SEQ_CHAR, SEQ_CHAR)
QSORT_TYPE(SEQ_COST, SEQ_COST)

/* initialization of SEQ. This should be called before loading the string data */
/* execution is SEQ_init => SEQ_scan_file => determing dep_max & pos_dep & hash_end
   => SEQ_init2 => SEQ_read_file */
void SEQ_init (SEQ *S){
    S->type = TYPE_SEQ;
    S->fname = S->fname2 = S->wfname = S->wfname2 = NULL;
    S->flag = 0;
    S->chr_num = S->chr_num2 = S->chr_num3 = 0;
    S->match_prob = 0;
    S->separator = ' ';

    S->normalized_by = S->separated_by = S->output_by = S->duplicated_by = S->reversed_by = S->matrix_by = S->reduced_by = 0;
    S->pairwise_max = 0;
    S->normalized_pos = S->separated_pos = S->output_pos = S->duplicated_pos = S->reversed_pos = S->matrix_pos = S->reduced_pos = 0;
    S->output_pos_fprint_pair = 0;
    S->base1 = S->base2 = 0;

    // filename

    // string, and multi string
    S->s = S->s_org = NULL;
    S->str_num = S->str_second = S->str_dup = 0;
    S->str_len = S->str_len_org = S->max_str = 0;
    S->min_str = 0;
    S->head = S->tail = S->clms = NULL;
    S->min_clms = INTHUGE;
    S->max_clms = 0;
    S->subst = NULL;
    S->wildcard = 0;

    // character and permutation
    S->chr_cnt = NULL;
    S->perm = S->unperm = NULL;
    S->subst_skip = SEQ_DEFAULT_SUBST_SKIP;
    S->end_extension = SEQ_DEFAULT_END_EXTENSION;

    // for edit distance
    S->edit_table = NULL;
    S->edit_table_end = 0;
    S->err_max = -1;
    S->extend = 0;
    S->err_ratio = 0.0;

    // for pattern string
    S->pat = NULL;
    S->plen = S->xlen = S->ylen = 0;
    S->p_ext = 0;
    S->perr = 0;
    S->plen_lb=0; S->plen_ub = INTHUGE;
    S->segment_num = S->segment_end = 0;
    S->plen_org = S->perr_org = 0;
    S->perr_min = -INTHUGE; S->perr_max = INTHUGE;
    S->perr_inc = 1;

    // for interleave
    S->interleave_len = S->interleave_len_org = 1;
    S->pos_skip_a = S->pos_skip_b = 0;

    // letter_cost table
    S->cost = NULL;
    S->micro_chr_num = 0;
    S->micro_chr_dist = 0;

    // string weight
    S->str_w = NULL;
}

/* show property of the sequence structure */
void SEQ_prop_print (SEQ *S){
    if ( !(S->flag & SHOW_MESSAGE) ) return;
    print_err ("input_file %s", S->fname);
    print_fname (" %s", S->fname2);
    print_err ("\n");
    print_err ("length= %d ,#strings= %d ,min./max.len.string= %d,%d ,#char=%d ,clms=%d ,prob._to_match %f\n",
               S->str_len, S->str_num-1, S->min_str, S->max_str, S->chr_num, (INT)(S->clms?S->clms[1]:0), SEQ_match_letter_prob (S));
//  print_fname (", string_duplicated_from %d to %d", S->duplicated_by, S->str_dup);
    print_fname ("string_separated_at %d", S->separated_by);
    print_fname (", reversed_from %d", S->reversed_by);
    print_fname (", output_upto %d", S->output_by);
    print_fname (", matrix_mode_upto %d", S->matrix_by);
    print_fname (", reduce_positions_upto %d", S->reduced_by);
    print_err ("\n");
}

void SEQ_set_boundary (SEQ *S, INT *b){
    if ( *b == -2 ) *b = S->str_dup;
    else if ( *b == -1 ) *b = S->fname2? S->str_second: S->str_num/2+1;
    else if ( *b == -3 ) *b = S->str_num+1;
    else ENMIN (*b, S->str_num);
    if ( *b == 1 ) *b = 0;
}

/* before this, dep_max has to be determined independently (and, pos_dep) */
/* before this, partition has to be determined */
void SEQ_set_param (SEQ *S){
    SEQ_ID i;

    // range check
    SEQ_set_boundary (S, &S->normalized_by);
    SEQ_set_boundary (S, &S->separated_by);
    SEQ_set_boundary (S, &S->output_by);
    SEQ_set_boundary (S, &S->reversed_by);
    SEQ_set_boundary (S, &S->matrix_by);
    SEQ_set_boundary (S, &S->reduced_by);

    if ( S->flag&SEQ_SEGMENT ){
        S->segment_end=0;
        FLOOP (i, 1, (SEQ_ID)MIN(S->matrix_by, S->output_by) )
            S->segment_end += (S->tail[i]-S->head[i]+S->clms[i]-1) / S->clms[i];
        if ( S->matrix_pos < S->output_pos ) S->output_pos_fprint_pair = S->matrix_pos;
    }

    malloc2 (S->s_org, S->str_len+ S->end_extension*2, goto ERR);
    S->s = S->s_org + S->end_extension;
    malloc2 (S->subst, (S->str_len+S->end_extension)/S->subst_skip+2, goto ERR);

    S->str_num_ = (S->flag&SEQ_SEGMENT)?S->segment_num: (S->output_by>0? S->output_by: S->str_num);

    if ( S->flag & SEQ_EDIT_DISTANCE ){
        S->edit_table_end = (INT)(S->err_ratio*S->extend)+10000;
        malloc2 (S->edit_table, S->edit_table_end, goto ERR);
    }

    // for genome letters
    if ( S->flag & SEQ_GENOME ){
        S->gperm[S->perm['A']] = S->perm['T'];
        S->gperm[S->perm['T']] = S->perm['A'];
        S->gperm[S->perm['G']] = S->perm['C'];
        S->gperm[S->perm['C']] = S->perm['G'];
        if ( S->chr_num>4 ){
            S->gperm[S->perm['N']] = S->perm['N'];
            S->wildcard = S->perm['N'];
        }
    }

    S->chr_num2 = S->chr_num * S->chr_num;
    S->chr_num3 = S->chr_num2 * S->chr_num;

//  malloc2 ( S->IDcnt, SEQ_LONG, S->str_num+1, "SEQ_init2: IDcnt");
//  malloc2 ( S->IDcnt2, SEQ_LONG, S->str_num+1, "SEQ_init2: IDcnt");
//  QUEUE_alloc ( &S->IDjump, S->str_num+1);
//  for (i=0 ; i<=S->str_num ; i++ ) S->IDcnt[i] = 0;
    if ( S->micro_chr_num > 0 ){
        if ( S->micro_chr_dist > 0 ) SEQ_mk_distance_difference_table (S);
        else SEQ_mk_bit_difference_table (S);
    }

    return;
    ERR:;
    SEQ_end (S);
    EXIT;
}

/* terminate routine */
void SEQ_end (SEQ *S){
    mfree (S->s_org, S->subst, S->head, S->tail, S->clms);
    mfree (S->chr_cnt, S->perm, S->unperm);
    mfree (S->pat, S->edit_table, S->str_w);
    free2d (S->cost);
    SEQ_init (S);
}

void SEQ_set_boundary2 (SEQ *S, INT b, SEQ_LONG *p){
    if ( b > 0 ) *p = b>=S->str_num? S->str_len: S->head[b];
}

// compute average length and boundary positions
void SEQ_compute_length (SEQ *S){
    SEQ_ID i, ii;
    SEQ_LONG x=0;

    // compute average length
    ii = (S->separated_by>0)? S->separated_by: S->str_num;
    FLOOP (i, 1, ii+1) x += (S->tail[i]-S->head[i]);
    S->ave_str = x/(i-1);
    if ( S->separated_by>0 ){
        FLOOP (i, 0, S->str_num) x += (S->tail[i]-S->head[i]);
        S->ave2_str = x/(S->str_num-ii);
    }

    // set boundary positions
    SEQ_set_boundary2 (S, S->normalized_by, &S->normalized_pos);
    SEQ_set_boundary2 (S, S->separated_by, &S->separated_pos);
    SEQ_set_boundary2 (S, S->output_by, &S->output_pos);
    SEQ_set_boundary2 (S, S->reversed_by, &S->reversed_pos);
    SEQ_set_boundary2 (S, S->matrix_by, &S->matrix_pos);
    SEQ_set_boundary2 (S, S->reduced_by, &S->reduced_pos);
    if ( S->output_by == 0 ) S->output_pos = S->str_len;
    S->output_pos_fprint_pair = S->output_pos;
}

/* make subst ID table used to get substring ID of given position */
void SEQ_make_substID_array (SEQ *S){
    SEQ_ID i;
    SEQ_LONG j;
    FLOOP (i, 1, S->str_num){
        for (j=S->head[i] ; j<S->tail[i] ; j+=S->subst_skip) S->subst[j/S->subst_skip] = i;
    }
    for (j=S->head[S->str_num] ; j<S->head[S->str_num]+S->end_extension ; j+=S->subst_skip)
        S->subst[j/S->subst_skip] = S->str_num;
}

/* compute the ID of the substring including the letter on the position p */
SEQ_ID SEQ_get_substID (SEQ *S, SEQ_LONG p){
    return (S->subst[p/SEQ_DEFAULT_SUBST_SKIP]);
}
INT SEQ_get_segID (SEQ *S, SEQ_LONG p){
//  return (S->subst[p/S->subst_skip]);
    INT s = SEQ_get_substID (S, p), ss;
    if ( S->flag&SEQ_SEGMENT ){
        if ( p >= S->matrix_pos ) return (s- S->matrix_by + S->segment_num);
        ss = (p-S->head[s]) / S->clms[s];
        if ( s>1 ) ss += (S->tail[1]-S->head[1]+S->clms[1]-1) / S->clms[1];
        return (ss);
    } else return (s);
}

SEQ_LONG SEQ_get_pos (SEQ_LONG p){
#ifdef SEQ_PPERM_MASK
    return (p&(SEQ_PPERM_MASK-1) );
#else
    return (p);
#endif
}

void SEQ_putc (FILE *fp, SEQ *S, INT c){
#ifdef SEQ_CHAR_LONG
    INT l1, l2, l3;
  if ( c == 200000000 ) fputc ('\n', fp);
  else if ( S->flag&SEQ_PUTC_NUM ){
    fprintf (fp, "%d%c", c, S->separator);
  } else if ( S->flag&SEQ_EUC ){
    l1 = c%200; c /= 200;
    l2 = c%200; c /= 200;
    l3 = c%200; c /= 200;
    fputc (c+32, fp);
    fputc (l3+32, fp);
    fputc (l2+32, fp);
    fputc (l1+32, fp);
  } else {
    l1 = c%256; c /= 256;
    l2 = c%256; c /= 256;
    l3 = c%256; c /= 256;
    fputc (c, fp);
    fputc (l3, fp);
    fputc (l2, fp);
    fputc (l1, fp);
  }
#elif defined SEQ_CHAR_SHORT
    if ( S->flag&SEQ_EUC && c<128*256 && c>=256 ){
    fputc (0x8F, fp);
    fputc (c/256+128, fp);
    fputc (c%256, fp);
  } else if ( S->flag&SEQ_EUC && c < 256 ){
    fputc (c, fp);
  } else {
    fputc (c/256, fp);
    fputc (c%256, fp);
  }
#else
    fputc (c, fp);
#endif
}

void SEQ_FILE2_putc (FILE2 *fp, SEQ *S, INT c){
#ifdef SEQ_CHAR_LONG
    INT l1, l2, l3;
  if ( c == 200000000 ) FILE2_putc (fp, '\n');
  else if ( S->flag&SEQ_PUTC_NUM ){
    FILE2_print_INT (fp, c, 0);
    FILE2_putc (fp, ' ');
  } else if ( S->flag&SEQ_EUC ){
    l1 = c%200; c /= 200;
    l2 = c%200; c /= 200;
    l3 = c%200; c /= 200;
    FILE2_putc (fp, c+32);
    FILE2_putc (fp, l3+32);
    FILE2_putc (fp, l2+32);
    FILE2_putc (fp, l1+32);
  } else {
    l1 = c%256; c /= 256;
    l2 = c%256; c /= 256;
    l3 = c%256; c /= 256;
    FILE2_putc (fp, c);
    FILE2_putc (fp, l3);
    FILE2_putc (fp, l2);
    FILE2_putc (fp, l1);
  }
#elif defined SEQ_CHAR_SHORT
    if ( S->flag&SEQ_EUC && c<128*256 && c>=256 ){
    FILE2_putc (fp, 0x8F);
    FILE2_putc (fp, c/256+128);
    FILE2_putc (fp, c%256);
  } else if ( S->flag&SEQ_EUC && c < 256 ){
    FILE2_putc (fp, c);
  } else {
    FILE2_putc (fp, c/256);
    FILE2_putc (fp, c%256);
  }
#else
    FILE2_putc (fp, c);
#endif
}

/* if the position - len is outside the head/tail of the string including
 the position, return the head/tail of the position */
/* for original string position */
SEQ_LONG SEQ_adopt_head (SEQ *S, SEQ_LONG x, INT len){
    SEQ_ID ID = SEQ_get_substID (S, x);
    if ( x-len < S->head[ID] ) return (S->head[ID]);
    else return (x-len);
}
SEQ_LONG SEQ_adopt_tail (SEQ *S, SEQ_LONG x, INT len){
    SEQ_ID ID = SEQ_get_substID (S, x);
    if ( x+len >= S->tail[ID] ) return (S->tail[ID]);
    else return (x+len);
}

/* compute the length to the end of strings; for original string positions */
SEQ_LONG SEQ_len_to_head_ (SEQ *S, SEQ_LONG x, SEQ_ID *sx, SEQ_LONG y, SEQ_ID *sy){
    SEQ_LONG hx, hy;
    *sx = SEQ_get_substID (S, x); hx = x - S->head[*sx];
    if ( (S->flag&SEQ_SEGMENT) && x<S->matrix_pos ) hx = hx % S->clms[*sx];
    *sy = SEQ_get_substID (S, y); hy = y - S->head[*sy];
    if ( (S->flag&SEQ_SEGMENT) && y<S->matrix_pos ) hy = hy % S->clms[*sy];
    return (MIN(hx, hy));
}
SEQ_LONG SEQ_len_to_tail_ (SEQ *S, SEQ_LONG x, SEQ_ID *sx, SEQ_LONG y, SEQ_ID *sy){
    SEQ_LONG hx, hy;
    *sx = SEQ_get_substID (S, x);
    if ( (S->flag&SEQ_SEGMENT) && x<S->matrix_pos ) hx = S->clms[*sx] - ((x - S->head[*sx])%S->clms[*sx]);
    else hx = S->tail[*sx]-x;
    *sy = SEQ_get_substID (S, y);
    if ( (S->flag&SEQ_SEGMENT) && y<S->matrix_pos ) hy = S->clms[*sy] - ((y - S->head[*sy])%S->clms[*sy]);
    else hy = S->tail[*sy]-y;
    return (MIN(hx, hy));
}
SEQ_LONG SEQ_len_to_head (SEQ *S, SEQ_LONG x, SEQ_LONG y){
    SEQ_ID sx, sy;
    return (SEQ_len_to_head_ (S, x, &sx, y, &sy) );
}
SEQ_LONG SEQ_len_to_tail (SEQ *S, SEQ_LONG x, SEQ_LONG y){
    SEQ_ID sx, sy;
    return (SEQ_len_to_tail_ (S, x, &sx, y, &sy) );
}

/*
SEQ_LONG SEQ_head_matrix (SEQ *S, SEQ_LONG x, SEQ_ID *sx ){
  *sx = SEQ_get_substID (S, x);
  if ( S->ylen > 0 && x<S->matrix_pos ) return (hx = hx % S->clms[sx];
  else return (S->head[sx]);
}
SEQ_LONG SEQ_tail_matrix (SEQ *S, SEQ_LONG x, SEQ_ID *sx ){
  *sx = SEQ_get_substID (S, x);
  if ( S->ylen > 0 && x<S->matrix_pos ) hx = S->clms[sx] - ((xx - S->head[sx])%S->clms[sx]);
  else hx = S->tail[sx]-xx;
}
*/

/* set fixed alphabet perm for genome mode */
void SEQ_genome_perm (SEQ *S){
    S->perm['A'] = 0; S->perm['T'] = 1; S->perm['G'] = 2;
    S->perm['C'] = 3; S->perm['N'] = 4;
    S->unperm[0] = 'A'; S->unperm[1] = 'T'; S->unperm[2] = 'G';
    S->unperm[3] = 'C'; S->unperm[4] = 'N';
    if ( S->chr_num != 5 ) S->chr_num = 4;
}

/**************************************************************/
/* return 1 if the letter is a genome string,
   or, letter is one of ATGCatgcXxNn */
/* return 1 if the mode is not genome sequence */
/**************************************************************/
INT SEQ_check_genome_letter (SEQ_CHAR *c){
    INT flag=1;
    if ( *c == 'A' || *c == 'a' ) *c = 'A';
    else if ( *c == 'T' || *c == 't' ) *c = 'T';
    else if ( *c == 'G' || *c == 'g' ) *c = 'G';
    else if ( *c == 'C' || *c == 'c' ) *c = 'C';
    else if ( *c == 'N' || *c == 'n' || *c == 'X' || *c == 'x' ) *c = 'N';
    else flag = 0;
    return (flag);
}

/* return 1 if two letters differ under the consideration of wildcard */
INT SEQ_wildcard_mismatch (SEQ *S, SEQ_CHAR c1, SEQ_CHAR c2){
    if ( S->cost ) return (S->cost[c1][c2]);
    return ((c1!=c2 && (S->wildcard==0 || (c1!=S->wildcard && c2!=S->wildcard)))?1:0);
}

/* replace the genome letters by its complement */
void SEQ_opposite_genome (SEQ *S, SEQ_LONG s, SEQ_LONG t){
    char *pat="ATGCatgc", *pat2="TACGtacg", *c;
    for (; s<t ; s++){
        c = strchr (pat, S->unperm[S->s[s]]);
        if ( c ) S->s[s] = S->perm[(SEQ_CHAR)(pat2[c-pat])];
    }
}
void SEQ_opposite_genome_ (SEQ_CHAR *s, SEQ_LONG x, SEQ_LONG y){
    char *pat="ATGCatgc", *pat2="TACGtacg", *c;
    for (; x<y ; x++){
        c = strchr (pat, s[x]);
        if ( c ) s[x] = (SEQ_CHAR)(pat2[c-pat]);
    }
}
void SEQ_reverse_string (SEQ_CHAR *s, SEQ_LONG x, SEQ_LONG y){
    SEQ_CHAR c;
    for (y-- ; x<y ; x++,y--){
        c = s[x] ; s[x] = s[y] ; s[y] = c;
    }
}

/* reverse strings from s-th to t-th (not t-1) */
void SEQ_reverse_strings (SEQ *S, SEQ_ID s, SEQ_ID t){
    SEQ_CHAR c;
    SEQ_ID i, ii;
    SEQ_LONG ss=S->head[s], tt=S->head[t+1];

    // reverse head and tail of substrings
    FLOOP (i, s, t+1){
        S->head[i] = ss + tt - S->head[i];
        S->tail[i] = ss + tt - S->tail[i];
        SWAP_SEQ_LONG (S->head[i], S->tail[i]);
    }
    for (i=s,ii=t ; i<ii ; i++,ii--){
        SWAP_SEQ_LONG (S->head[i], S->head[ii]);
        SWAP_SEQ_LONG (S->tail[i], S->tail[ii]);
        if ( S->clms ) SWAP_SEQ_LONG (S->clms[i], S->clms[ii]);
    }

    // reverse string
    for (i=ss,ii=tt-1 ; i<ii ; i++,ii--){
        c = S->s[i] ; S->s[i] = S->s[ii] ; S->s[ii] = c;
    }
    // shift string so that each head is a multiple
    FLOOP (i, s, t+1){
        ss = S->head[i]%S->subst_skip;
        FLOOP (ii, S->head[i], S->tail[i]) S->s[ii-ss] = S->s[ii];
        S->head[i] -= ss; S->tail[i] -= ss;
    }
}

/* output all strings in S->s */
void SEQ_fprint_all_string (FILE *fp, SEQ *S){
    SEQ_ID i;
    SEQ_LONG j;
    fprintf (fp, "all strings\n");
    FLOOP (i, 1, (S->output_by>0? S->output_by: S->str_num)){
        fprintf (fp, "string "SEQ_IDF": head "SEQ_IDF", tail "SEQ_IDF"\n", i, S->head[i], S->tail[i]);
        FLOOP (j, S->head[i], S->tail[i]) SEQ_putc (fp, S, S->unperm[S->s[j]]);
        fprintf (fp, "\n\n");
    }
}

void SEQ_fprint_string (void *fp, SEQ *S, SEQ_CHAR *s, INT len, INT flag){
    SEQ_CHAR *s_end=s+len, *s_end2;
    if ( flag ){
//    i=k; while ( i < k+len ){
        while (s < s_end ){
            s_end2 = MIN (s_end, s+((((FILE2 *)fp)->buf_org+FILE2_BUFSIZ)-((FILE2 *)fp)->buf));
            for ( ; s<s_end2 ; s++)
                SEQ_FILE2_putc ((FILE2 *)fp, S, S->unperm[*s]);
            FILE2_flush ((FILE2*)fp);
        }
    } else {
        for ( ; s<s_end ; s++) SEQ_putc ((FILE *)fp, S, S->unperm[*s]);
    }
}

/* output a substring, if flag, multi-output mode, and use FILE2 */
/*
void SEQ_fprint_substring_ (void *fp, SEQ *S, SEQ_LONG k, INT len, INT flag){
  SEQ_LONG i, ii;
  if ( flag ){
    FILE2_flush ((FILE2*)fp);
    i=k; while ( i < k+len ){
      FLOOP (ii, i, MIN(k+len,i+FILE2_BUFSIZ/2))
          SEQ_FILE2_putc ((FILE2 *)fp, S, S->unperm[S->s[ii]]);
      FILE2_flush ((FILE2*)fp);
      i = ii;
    }
  } else {
    FLOOP (i, k, k+len) SEQ_putc ((FILE *)fp, S, S->unperm[S->s[i]]);
  }
}
*/

/* output one interval, by a substring, with extensions */
/* for original string positions */
void SEQ_fprint_interval (void *fp, SEQ *S, SEQ_LONG x, SEQ_LONG y, INT flag){
    if ( S->flag & SEQ_OUTPUT_STRING ){
        x = SEQ_adopt_head (S, x, S->p_ext);
        y = SEQ_adopt_tail (S, y, S->plen_org+S->p_ext);
        if ( flag ) FILE2_puts ((FILE2*)fp, "     "); else fprintf ((FILE *)fp, "     ");
        SEQ_fprint_string ((FILE *)fp, S, &S->s[x], y-x, flag);
    }
}

/* output pattern substrings beginning at each position in pos_list */
void SEQ_fprint_pos_list (FILE *fp, SEQ *S, SEQ_LONG *pos_list, SEQ_LONG t){
    SEQ_LONG i, x;
    FLOOP (i, 0, t){
        x = SEQ_get_pos (pos_list[i]);
        fprintf (fp, SEQ_LONGF, x+S->base1);
        SEQ_fprint_interval (fp, S, x, x, 0);
        fprintf (fp, "\n");
    }
}

/* read pairs of positions x and y, and print interval string from x to y */
void SEQ_fprint_all_interval (SEQ *S, char *input_fname, char *output_fname){
    SEQ_LONG x, y;
    FILE *out_fp=NULL;
    FILE2 in_fp = INIT_FILE2;

    FILE2_open (in_fp, input_fname, "r", goto END);
    if ( !input_fname ) in_fp.fp = stdin;
    if ( output_fname ) fopen2 (out_fp, input_fname, "w", goto END);
    else out_fp = stdout;

    while (1){
        x = (SEQ_LONG)FILE2_read_int (&in_fp);
        y = (SEQ_LONG)FILE2_read_int (&in_fp);
        if ( FILE_err == 2 ) break;
        FILE2_read_until_newline (&in_fp);
        fprintf (out_fp, SEQ_LONGF "%c" SEQ_LONGF, x+S->base1, S->separator, y+S->base1);
        SEQ_fprint_interval (out_fp, S, x, y, 0);
        fprintf (out_fp, "\n");
    }
    END:;
    FILE2_close (&in_fp);
    fclose2 (out_fp);
    if ( ERROR_MES ) EXIT;
}

/* translate positions in the file to intervals, and write them to the file */
/* consecutive integers are considered to be an interval */
/* each line has to include at least one integer, and the output is the
 pair of integers representing the beginning/ending position of an interval */
void SEQ_fprint_pos_substrings (SEQ *S, char *input_fname, char *output_fname){
    SEQ_LONG x, start=S->str_len, prv=S->str_len;
    FILE *out_fp=NULL;
    INT flag=0;
    FILE2 in_fp = INIT_FILE2;

    FILE2_open (in_fp, input_fname, "r", goto END);
    if ( !input_fname ) in_fp.fp = stdin;
    if ( output_fname ) fopen2 (out_fp, output_fname, "w", goto END);
    else out_fp = stdout;

    while (1){
        flag=0;
        x = (SEQ_LONG)FILE2_read_int (&in_fp);
        if ( FILE_err&4 ) flag=1;  // read no integer
        FILE2_read_until_newline (&in_fp);
        if ( FILE_err&2 ) break;
        if ( flag ) continue;
        if ( S->flag & SEQ_OUTPUT_INTERVAL ){
            if ( x != prv+1 ){
                if ( start < S->str_len ){
                    fprintf (out_fp, SEQ_LONGF "%c" SEQ_LONGF, start+S->base1, S->separator, prv+S->base1);
                    SEQ_fprint_interval (out_fp, S, start, prv, 0);
                    fprintf (out_fp, "\n");
                }
                start = x;
            }
            prv = x;
        } else {
            fprintf (out_fp, SEQ_LONGF, x+S->base1);
            SEQ_fprint_string (out_fp, S, &S->s[x], S->plen, 0);
            fprintf (out_fp, "\n");
        }
    }
    if ( S->flag & SEQ_OUTPUT_INTERVAL ){ // output last interval, if it is prcessing and not output yet
        if ( x != prv+1 ){
            if ( start >= 0 ) SEQ_fprint_interval (out_fp, S, start, prv, 0);
        }
    }
    END:;
    FILE2_close (&in_fp);
    fclose2 (out_fp);
    if ( ERROR_MES ) EXIT;
}



/* write head positions to the position file */
void SEQ_fprint_headtail (SEQ *S, char *fname){
    FILE *fp;
    SEQ_ID i;
    fopen2 (fp, fname, "w", EXIT);
    FLOOP (i, 1, S->str_num)
        fprintf (fp, SEQ_LONGF "%c" SEQ_LONGF "\n", S->head[i], S->separator, S->tail[i]);
    fclose2 (fp);
}

/* read head/tail positions from the file */
void SEQ_load_headtail (SEQ *S, char *fname){
    FILE2 fp = INIT_FILE2;
    SEQ_ID i=0;
    SEQ_LONG x, y;
    FILE2_open (fp, fname, "r", EXIT);
    while (1){
        x = (SEQ_LONG)FILE2_read_int (&fp);
        y = (SEQ_LONG)FILE2_read_int (&fp);
        if ( FILE_err&2 ) break;
        FILE2_read_until_newline ( &fp);
        i++;
    }
    S->str_num = i;
    if ( !(S->head) || S->str_num<=i ){
        free2 (S->head);
        free2 (S->tail);
        malloc2 (S->head, i+2, goto ERR);
        malloc2 (S->tail, i+2, goto ERR);
    }
    S->head[0] = S->tail[0] = 0; i=0;
    FILE2_reset (&fp);
    while (1){
        x = (SEQ_LONG)FILE2_read_int (&fp);
        y = (SEQ_LONG)FILE2_read_int (&fp);
        if ( FILE_err&2 ) break;
        FILE2_read_until_newline ( &fp);
        S->head[i] = x;
        S->tail[i] = y;
        i++;
    }
    S->head[i] = S->tail[i] = 0;
    ERR:;
    FILE2_close (&fp);
    free2 (S->head);
    free2 (S->tail);
    EXIT;
}

/* scan/read strings from a file, cnt==0 => read */
void SEQ_file_read (SEQ *S, char *fname, SEQ_LONG *cnt){
    SEQ_CHAR c;
    INT first=(S->str_num<=1), cc=0, flag=0, n_flag=0, g_flag = (S->flag & (SEQ_IGNORE_NEWLINE+SEQ_GENOME)), newline_flag =0;
    FILE2 fp = INIT_FILE2;
    SEQ_LONG len=0, clm=0, k=0, b=0;

    FILE_err = 0;
    if ( cnt ){
        if ( first ) S->segment_num = 0;
        if ( S->flag&SEQ_GENOME ) SEQ_genome_perm (S);  // fixed alphabet, for genome mode
    } else S->head[0] = S->head[1] = S->tail[0] = 0;
    FILE2_open (fp, fname, "r", {free2(cnt);EXIT;});
    while (1){
        if ( S->flag & SEQ_NUM ){
            if ( FILE_err & 2 ) break;  // read interger, and EOF
            else if ( FILE_err & 1 ) FILE_err = 8;  // set 4th bit for 'newline flag'
            else {
                cc = FILE2_read_int (&fp);
                if ( FILE_err == 6 ) break;
                if ( FILE_err == 5 ) FILE_err = 8; // set 4th bit for 'newline flag'
                else if ( cc >= SEQ_CHAR_END ) cc = SEQ_CHAR_END;
            }
        } else if ( (cc=FILE2_getc(&fp)) < 0 ) break;

        c = cc;
        if ( S->flag & SEQ_NUM ){ if ( FILE_err == 8 ) flag++; else flag = 0; }
        else if ( c == '\n' ){ flag += 1; n_flag = 1; }
        else if ( c == '\r' ){ flag += 4; n_flag = 2; }
        else flag = 0;
        if ( (S->flag & SEQ_MULTI_STRING) && len>0 ){   // newlines after other letters
            if ( (flag&3)==3 || (flag&12)==12 ){ flag = 2; b++; }
            if ( flag==2 || flag==8 || flag==10 || newline_flag ){  // two-consecutive-newlines
                if ( cnt ){
                    ENMAX (S->max_str, k);
                    if ( S->min_str == 0 ) S->min_str = k; else ENMIN (S->min_str, k);
                    k = 0;
                } else S->tail[S->str_num] = S->str_len;    // set tail
                if ( S->ylen>0 && cnt){
                    if ( (first && S->matrix_by==-1) || (S->matrix_by>0 && S->str_num<(SEQ_ID)S->matrix_by) )
                        S->segment_num += (len+clm-1)/clm;   // update segment_num
                    else S->segment_num ++;
                }

                S->str_len = ((b+(S->subst_skip-1)) / S->subst_skip) * S->subst_skip;
                S->str_num++;
                if ( !cnt ) S->head[S->str_num] = S->str_len;   // set head
                else if ( S->duplicated_by == S->str_num ) S->duplicated_pos = S->str_len;
                len = clm = 0;
                flag = newline_flag = 0;
                continue;
            }
        } else if ( (flag&3)==3 || (flag&12)==12 ){ flag = 0; b = S->str_len; } // illigal combination of '\r' and '\n'
        if ( S->ylen>0 && clm==0 && (((c=='\n' || c=='\r') && !(S->flag & SEQ_NUM)) || ((S->flag&SEQ_NUM) && FILE_err==8)) ){  // memory #clms of matrix
            clm = len;
            if ( !cnt ){
                S->clms[S->str_num] = len;
                ENMIN (S->min_clms, len);
                ENMIN (S->max_clms, len);
            }
        }

        if ( (S->flag&SEQ_GENOME) && c=='>' ){  // ignore comment line
            FILE2_read_until_newline (&fp);
            if ( len>0 && (S->flag&SEQ_EUC) ) newline_flag = 1;
            continue;
        }
        if ( S->flag & SEQ_NUM ){
            if ( FILE_err == 8 ) continue; // newline is always ignored in num-sequence mode
            n_flag = 0;
        } else {
            if ( c=='\n' ){
                if ( g_flag || n_flag==2 || len==0 ){ n_flag = 0; continue; }  // ignore newline, (newlines at the head of strings are ignored)
            } else if ( c=='\r' ){
                if (g_flag || n_flag==1 || len==0 ){ n_flag = 0; continue; }  // ignore newline,  (newlines at the head of strings are ignored)
            } else n_flag = 0;
#ifdef SEQ_CHAR_LONG  // 4byte-word
            if ( c!='\n' && c!='\r' ){
        if ( S->flag&SEQ_EUC ) c = (c-32)*200*200*200 + (FILE2_getc(&fp)-32)*200*200 + (FILE2_getc(&fp)-32)*200 + (FILE2_getc(&fp)-32);
        else c = c*256*256*256 + FILE2_getc(&fp)*256*256 + FILE2_getc(&fp)*256 + FILE2_getc(&fp);
      } else c = 200000000;
      if ( c >= SEQ_CHAR_END ) c = 0;
#elif defined SEQ_CHAR_SHORT  // 2byte-word
            if ( (S->flag&SEQ_EUC) && c==0x8F ) c = (FILE2_getc(&fp)-128)*256 +FILE2_getc(&fp);
      else if ( c!='\n' && c!='\r' && (!(S->flag&SEQ_EUC) || c>=0x80) ) c = c*256 +FILE2_getc(&fp);
#else
            if ( (S->flag&SEQ_GENOME) && (!SEQ_check_genome_letter(&c)) ) continue;
#endif
        }

#ifdef SEQ_CHAR_LONG  // 4byte-word
        if ( cnt && c<200000000 ){
#else
        if ( cnt ){  // scan file
#endif
            if ( cnt[c] == 0 ){ // first appearance of a new char
                if ( S->flag&SEQ_GENOME ){
                    if ( c == 'N' ) S->chr_num = 5;
                } else {
                    S->unperm[S->chr_num] = c;
                    S->perm[c] = S->chr_num;
                    S->chr_num++;
                }
            }
            cnt[c]++;
            k++;
            S->str_len_org++;
        } else {  // read file
#ifdef SEQ_CHAR_LONG  // 4byte-word
            if ( c == 200000000 ) S->s[S->str_len] = S->chr_num; else
#endif
            S->s[S->str_len] = S->perm[c];
        }
        S->str_len++;
        len++;
        b = S->str_len;
    }
    if ( len>0 ){    //  if the last string has no endmark
        if ( cnt ){
            ENMAX (S->max_str, k);
            if ( S->min_str == 0 ) S->min_str = k; else ENMIN (S->min_str, k);
            k = 0;
        } else S->tail[S->str_num] = S->str_len;    // set tail
        if ( S->ylen>0 && clm==0 && !cnt ){
            S->clms[S->str_num] = len;
            ENMIN ( S->min_clms, len);
            ENMIN ( S->max_clms, len);
        }
        if ( S->ylen>0 && cnt && ( (first && S->matrix_by==-1) || (S->matrix_by>0 && S->str_num<(SEQ_ID)S->matrix_by) ) )
            S->segment_num += (len+clm-1)/clm;   // update segment_num
        if ( !cnt ) S->tail[S->str_num] = S->str_len; // write the tail of the last string
        S->str_num++;   // increment #string
        if ( S->duplicated_by == S->str_num ) S->duplicated_pos = S->str_len;
        S->str_len = ((S->str_len+(S->subst_skip-1)) / S->subst_skip) * S->subst_skip;
        if ( !cnt ) S->head[S->str_num] = S->str_len;
    }
#ifdef SEQ_CHAR_LONG  // 4byte-word
    if ( cnt ) S->unperm[S->chr_num] = 200000000;   // newline code
#endif

    FILE2_close (&fp);
}

/*************************************************************/
/* scan the file and get lengths of the string data, #apparance of each char, #substrings */
/* if S->flag&SEQ_IGNORE_NEWLINE, ignore '\n' (o.w., read it as a usual character) */
/*************************************************************/
void SEQ_scan_file (SEQ *S){
    SEQ_LONG *cnt;
    INT i;

    S->chr_num = 0;
    S->str_num = 1;
    S->str_len = S->str_len_org = 0;
    calloc2 (cnt, SEQ_CHAR_END, goto ERR);
    calloc2 (S->chr_cnt, SEQ_CHAR_END, goto ERR);
    calloc2 (S->perm, SEQ_CHAR_END+1, goto ERR);
    calloc2 (S->unperm, SEQ_CHAR_END+1, goto ERR);
    ARY_FILL (cnt, 0, SEQ_CHAR_END, 0);  // initialize char counter

    print_mes (S, "reading file %s\n", S->fname);
    SEQ_file_read (S, S->fname, cnt);
    if ( S->fname2 ){
        S->str_second = S->str_num;
        if ( S->duplicated_by == -1 ){
            S->duplicated_by = S->str_num;
            S->duplicated_pos = S->str_len;
        }
        print_mes (S, "reading file %s\n", S->fname2);
        SEQ_file_read (S, S->fname2, cnt);
    }
    if ( S->duplicated_by == -1 ) S->duplicated_by = S->str_num/2+1;

    if ( !(S->flag & SEQ_MULTI_STRING) ) S->str_num = 2;
    FLOOP (i, 0, S->chr_num) S->chr_cnt[i] = cnt[S->unperm[i]];  // copy char counter

    if ( S->duplicated_by > 0 && (SEQ_ID)S->duplicated_by < S->str_num ){
        S->str_dup = S->str_num;
//    if ( S->output_by == 0 || (SEQ_ID)S->output_by > S->str_num ) S->output_by = S->str_num;
        S->str_len += (S->str_len - S->duplicated_pos);
        S->str_num += (S->str_num - S->duplicated_by);
    }
    S->match_prob = SEQ_match_letter_prob (S);
    ERR:;
    free2 (cnt);
    if ( !ERROR_MES ) return;
    free2 (S->chr_cnt); free2 (S->perm); free2 (S->unperm);
}

/*************************************************************/
/* read file and store the data to memory */
/* if S->flag&SEQ_IGNORE_NEWLINE, ignore '\n' (o.w., read it as a usual character) */
/*************************************************************/
void SEQ_read_file (SEQ *S){
    SEQ_LONG i;

    malloc2 (S->head, S->str_num+2, goto ERR);
    malloc2 (S->tail, S->str_num+2, goto ERR);
    if ( S->ylen > 0 ) calloc2 ( S->clms, S->str_num+2, goto ERR);

    S->str_num = 1;
    S->str_len = 0;
    SEQ_file_read (S, S->fname, NULL);
    if ( S->fname2 ) SEQ_file_read (S, S->fname2, NULL);
    if ( ERROR_MES ) goto ERR;
    S->tail[S->str_num] = S->head[S->str_num] = S->str_len;
    // duplicate the latter strings
    if ( S->duplicated_by > 0 && (SEQ_ID)S->duplicated_by < S->str_num ){
        memcpy ( &S->s[S->str_len], &S->s[S->duplicated_pos], S->str_len-S->duplicated_pos);
        for (i=S->str_num*2-S->duplicated_by ; i>=S->str_num ; i-- ){
            S->head[i] = S->head[i -(S->str_num -S->duplicated_by)] +(S->str_len -S->duplicated_pos);
            S->tail[i] = S->tail[i -(S->str_num -S->duplicated_by)] +(S->str_len -S->duplicated_pos);
            if ( S->clms ) S->clms[i] = S->clms[i -(S->str_num -S->duplicated_by)];
        }
        S->str_len += (S->str_len - S->duplicated_pos);
        S->str_num += (S->str_num - S->duplicated_by);
    }
    // reverse the latter strings
    if ( S->reversed_by >0 ){
        SEQ_reverse_strings (S, S->reversed_by, S->str_num-1);
        if ( S->flag&SEQ_GENOME )
            SEQ_opposite_genome (S, S->head[S->reversed_by], S->tail[S->str_num-1]);
    }
    return;
    ERR:;
    mfree (S->head, S->tail, S->clms);
}

/* load a file (or 2 files) from file(s)*/
void SEQ_load (SEQ *S){
    FILE2 fp = INIT_FILE2;
    // scan input file and get file/string sizes, and #charactors
    SEQ_scan_file (S);
    SEQ_set_param (S);    // 2nd initialization and memory allocation

    // read file to memory
    SEQ_read_file (S);
    SEQ_compute_length (S);  // compute average length of strings
    if ( !(S->flag&SEQ_NO_SUBST_ID) ) SEQ_make_substID_array (S);   // make substID array
    if ( S->flag & SHOW_MESSAGE ) SEQ_prop_print (S);
    if ( (S->flag & SEQ_STR_W) || S->wfname ){
        calloc2 (S->str_w, S->str_num, EXIT);
        if ( S->wfname ){
            FILE2_open (fp, S->wfname, "r", EXIT);
            ARY_READ (&S->str_w[1], WEIGHT, S->str_second-1, fp);
            FILE2_close (&fp);
        }
        if ( S->wfname2 ){
            FILE2_open (fp, S->wfname2,"r", EXIT);
            ARY_READ (&S->str_w[S->str_second], WEIGHT, S->str_num-S->str_second, fp);
            FILE2_close (&fp);
        }
    }
}

/* write the strings from ID start to end-1 to the file "FILE2 fp" (for appending) */
void SEQ_save_ (SEQ *S, FILE2 *fp, SEQ_ID start, SEQ_ID end){
    SEQ_ID i;
    SEQ_LONG j;

    FLOOP (i, start, end){
        FLOOP (j, S->head[i], S->tail[i]){
            SEQ_FILE2_putc (fp, S, S->unperm[S->s[j]]);
            FILE2_flush (fp);
        }
        FILE2_putc (fp, '\n'); FILE2_putc (fp, '\n');
    }
}

/* write the strings from ID start to end-1 to the file "fname" */
void SEQ_save (SEQ *S, char *fname, SEQ_ID start, SEQ_ID end){
    FILE2 fp;
    FILE2_open (fp, fname, "w", EXIT);
    SEQ_save_ (S, &fp, start, end);
    FILE2_closew (&fp);
}

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/


/* compute mismatches of substrings of length k, starting at i and j */
SEQ_LONG SEQ_Hamming_distance (SEQ *S, SEQ_CHAR *s1, SEQ_CHAR *s2, SEQ_LONG k, SEQ_LONG ub){
    SEQ_LONG sc=0;
    SEQ_CHAR *s1_end;
    for (s1_end=s1+k ; s1<s1_end ; s1++,s2++){
        if ( SEQ_wildcard_mismatch (S, *s1, *s2) ){ sc++; if ( sc>ub ) break; }
    }
    return (sc);
}

/* extend a pair of similar string with Hamming distance, until distance exceeds err_max */
/* these extend routines accepts original string positions, not shifted */
/* end < 0, left direction, err_max==0, use err_ratio */
/* return the max length being under the err_max/err_ratio, and the distance
  if the search reaches to the end of string */
INT SEQ_Ham_ext (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT dir, INT *k){
    INT i;

    for (*k=0, i=0 ; i<end ; i++, xx+=dir, yy+=dir){
        if ( SEQ_wildcard_mismatch (S, *xx, *yy) ){
            (*k)++;
            if ( err_ratio == 0.0 ){
                if ( *k > err_max ) return (i);
            } else if ( err_ratio*i+1 <= *k ) return (i);
        }
    }
    return (i);
}
/* compute edit distance starting from x and y */
/* end < 0, left direction, err_max==0, use err_ratio */
/* return the max length being under the err_max/err_ratio, and the distance
  if the search reaches to the end of string */
INT SEQ_edit_ext (SEQ *S, INT *table, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT dir, INT *k){
    INT i, jj, j, pre_width=0, width, up, down, mid, cent=S->edit_table_end/2;
    table[cent] = 0;
    for (i=0 ; i<end ; i++,xx+=dir,yy+=dir){
        width = err_ratio==0? MIN(err_max,i+1): (INT)ceil((i+1)*err_ratio);
        if ( width>pre_width ) table[cent-width-1] = table[cent+width+1] = INTHUGE;
        for (jj=-width,down=INTHUGE ; jj<width ; jj++){
            j = jj+cent;
            up = table[j+1]+1;
            mid = MIN(MIN(down, up),table[j]+(SEQ_wildcard_mismatch (S, *xx, *(yy+jj))?1:0));
            down = table[j-1]+1;
            table[j] = mid;
        }
        *k = table[cent];
//    printf ("%d,%d: %d %d\n", i, *k, err_max, (INT)(err_ratio*i)+1);
        if ( err_ratio == 0.0 ){
            if ( *k > err_max ) return (i);
        } else if ( err_ratio*i+1 <= *k ) return (i);
        pre_width = width;
    }
    return (i);
}
INT SEQ_CHam_ext (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, SEQ_CHAR *zx, SEQ_CHAR *zy, INT end, INT err_max, INT dir, INT *k){
    INT i, kk = *k;

    for (i=0 ; i<end ; i++, xx+=dir, yy+=dir, zx+=dir, zy+=dir){
        if ( SEQ_wildcard_mismatch (S, *zx, *zy) ) kk--;
        if ( SEQ_wildcard_mismatch (S, *xx, *yy) ){
            kk++;
            if ( *k < kk ){
                *k = kk;
                if ( kk > err_max ) return (i);
            }
        }
    }
    return (i);
}

/* return -1 if the position pair is not canonical */
/* a string is canonical if it is the first pair belonging to the position list */
/* if it is canonical, return the length to the end of continuous small Hamming distance
 substrings */
/* if comparison reaches to the string end, over will be set to non-zero
   if the end is exactly equal to the string end, *over will be 0 */
INT SEQ_Ham_canonicity (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, double err_ratio, INT *k){
    INT i, kk=*k, ii=0;
    SEQ_CHAR *zx=xx+S->plen_org, *zy=yy+S->plen_org;

    for (*k=0,i=0 ; i<end ; xx--,yy--,zx--,zy--){
        if ( SEQ_wildcard_mismatch (S, *zx, *zy) ) kk--;
        if ( SEQ_wildcard_mismatch (S, *xx, *yy) ){
            (*k)++; kk++;
            if ( err_ratio == 0.0 ){
                if ( *k > err_max ) return (i);
            } else if ( err_ratio*i+1 <= *k ) return (i);
        }
        i++;
        if ( i-ii == S->interleave_len ){
            if ( kk <= S->perr_org ) return (-1);
            ii += S->interleave_len;
        }
//    if ( i-ii == S->interleave_tmp && kk <= S->perr_org ) return (-1);
    }
    return (i);
}
INT SEQ_CHam_canonicity (SEQ *S, SEQ_CHAR *xx, SEQ_CHAR *yy, INT end, INT err_max, INT *k){
    SEQ_CHAR *zx = xx+S->plen_org, *zy = yy+S->plen_org;
    INT i, ii=0, kk=*k;
    for (i=0 ; i<end ; zx--,zy--,xx--,yy-- ){
        if ( SEQ_wildcard_mismatch (S, *zx, *zy) ) kk--;
        if ( SEQ_wildcard_mismatch (S, *xx, *yy) ){
            kk++;
            if ( kk > *k ){
                *k = kk;
                if ( kk > err_max ) return (i);
            }
        }
        i++;
        if ( i-ii == S->interleave_len ) return (-1);
//    if ( i-ii == S->interleave_tmp && kk <= S->perr_org ) return (-1);
    }
    return (i);
}


/* extension for all similarity measures */
/*
INT SEQ_extension (SEQ *S, INT *table, SEQ_LONG x, SEQ_LONG y, SEQ_LONG end_l, SEQ_LONG end_r, INT end, INT err_max, double err_ratio, INT *k, INT flag, INT *reach_to_end){
  INT i, end2=end, dir=1;
  SEQ_CHAR *xx = S->s+x, *yy = S->s+y, *zx=xx, *zy=yy;
  if ( end<0 ){
    end = end2 = -end;
    dir=-1;
    xx--;
    yy--;
    zx += S->plen_org-1;
    zy += S->plen_org-1;
    ENMIN (end, (INT)end_l);
  } else if ( end>0 ){
    ENMIN (end, (INT)end_r -S->plen_org );
    xx += S->plen_org;
    yy += S->plen_org;
  } else return (0);
  *reach_to_end = 0;
  if ( flag&SEQ_HAMMING_DISTANCE ){
    i = SEQ_Ham_ext (S, xx, yy, end, err_max, err_ratio, dir, k);
  } else if ( flag&SEQ_EDIT_DISTANCE ){
    i = SEQ_edit_ext (S, table, xx, yy, end, err_max, err_ratio, dir, k);
  } else i = SEQ_CHam_ext (S, xx, yy, zx, zy, end, err_max, dir, k);
  if ( i == end && end2>end ) *reach_to_end = 1;
  return (i);
}
*/
INT SEQ_extension (SEQ *S, INT *table, SEQ_LONG x, SEQ_LONG y, INT end, INT err_max, double err_ratio, INT *k, INT flag){
    INT i, dir=1;
    SEQ_CHAR *xx = S->s+x, *yy = S->s+y, *zx=xx, *zy=yy;
    if ( end<0 ){
        end = -end;
        dir=-1;
        xx--;
        yy--;
        zx += S->plen_org-1;
        zy += S->plen_org-1;
    } else if ( end>0 ){
        end -= S->plen_org;
        xx += S->plen_org;
        yy += S->plen_org;
    } else return (0);
    if ( flag&SEQ_HAMMING_DISTANCE ){
        i = SEQ_Ham_ext (S, xx, yy, end, err_max, err_ratio, dir, k);
    } else if ( flag&SEQ_EDIT_DISTANCE ){
        i = SEQ_edit_ext (S, table, xx, yy, end, err_max, err_ratio, dir, k);
    } else i = SEQ_CHam_ext (S, xx, yy, zx, zy, end, err_max, dir, k);
    return (i);
}

/* extend the pair under continuous small distance */
/* return non-negative if it can be extended to length end (sum of both directions) */
INT SEQ_extend_pair (SEQ *S, SEQ_LONG x, SEQ_LONG y, INT end, INT err_max, double err_ratio, INT *k){
    INT k1, k2, l;
//  l = (INT)SEQ_extension (S, S->edit_table, x, y, end, end, -end, err_max, err_ratio, &k1, S->flag, &i )
//     +SEQ_extension (S, S->edit_table, x, y, end, end, end, err_max, err_ratio, &k2, S->flag, &ii);
    l = (INT)SEQ_extension (S, S->edit_table, x, y, -end, err_max, err_ratio, &k1, S->flag )
        +SEQ_extension (S, S->edit_table, x, y, end, err_max, err_ratio, &k2, S->flag);
    *k += k1+k2;
    return (l);
}

/* find the limit of the extension to both side */
/* return -1 if the string is not the canonical in the maximal similar string */
/* canonicity is not considered in the case of edit distance */
/* return 1,2,3 if the extension reaches to the end of string */
/* *l,*r will be the length to the limit for left/right direction */
/* *k will be the distance of maximalized string pair */
INT SEQ_maximalize_pair (SEQ *S, SEQ_LONG x, SEQ_LONG y, SEQ_LONG end_l, SEQ_LONG end_r, INT err_max, double err_ratio, INT *k, SEQ_LONG *l, SEQ_LONG *r, INT flag){
    INT z, kl=*k, kr=*k;
    if ( err_ratio == 0.0 && err_ratio >0 ) err_max = (INT)(err_ratio * (end_l +end_r));
    if ( flag&SEQ_HAMMING_DISTANCE ){
        z = SEQ_Ham_canonicity (S, S->s+x-1, S->s+y-1, end_l, err_max, err_ratio, &kl);
    } else if ( flag&SEQ_EDIT_DISTANCE ){
        z = SEQ_extension (S, S->edit_table, x, y, -((INT)end_l), err_max, err_ratio, &kl, S->flag);
//    z = SEQ_extension (S, S->edit_table, x, y, end_l, end_r, -((INT)end_l), err_max, err_ratio, &kl, S->flag, &i);
    } else z = SEQ_CHam_canonicity (S, S->s+x-1, S->s+y-1, end_l, err_max, &kl);

    if ( z<0 ) return (-1);
    *l = z;
//  *r = SEQ_extension (S, S->edit_table, x, y, end_l, end_r, INTHUGE, err_max, err_ratio, &kr, S->flag, &fr);
    *r = SEQ_extension (S, S->edit_table, x, y, end_r, err_max, err_ratio, &kr, S->flag);
    if ( flag&(SEQ_HAMMING_DISTANCE+SEQ_EDIT_DISTANCE) ) *k += kl+kr;
    else ENMAX (*k, MAX (kl, kr));
    return ((end_l==*l?2:0) + (end_r==*r+S->plen_org?1:0));
}

/* compute the possibility of matching randomly chosen letters */
double SEQ_match_letter_prob (SEQ *S){
    INT i;
    double p=0.0, q=0.0;
    FLOOP (i, 0, S->chr_num){
        q += S->chr_cnt[i];
        p += ((double)S->chr_cnt[i])*S->chr_cnt[i];
    }
    return ( (p/q)/q);
}

/* compute the table such that each (i,j)-cell is the Hamming distance of the charactor i,j
  (unperm[i] and unperm[j]) (letter starts from 32)
  each letter is considered as (S->micro_chr_num)-digit */
void SEQ_mk_bit_difference_table (SEQ *S){
    INT i, j, c, ci, cj;
    if ( (S->micro_chr_num > 0 && S->chr_num>223) || S->chr_num>1024 ) return;
    malloc2d (S->cost, S->chr_num, S->chr_num, EXIT);
    FLOOP (i, 0, S->chr_num){
        FLOOP (j, i, S->chr_num){
            if ( S->micro_chr_num == 0 ){
                S->cost[i][j] = S->cost[j][i] = (i==j? 0: ((S->wildcard==0 || (i!=S->wildcard && j!=S->wildcard))?1:0 ));
                continue;
            }
            ci = S->unperm[i]-32;
            cj = S->unperm[j]-32;
            for (c=0 ; ci+cj>0 ; ci/=S->micro_chr_num,cj/=S->micro_chr_num)
                if ( ci%S->micro_chr_num != cj%S->micro_chr_num ) c++;
            S->cost[i][j] = S->cost[j][i] = c;
        }
    }
}

/* compute the table such that each (i,j)-cell is the distance of the charactor i,j
  if the distance is larger than the threshold, the distance will be infinity(S->perr_org)
  (unperm[i] and unperm[j]) (letter starts from 32)
  each letter is considered as (S->micro_chr_num)-digit */
void SEQ_mk_distance_difference_table (SEQ *S){
    INT i, j, c, ci, cj, s, si, sj;
    if ( S->chr_num>223 ) return;
    malloc2d (S->cost, S->chr_num, S->chr_num, EXIT);
    FLOOP (i, 0, S->chr_num){
        FLOOP (j, i, S->chr_num){
            ci = S->unperm[i] - 32;
            cj = S->unperm[j] - 32;
            for (c=0 ; ci+cj>0 ; ci/=S->micro_chr_num,cj/=S->micro_chr_num){
                si = ci%S->micro_chr_num; sj = cj%S->micro_chr_num;
                if ( si < sj ) s = MIN( sj-si, si+S->micro_chr_num-sj);  // cyclic distance
                else s = MIN( si-sj, sj+S->micro_chr_num-si);
//        c += ( s>S->micro_chr_dist? S->perr_org: s);
                c += ( s>S->micro_chr_dist? S->perr_org: s);
//        c += ( s>0? 1: 0);
            }
            S->cost[i][j] = S->cost[j][i] = c;
        }
    }
}



#endif
