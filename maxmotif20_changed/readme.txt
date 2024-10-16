######################################################################
MaxMotif: Linear time algorithm for maximal motif mining
     Coded by Takeaki Uno,   e-mail:uno@nii.jp, 
        homepage:   http://research.nii.ac.jp/~uno/index.html
######################################################################

** This program is available for only academic use, basically.   **
** Anyone can modify this program, but he/she has to write down  **
** the change of the modification on the top of the source code. **
** Neither contact nor appointment to Takeaki Uno is needed.     **
** If one wants to re-distribute this code, do not forget to     **
** refer the newest code, and show the link to homepage          **
** of Takeaki Uno, to notify the news about codes for the users. **
** For the commercial use, please make a contact to Takeaki Uno. **

################################
####   Problem Definition   ####
################################

Consider a set of letters, and a wildcard which matches any letter.
Here we call a string including only letters "string", string including
both letters and wildcards, and whose first letter and last last are
not wildcard "motif". "ABCDE" is a string, "AooDoF" is a motif, and 
"ooAAABBo" is not a motif where 'o' is wildcard. A string is also a
motif. A motif A is said to be included in the other motif B if A
can be a substring of B by changing some wild cards to letters. 
We say that A appears at B, and the substring matches to A. For example,
"AoCoE" is included in both "ABCDE" and "CDAoCDEFF", but not in
"ABACEEE". For a string S, the frequency of a motif P is the number of 
positions x such that the substring starting at x of length equal to P
includes S. For example, the frequency of "AoB" in "AABBAAB" is 3.
For given a string S and a threshold value theta, called minimum support 
or quorum, a motif is called frequent if its frequency is at least theta
(sometimes frequent motif is called motif, simply).
A motif is called closed motif (or maximal motif) if it is included in 
the other motif with the same frequency. For given a string S and a
threshold value theta, the problem is to enumerate all the frequent motifs,
or frequent closed motifs.

#################################
####    Input File Format    ####
#################################

MaxMotif inputs a text file, and compare the strings in the file, 
so that each character is a letter (thus basically can read any 
binary file). Strings to be compared are all substrings of the given
length in the text file. By giving a command, MaxMotif regards two
or more consecutive newline characters (such as "\n") as a separator
of (long) strings, so that we can give several (long) strings. In this
case, MaxMotif does not take any substring that start from a string and
end at another string (this mode is called "multi-string mode"). 
Note that the newline characters at the beginning of the strings are 
considered as a part of a separator, thus any string start with non-
newline characters. MaxMotif accepts any newline characters, windows
("\n\r"), UNIX ("\n") and Macintosh ("\r").


#############################################
####    Usage and input/output format    ####
#############################################

    ==== How to Compile ====
Unzip the file into arbitrary directory, and execute "make".
Then you can see "maxmotif" (or maxmotif.exe) in the same directory.

    ====  Command Line Options   ====
To execute MaxMotif, just type maxmotif and give some parameters as follows.

% maxmotif FCnfqISZR [options] input-filename support [output-filename]

"%" is not needed to type. It is a symbol to represent command line.
To see a simple explanation, just execute "maxmotif" without parameters.

"input-filename" is the filename of the input string database.
It can be *any* text file (or binary file).
The 1st letter of input-filename must not be '-'. Otherwise it is 
regarded as an option. Each character of the input is regarded as a 
letter, thus we can not give a problem of over 256 letters, in this version.
"output-filename" is the name of file to write the itemsets the program
finds. You can omit the output file to see only the number of frequent
motifs in the database.


The first parameter is given to the program to indicate the task.
 F: enumerate frequent motifs,
 C: enumerate frequent closed motifs

For example, if you want to enumerate frequent closed motifs, type 
"C" in the first parameter. Additionally, we can give the following
commands, to specify the input/output style: 

