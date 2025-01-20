MF=     Makefile

CC=     g++

CFLAGS=  -D_USE_64 -fopenmp -fomit-frame-pointer -funroll-loops -msse3 -O3
#-DUSE_KEPT_LENGTH
#CFLAGS=  -D_USE_64 -fopenmp -DNDEBUG -g



#-DCOUNT

#-DUSE_KEPT_LENGTH
#-fomit-frame-pointer -funroll-loops -msse3 -O3
# -DUSE_KEPT_LENGTH
# -fomit-frame-pointer -funroll-loops -msse3 -O3
# -fomit-frame-pointer -funroll-loops -msse3 -O3
#-DNDEBUG -g
#-DUSE_KEPT_LENGTH

# -DVERBOSE
#-fomit-frame-pointer -funroll-loops -msse3 -O3
#-fomit-frame-pointer -funroll-loops -msse3 -O3
#
#
#-fomit-frame-pointer -funroll-loops -O3 -msse3
# -fomit-frame-pointer -funroll-loops -O3 -msse3
#-fomit-frame-pointer -funroll-loops -O3 -msse3
#
#-fomit-frame-pointer -funroll-loops -O3 -msse3
#-DNDEBUG -g
#

#-DNDEBUG -g
#-fomit-frame-pointer -funroll-loops -O3 -msse3
#
#
# -DNDEBUG -g
#-O3
# -fomit-frame-pointer -funroll-loops
#
#-DVERBOSE
#-msse3 -fopenmp -O3 -fomit-frame-pointer -funroll-loops
#-pg
#
#-msse3 -fopenmp -O3 -fomit-frame-pointer -funroll-loops
#-DVERBOSE
#-DNDEBUG -g
#-msse3 -fopenmp -O3 -fomit-frame-pointer -funroll-loops -DVERBOSE
# -DVERBOSE

#
#-g
LFLAGS= -std=c++17 -I ./libsdsl/include/ -I /usr/include/boost -I ./include/ -L ./libsdsl/lib/ -lsdsl -ldivsufsort -ldivsufsort64 -Wl,-rpath=$(PWD)/libsdsl/lib

EXE=    run_baselineReporting

SRC=    baseline_reporting.cpp

#SRC=    baseline_Sep02.cpp

HD=     baseline.h cmdline.h Makefile

#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .cpp .o

OBJ=    $(SRC:.cpp=.o)

.cpp.o:
	$(CC) $(CFLAGS) -c $(LFLAGS) $<




all:    $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS)

$(OBJ): $(MF) $(HD)

clean:
	rm -f $(OBJ) $(EXE) *~

clean-all:
	rm -f $(OBJ) $(EXE) *~
	rm -r libsdsl
	rm -r sdsl-lite



#
#
#Counting_stabbed_char_counting_EXE=    run_counting_stabbed_char_reporting
#Counting_stabbed_char_counting_SRC=    count_stabbed_char.cpp LZ77_char.cpp suffixTree.cpp stNode.cpp truncatedPrefixTree_char.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree_reporting.cpp truncatedSuffixTree_char.cpp
#Counting_stabbed_char_counting_HD=     LZ77_char.h kd_tree_reporting.h suffixTree.h stNode.h truncatedPrefixTree_char.h SA_LCP_LCE.h prefixTree.h prefixNode.h truncatedSuffixTree_char.h Makefile

#
#
#
#Counting_stabbed_char_counting_EXE=    run_counting_stabbed_char_RT
#Counting_stabbed_char_counting_SRC=    count_stabbed_char.cpp LZ77_char.cpp suffixTree.cpp stNode.cpp truncatedPrefixTree_char.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp truncatedSuffixTree_char.cpp
#Counting_stabbed_char_counting_HD=     LZ77_char.h RangeTree.h suffixTree.h stNode.h truncatedPrefixTree_char.h SA_LCP_LCE.h prefixTree.h prefixNode.h truncatedSuffixTree_char.h Makefile
#





