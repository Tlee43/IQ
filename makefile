CXX?=g++
CC=g++
MSAT=ld

CSRCS    = $(wildcard ./src/*.cpp)
COBJS    = $(CSRCS:./src/%.cpp=./debug/%.o)
LIBD =
LIBS =
CXX?=g++
CC=g++

MINISAT_DIR=./src/minisat/build/release/lib
LIBD+=-L$(MINISAT_DIR)
MINISAT_LIB=$(MINISAT_DIR)/libminisat.a
MSAT=lr

CFLAGS+=-I./src/minisat
LIBS+=-lminisat

all: ./debug satmake app.exe

./debug:
	mkdir ./debug

satmake:
	@cd ./src/minisat ; make CXX=$(CXX) $(MSAT)
	
app.exe:  $(COBJS)
	@echo Linking: $@
#	for debug
#	$(CXX) -ggdb -o $@ $(COBJS) $(LIBD) $(LIBS) -lz #-lstdc++fs
	
#	for test
	$(CXX) -DNDEBUG -O3 -o $@ $(COBJS) $(LIBD) $(LIBS) -lz #-lstdc++fs
	
./debug/%.o: ./src/%.cpp
	@echo Compiling: $@
#	for debug
#	$(CXX) $(CFLAGS) -ggdb -c $<
	
#	for test
	$(CXX) $(CFLAGS) -DNDEBUG -c -o ./$@ $<
	

	
