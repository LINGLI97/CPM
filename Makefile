MF=     Makefile

CC=     g++

CFLAGS=  -D_USE_64 -fopenmp -fomit-frame-pointer -funroll-loops -O3 -msse3
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
LFLAGS= -std=c++17 -I ./libsdsl/include/ -L ./libsdsl/lib/ -lsdsl -ldivsufsort -ldivsufsort64 -Wl,-rpath=$(PWD)/libsdsl/lib

EXE=    CPM

SRC=    baseline.cpp

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








STXXL_PATH= ./stxxl
STXXL_INC= -I$(STXXL_PATH)/include
STXXL_LIB= -L$(STXXL_PATH)/lib -lstxxl
CFLAGS += $(STXXL_INC)
LFLAGS += $(STXXL_LIB)

External_EXE=    run_EM

#External_SRC=    Test_EM.cpp utils.cpp
External_SRC= MiningExternal.cpp utils.cpp

External_HD=     MiningExternal.h cmdline.h Makefile


External_OBJ = $(External_SRC:.cpp=.o)



External:    $(External_EXE)

$(External_EXE): $(External_OBJ)
	$(CC) $(CFLAGS) -o $@ $(External_OBJ) $(LFLAGS)

$(External_OBJ): $(MF) $(External_HD)

clean_External:
	rm -f $(External_OBJ) $(External_EXE) *~










Test_EXE=    run_test

Test_SRC=    test.cpp

Test_HD=     baseline.h cmdline.h Makefile


Test_OBJ=    $(Test_SRC:.cpp=.o)


test:    $(Test_EXE)

$(Test_EXE): $(Test_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Test_OBJ) $(LFLAGS)

$(Test_OBJ): $(MF) $(Test_HD)

clean_test:
	rm -f $(Test_OBJ) $(Test_EXE) *~




Test_Counting_EXE=    run_test_counting

#Test_Counting_SRC=    naiver.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp

Test_Counting_SRC=    test_counting.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp

Test_Counting_HD=     cmdline.h counting.h suffixTree.h stNode.h SA_LCP_LCE.h prefixTree.h prefixNode.h kd_tree.h Makefile


Test_Counting_OBJ=    $(Test_Counting_SRC:.cpp=.o)


Test_Counting:    $(Test_Counting_EXE)

$(Test_Counting_EXE): $(Test_Counting_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Test_Counting_OBJ) $(LFLAGS)

$(Test_Counting_OBJ): $(MF) $(Test_Counting_HD)

clean_Test_Counting:
	rm -f $(Test_Counting_OBJ) $(Test_Counting_EXE) *~





Counting_Sampled_EXE=    run_counting_sampled

Counting_Sampled_SRC=    count_sampled.cpp sparseSuffixTree.cpp sstNode.cpp kd_tree.cpp krfp.cpp utils2.cpp sparsePrefixTree.cpp sparsePrefixNode.cpp

Counting_Sampled_HD=     sparseSuffixTree.h sstNode.h kd_tree.h krfp.h utils2.h sparsePrefixNode.h sparsePrefixTree.h Makefile


Counting_Sampled_OBJ=    $(Counting_Sampled_SRC:.cpp=.o)


Counting_Sampled:    $(Counting_Sampled_EXE)

$(Counting_Sampled_EXE): $(Counting_Sampled_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Counting_Sampled_OBJ) $(LFLAGS)

$(Counting_Sampled_OBJ): $(MF) $(Counting_Sampled_HD)

clean_Counting_Sampled:
	rm -f $(Counting_Sampled_OBJ) $(Counting_Sampled_EXE) *~


















Counting_EXE=    run_counting

Counting_SRC=    counting.cpp suffixTree.cpp stNode.cpp SA_LCP_LCE.cpp prefixTree.cpp prefixNode.cpp kd_tree.cpp

Counting_HD=     counting.h suffixTree.h stNode.h SA_LCP_LCE.h prefixTree.h prefixNode.h kd_tree.h Makefile


Counting_OBJ=    $(Counting_SRC:.cpp=.o)


Counting:    $(Counting_EXE)

$(Counting_EXE): $(Counting_OBJ)
	$(CC) $(CFLAGS) -o $@ $(Counting_OBJ) $(LFLAGS)

$(Counting_OBJ): $(MF) $(Counting_HD)

clean_Counting:
	rm -f $(Counting_OBJ) $(Counting_EXE) *~




.PHONY: all clean