Counting_stabbed_char_counting_EXE=    run_counting_stabbed_char_kdtree_reporting
Counting_stabbed_char_counting_SRC=    count_stabbed_char.cpp kkp.cpp kd_tree_reporting.cpp suffixTree.cpp stNode.cpp truncatedPrefixTree_char.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp truncatedSuffixTree_char.cpp
Counting_stabbed_char_counting_HD=     suffixTree.h kkp.h kd_tree_reporting.h stNode.h truncatedPrefixTree_char.h SA_LCP_LCE.h prefixTree.h prefixNode.h truncatedSuffixTree_char.h Makefile



# r tree
#Counting_stabbed_char_counting_EXE=    run_counting_stabbed_char_32
#Counting_stabbed_char_counting_SRC=    count_stabbed_char.cpp kkp.cpp suffixTree.cpp stNode.cpp truncatedPrefixTree_char.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp truncatedSuffixTree_char.cpp
#Counting_stabbed_char_counting_HD=     suffixTree.h kkp.h stNode.h truncatedPrefixTree_char.h SA_LCP_LCE.h prefixTree.h prefixNode.h truncatedSuffixTree_char.h Makefile



Counting_stabbed_char_counting_OBJ=    $(Counting_stabbed_char_counting_SRC:.cpp=.o)


Counting_stabbed_char_counting:    $(Counting_stabbed_char_counting_EXE)

$(Counting_stabbed_char_counting_EXE): $(Counting_stabbed_char_counting_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Counting_stabbed_char_counting_OBJ) $(LFLAGS)

$(Counting_stabbed_char_counting_OBJ): $(MF) $(Counting_stabbed_char_counting_HD)

clean_Counting_stabbed_char_counting:
	rm -f $(Counting_stabbed_char_counting_OBJ) $(Counting_stabbed_char_counting_EXE) *~




STXXL_PATH= ./stxxl
STXXL_INC= -I$(STXXL_PATH)/include
STXXL_LIB= -L$(STXXL_PATH)/lib -lstxxl
CFLAGS_EM = $(CFLAGS) $(STXXL_INC)
LFLAGS_EM = $(LFLAGS) $(STXXL_LIB)

External_EXE=    run_EM_variedRAM

#External_SRC=    IM.cpp utils.cpp
#External_SRC=    Test_EM.cpp utils.cpp

External_SRC= ExternalMining.cpp utils.cpp

External_HD=     ExternalMining.h baseline.h cmdline.h Makefile


External_OBJ = $(External_SRC:.cpp=.o)



External:    $(External_EXE)

$(External_EXE): $(External_OBJ)
	$(CC) $(CFLAGS_EM) -o $@ $(External_OBJ) $(LFLAGS_EM)

$(External_OBJ): $(MF) $(External_HD)

clean_External:
	rm -f $(External_OBJ) $(External_EXE) *~





#
#Test_EXE=    run_test
#
#Test_SRC=    test.cpp
#
#Test_HD=     baseline.h cmdline.h Makefile
#
#
#Test_OBJ=    $(Test_SRC:.cpp=.o)
#
#
#test:    $(Test_EXE)
#
#$(Test_EXE): $(Test_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(Test_OBJ) $(LFLAGS)
#
#$(Test_OBJ): $(MF) $(Test_HD)
#
#clean_test:
#	rm -f $(Test_OBJ) $(Test_EXE) *~
#




Test_EXE=    run_kd_counting

Test_SRC=    kd_test.cpp kd_tree_counting.cpp

Test_HD=     kd_tree_counting.h Makefile


Test_OBJ=    $(Test_SRC:.cpp=.o)


test:    $(Test_EXE)

$(Test_EXE): $(Test_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Test_OBJ) $(LFLAGS)

$(Test_OBJ): $(MF) $(Test_HD)

clean_test:
	rm -f $(Test_OBJ) $(Test_EXE) *~







kkp_EXE=    run_kkp

kkp_SRC=    test_kkp.cpp kkp.cpp common.cpp

kkp_HD=     kkp.h common.h Makefile


kkp_OBJ=    $(kkp_SRC:.cpp=.o)


kkp:    $(kkp_EXE)

$(kkp_EXE): $(kkp_OBJ)
	$(CC) $(CFLAGS) -o $@ $(kkp_OBJ) $(LFLAGS)