The following commands give some variations for the tasks and 
the output mode.

 c:multi string comparison
    by giving this option, MaxMotif considers the input file as
    a set of strings. The separator of strings is two consecutive 
    newline characters. The details are described in the input file
    format section.
 D:count the frequency by document occurrence
    MaxMotif counts the frequency by how many positions it appears.
    This way is called "position occurrence". On contrary, in 
    "document occurrence", the frequency is counted by the number 
    of strings that have positions at which the motif appears.
    In the multi string mode given by 'c', 'D' command indicates to
    use the document occurrence.
 _:no message
    maxmotif outputs some internal computational parameters and the
    property of the input files such as length of the input string
    and the number of strings in the multi-string mode (output to 
    standard error). By giving this command, maxmotif does not output
    such information.
 +:append output to a (existing) file
    the output of MaxMotif will be appended to output file, so the
    output file will not be cleared, if it exists.
 %:show progress of the computation
    the progress of the computation is printed to standard error for
    each 1,000,000 solutions found
 f:output the frequency on the end of each motif found,
    this command means to write the frequency of the pattern 
    following to the output pattern in each line of the output file.
 I: output positions
    by this command, MaxMotif outputs the position of the appearance
    of each output motif pattern to the line next to the line of the
    motif pattern, in the output file. In the multi string mode
    specified by 'c' command, the position will be given by a pair
    of string ID and the position in the string. Further, in the 
    matrix mode given by -3 option below, the position will be given
    by a tuple of (string ID, segment ID, position).


The followings are input format commands.

 i:ignore newline characters in the input file,
    MaxMotif reads a text file, with regarding each character as a
    letter, thus basically can input any binary string. By giving 
    this command, all newline-codes ('\n' and '\r') are removed from
    the input, and not counted. In the multi-string mode, the 
    separations are two or more consecutive newlines but they are 
    not ignored.
 G:genome sequence mode
    this is the mode of genome sequence mode (fasta file mode). In
    precise, any letter aAtTgGcC will be ATGC, nNxX will be N, which
    acts as a wildcard, and other characters will be ignored. Newline
    codes are ignored (same as i command). Further, a line starting
    with '>' is regarded as a comment line, and will be ignored.
 J (for genome sequence mode): regard comment as a separator
    a comment line is considered as a separator, thus if a string
    includes a comment line, the string will be separated at the
    comment.
 J (for maxmotif16):read text as EUC text (Japanese text code)
    this is the Japanese text mode (EUC mode), which can read both 
    8 bit characters and 16 bit characters, and external 24 bit
    characters. This is valid only for maxmotif16 program (16 bit mode
    executable file).
 J (for maxmotif32):regard each byte as 1 digit of at most 200 
    maxmotif32 reads 4bytes as a character. It actually conflicts to
    newline codes, since we can not give character whose first 1 byte is
    a newline code. By giving this option, maxmotif32 regards each byte of 
    4 bytes as 1 digit of at most 199, such that 32 is 0, 33 is one,...
    and 231 is 199. For example, the 4bytes 32 33 35 39 corresponding to
    one character will be decoded to (32-32)*200*200*200 + (33-32)*200*200
    + (35-32)*200 + (39-32) = 40607. maxmotif32 accepts the characters of
    less than 10,000,000, and if the decoded number is over 10,000,000 
    for example (35-32)*200*200*200 = 24,000,000, it will be regarded as 0.
    It is common to the usual mode.