$(kkp_OBJ): $(MF) $(kkp_HD)

clean_kkp:
	rm -f $(kkp_OBJ) $(kkp_EXE) *~






baselineMining_EXE=    run_baselineMining

baselineMining_SRC=    baseline_mining.cpp

baselineMining_HD=     baseline.h cmdline.h Makefile


baselineMining_OBJ=    $(baselineMining_SRC:.cpp=.o)


baselineMining:    $(baselineMining_EXE)

$(baselineMining_EXE): $(baselineMining_OBJ)
	$(CC) $(CFLAGS) -o $@ $(baselineMining_OBJ) $(LFLAGS)

$(baselineMining_OBJ): $(MF) $(baselineMining_HD)

clean_baselineMining:
	rm -f $(baselineMining_OBJ) $(baselineMining_EXE) *~



#
#
#MiningFrq_EXE=    run_kd
#
#MiningFrq_SRC=    kd_tree.cpp
#
#MiningFrq_HD=     kd_tree.h cmdline.h Makefile
#
#
#MiningFrq_OBJ=    $(MiningFrq_SRC:.cpp=.o)
#
#
#MiningFrq:    $(MiningFrq_EXE)
#
#$(MiningFrq_EXE): $(MiningFrq_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(MiningFrq_OBJ) $(LFLAGS)
#
#$(MiningFrq_OBJ): $(MF) $(MiningFrq_HD)
#
#clean_MiningFrq:
#	rm -f $(MiningFrq_OBJ) $(MiningFrq_EXE) *~
#



MiningFrq_EXE=    run_MiningFrq

MiningFrq_SRC=    MiningFrq.cpp utils2.cpp

MiningFrq_HD=     karp_rabin_hashing.hpp utils2.h cmdline.h Makefile


MiningFrq_OBJ=    $(MiningFrq_SRC:.cpp=.o)


MiningFrq:    $(MiningFrq_EXE)

$(MiningFrq_EXE): $(MiningFrq_OBJ)
	$(CC) $(CFLAGS) -o $@ $(MiningFrq_OBJ) $(LFLAGS)

$(MiningFrq_OBJ): $(MF) $(MiningFrq_HD)

clean_MiningFrq:
	rm -f $(MiningFrq_OBJ) $(MiningFrq_EXE) *~







#
#Test_Counting_EXE=    run_test_counting
#
##Test_Counting_SRC=    naiver.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp
#
#Test_Counting_SRC=    test_counting.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp
#
#Test_Counting_HD=     cmdline.h counting.h suffixTree.h stNode.h SA_LCP_LCE.h prefixTree.h prefixNode.h kd_tree.h Makefile
#
#
#Test_Counting_OBJ=    $(Test_Counting_SRC:.cpp=.o)
#

#
#Test_Counting:    $(Test_Counting_EXE)
#
#$(Test_Counting_EXE): $(Test_Counting_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(Test_Counting_OBJ) $(LFLAGS)
#
#$(Test_Counting_OBJ): $(MF) $(Test_Counting_HD)
#
#clean_Test_Counting:
#	rm -f $(Test_Counting_OBJ) $(Test_Counting_EXE) *~




Counting_Sampled_EXE=    run_counting_sampled_woba_32

Counting_Sampled_SRC=    count_sampled_woba.cpp sparseSuffixTree.cpp sstNode.cpp krfp.cpp utils2.cpp sparsePrefixTree.cpp sparsePrefixNode.cpp SA_LCP_LCE.cpp

Counting_Sampled_HD=     sparseSuffixTree.h sstNode.h krfp.h utils2.h sparsePrefixNode.h sparsePrefixTree.h SA_LCP_LCE.cpp Makefile


Counting_Sampled_OBJ=    $(Counting_Sampled_SRC:.cpp=.o)


Counting_Sampled:    $(Counting_Sampled_EXE)

$(Counting_Sampled_EXE): $(Counting_Sampled_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Counting_Sampled_OBJ) $(LFLAGS)

$(Counting_Sampled_OBJ): $(MF) $(Counting_Sampled_HD)

clean_Counting_Sampled:
	rm -f $(Counting_Sampled_OBJ) $(Counting_Sampled_EXE) *~