The following is a list of options to give additional information to
the program, or restrict the motifs to be found. You can place them
between the first parameter and the second parameter, as many as you
want. Giving no option is also allowed.

 -2 [filename]:read additional file, attached to the input file
   maxmotif inputs one file. This option enables to read the second
   file. The strings will be attached to the end of the first file.
   In multi-string mode, the beginning of the second file is regarded
   as a separator.
 -d [num]:duplicate strings from [num]th to the end
   the strings with string ID from [num] to the last string will be
   duplicated, and appended to the last string. Thus the first string
   of the duplicated strings has one larger string ID than the 
   last string. When [num] is -1 or -3, it is automatically
   changed to a certain number (see (*1)).
 -r [num]:reverse strings from [num]th string to the last string
   reverse the strings from string ID [num] to the last. In the genome
   mode, each letter will be transformed to its complement (A-T, G-C).
   When [num] is -1 or -2, it is automatically changed to a
   certain number (see (*1)).
 -3 [num]:matrix mode for each string up to [num]th
   in matrix mode, each string will be considered as a matrix such that
   each cell is a letter of the string. A row of the matrix corresponds
   to a substring of the string, so that the string is divided into 
   several substrings of the same length and the i-th substring is 
   the i-th row. In the matrix mode, MaxMotif takes only substrings 
   which are totally contained in a row. The length of the rows is
   given by the length to the first newline character of the string in
   the input file. It means that we can input matrices of different
   row sizes. When [num] is -1, -2 or -3, it is automatically
   changed to a certain number (see (*1)).
 -w [char] :specify character for wildcard in the output file
      if -w % is given, the wildcard will be %, such as A%%%B.
      The default wildcard is '#'.
 -W [filename]:read string weight file. If this option will be given
     twice, the latter will be regarded as the weight file of the
     second file, given by -2 option. When string weight is given,
     the frequency is counted by the sum of string weights to which
     the motif appearance positions belong. In the document occurrence
     mode, the frequency is counted by the sum of weights of the
     strings including the motif.

(*1) for -d, -r and -3 options, when [num] is -1, the value will be
set to the first string in the second file when -2 option is given,
and the or the half of strings (except for -d). If [num] is -2, [num]
will be changed to the ID of top string generated by the duplication
given by -d option. For -3 option, if [num] is -3, it will
be changed to the ID of last string +1, which means all strings.

The following options are for the constraints for the motifs.

 -l [num]: output patterns with size at least [num]
 -u [num]: output patterns with size at most [num]
      if -l 3 and -u 6 are given, only motifs with 3 to 6 letters 
      will be output, so ABooC will be output, but neither AB nor
      ABCoooDE will not be output.
 -L [num]: output patterns with at least [num] solid letters
 -U [num]: output patterns with at most [num] solid letters
      if -L 3 and -U 6 are given, only motifs with 3 to 6 solid letters
      will be output, so ABoCoooD will be output, but neither Aoob
      nor ABCoooDEFoooGH will not be output.
 -p [num]: upper bound for quorum (or support, maximum support)
      if -p 10 is given, a motif appears at 12 positions will not be
      output.
 -m [num]: upper bound for consecutive wildcards
      if -m 2 is given, AoooC will not be allowed. The default value
      is 10.
 -R [num] :reduce redundant patterns, i.e., ignore the positions such
     that the substring from the matches A reaches to the position.

Examples)

- To find all frequent motifs in "example.txt" for support 3, and length of
 no less than 2. Output frequencies of each motif found. Do not output 
 to file.

% maxmotif Ff -l 2 test.dat 3

- To find all frequent closed motifs in "example.txt" for support 4, and
  the character representing wildcards in the output file to 'o'.
   Output positions of each motif appears.

% maxmotif CI -w 'o' test.dat 4


###########################
####    Performance    #### 
###########################

The performance of Maxmotif is stable, for both computation time and memory
use. The initialization and preprocess time is linear in the size of 
input string. The computation time for finding frequent motifs depends
on the number of motifs found, and the minimum support.
When the minimum support is large and the number of the motifs found is
small, the computation time for each motif is relatively large. However, 
computation time per motif decreases as the increase of the motifs found,
and roughly speaking when the size of output file is equal to the input
database size, it will be constant, such as 1/1,000,000 sec (by PC with 
CPU of 2GHz.)

Memory usage of MaxMotif is also stable. It is an advantage compared to
other implementations. The memory usage of MaxMotif is almost linear in
the size of the input database. Approximately MaxMotif uses integers at
most three times as much as the input string length. The memory usage of
the other implementations increases as the increase of the number of 
motifs found, but that of MaxMotif does not.


###################################################
####    Algorithms and Implementation Issue    #### 
###################################################

We start with the explanation of frequent motif mining.
The basic idea of the algorithm is depth first search. Let S=(s1,s2,...,sm) 
be the given string, t be the minimum support. For a motif P, the location 
list L(P) is the positions such that the substring of S starting at the
positions matches P. MaxMotif starts with empty string. Then, appending
(possibly 0) wildcards and a letter, we generates candidates of frequent
motifs. We call this operation extension. We generate all possible extensions,
and compute the frequency of each candidate by computing the location lists,
and for the frequent ones, we generate recursive calls to enumerate motifs
obtained by further extensions from the motif.
Any motif can be obtained by iterative extension from the emptyset, 
and the way of extensions is unique, thus we have no duplication.
This algorithm is written as follows:

  FrequentMotifMining (S:string, P:motif )
    Output P
    For each letter l and gap c, 
      P' := P + "c wildcards" + l
      if (P') is frequent, then FrequentMotifMining (S, P')

By calling FrequentMotifMining (S, emptyset), we can enumerate all frequent
motifs. This algorithm outputs one motif, thus the computation time of the
algorithm is basically linear in the output size.

However, a straightforward implementation of this algorithm is very slow,
since computing frequency of P' takes long time. To be fast, we use 
occurrence deliver explained below.

   === occurrence deliver ===
We suppose that each transaction is sorted. The occurrence deliver computes
L(P') for all extensions P', in time linear in the sum of their sizes.
First, for each couple of letter and gap, we give an empty bucket. Then,
for each position x in L(P'), we scan S from x + (length of P), and for each 
letter c at gap l, insert x to the bucket of the couple c and l.
After scanning all position x in L(P), the bucket of couple (c,l) is L(P')
where P' is the motif obtained by appending l wildcards and c.

   === closed motif mining ===
To enumerate closed motifs, we use the ppc extension (prefix preserving
closure extension). For a motif P, we define the closure C(P) by the closed
motif including P and having the same frequency to P. C(P) is obtained from 
P by replacing its wildcards by characters one by one, in linear time 
in the length of S.
When P is a closed motif, we define its core index, denoted by core_i(P),
by the minimum length i such that C(P(i)) = P where P(i) is the string with 
wildcards of first i letters. Then, a closed motif P' is said to be a
 ppc extension of P if and only if 

(i) P' = C(P->(i,c)) for some i>core_i(P).
(ii) P(i) = P'(i).

where P->(i,c) is the string obtained from P by replacing ith-letter to c.
Note that P' has smaller frequency than P. It is proved that any closed
motif is a ppc extension of the other unique closed motif.
Thus, the binary relation ppc extension induces a rooted tree whose root
is C(empty string). Thus, starting from the smallest closed motif (which
is usually empty string) and find ppc extension recursively for all i, we
can perform depth first search on the rooted tree. 



###############################
####    Acknowledgments    #### 
###############################

We thank to Ken Satoh of National Institute of Informatics Japan, Hiroki
Arimura of Hokkaido University for their contributions for the research,
A part of the research of MaxMotif is supported by joint-research fund of
National Institute of Informatics Japan, and Grant-in-aid from the
Ministry of Education, Science, Sport and Culture of Japan
(Monbu-Kagaku-Sho).


##########################
####    References    #### 
##########################

[1] Hiroki, Arimura, Takeaki Uno, "An Efficient Polynomial Space and
Polynomial Delay Algorithm for Enumeration of Maximal Motifs in a Sequence",
to appear in Journal of Combinatorial Optimization, 2006

[2] Hiroki Arimura, Takeaki Uno, "Polynomial Space and Polynomial Delay
Algorithm for Enumeration of Maximal Motifs in a Sequence", ISAAC2005,
Lecture Notes in Computer Science 3827, pp. 724-737, 2005