Counting_EXE=    run_counting_32

Counting_SRC=    counting.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp

Counting_HD=     counting.h suffixTree.h stNode.h SA_LCP_LCE.h prefixTree.h prefixNode.h Makefile


Counting_OBJ=    $(Counting_SRC:.cpp=.o)


Counting:    $(Counting_EXE)

$(Counting_EXE): $(Counting_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Counting_OBJ) $(LFLAGS)

$(Counting_OBJ): $(MF) $(Counting_HD)

clean_Counting:
	rm -f $(Counting_OBJ) $(Counting_EXE) *~








#
#
#
#
#produceFrq_EXE=    run_produceFrq
#
#produceFrq_SRC=    ProduceFrq.cpp frq_stNode.cpp frq_suffixTree.cpp
#
#produceFrq_HD=     frq_suffixTree.h frq_stNode.h Makefile
#
#
#produceFrq_OBJ=    $(produceFrq_SRC:.cpp=.o)
#
#
#produceFrq:    $(produceFrq_EXE)
#
#$(produceFrq_EXE): $(produceFrq_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(produceFrq_OBJ) $(LFLAGS)
#
#$(produceFrq_OBJ): $(MF) $(produceFrq_HD)
#
#clean_produceFrq:
#	rm -f $(produceFrq_OBJ) $(produceFrq_EXE) *~
#
#
#




#Counting_stabbed_INT_EXE=    run_counting_stabbed_INT
#
#Counting_stabbed_INT_SRC=    count_stabbed_INT.cpp LZ77_INT.cpp LZ77_suffixTree_INT.cpp LZ77_stNode_INT.cpp LZ77_SA_LCP_LCE_INT.cpp LZ77_prefixTree_INT.cpp LZ77_prefixNode_INT.cpp kd_tree.cpp truncatedSuffixTree_INT.cpp
#
#Counting_stabbed_INT_HD=     LZ77_INT.h LZ77_suffixTree_INT.h LZ77_stNode_INT.h LZ77_SA_LCP_LCE_INT.h LZ77_prefixTree_INT.h LZ77_prefixNode_INT.h kd_tree.h truncatedSuffixTree_INT.h Makefile
#
#
#Counting_stabbed_INT_OBJ=    $(Counting_stabbed_INT_SRC:.cpp=.o)
#
#
#Counting_stabbed_INT:    $(Counting_stabbed_INT_EXE)
#
#$(Counting_stabbed_INT_EXE): $(Counting_stabbed_INT_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(Counting_stabbed_INT_OBJ) $(LFLAGS)
#
#$(Counting_stabbed_INT_OBJ): $(MF) $(Counting_stabbed_INT_HD)
#
#clean_Counting_stabbed_INT:
#	rm -f $(Counting_stabbed_INT_OBJ) $(Counting_stabbed_INT_EXE) *~


#Counting_stabbed_char_EXE=    run_counting_stabbed_char
#
#Counting_stabbed_char_SRC=    count_stabbed_char.cpp LZ77_char.cpp suffixTree.cpp stNode.cpp truncatedPrefixTree_char.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp truncatedSuffixTree_char.cpp
#
#Counting_stabbed_char_HD=     LZ77_char.h suffixTree.h stNode.h truncatedPrefixTree_char.h SA_LCP_LCE.h prefixTree.h prefixNode.h kd_tree.h truncatedSuffixTree_char.h Makefile
#
#
#Counting_stabbed_char_OBJ=    $(Counting_stabbed_char_SRC:.cpp=.o)
#
#
#Counting_stabbed_char:    $(Counting_stabbed_char_EXE)
#
#$(Counting_stabbed_char_EXE): $(Counting_stabbed_char_OBJ)
#	$(CC) $(CFLAGS) -o $@ $(Counting_stabbed_char_OBJ) $(LFLAGS)
#
#$(Counting_stabbed_char_OBJ): $(MF) $(Counting_stabbed_char_HD)
#
#clean_Counting_stabbed_char:
#	rm -f $(Counting_stabbed_char_OBJ) $(Counting_stabbed_char_EXE) *~


#




