# User makefile generated by OptFrame MCT (Make a Compilable Thing!)

all: mctAppPN

CC=g++

mctAppPN:      mainPN.o  PNProblemInstance.o  PNEvaluator.o PNConstructiveGreedy.o PNConstructiveRandom.o  PNNSSeqBitFlip.o   PNScanner.o  
	g++ $(GCC_FLAGS) mainPN.o  PNProblemInstance.o  PNEvaluator.o PNConstructiveGreedy.o PNConstructiveRandom.o PNNSSeqBitFlip.o   PNScanner.o  -o app_PN

mainPN.o: mainPN.cpp  PN/Representation.h  PN/Solution.h 
	g++ -c $(GCC_FLAGS) mainPN.cpp -o mainPN.o
	
PNProblemInstance.o: PN/ProblemInstance.cpp PN/ProblemInstance.h 
	g++ -c $(GCC_FLAGS) PN/ProblemInstance.cpp -o PNProblemInstance.o
	
PNEvaluator.o: PN/Evaluator.cpp PN/Evaluator.h
	g++ -c $(GCC_FLAGS) PN/Evaluator.cpp -o PNEvaluator.o

PNConstructiveRandom.o: PN/ConstructiveRandom.cpp PN/ConstructiveRandom.h
	g++ -c $(GCC_FLAGS) PN/ConstructiveRandom.cpp -o PNConstructiveRandom.o

PNConstructiveGreedy.o: PN/ConstructiveGreedy.cpp PN/ConstructiveGreedy.h
	g++ -c $(GCC_FLAGS) PN/ConstructiveGreedy.cpp -o PNConstructiveGreedy.o

PNNSSeqBitFlip.o: PN/NSSeqBitFlip.cpp PN/NSSeqBitFlip.h
	g++ -c $(GCC_FLAGS) PN/NSSeqBitFlip.cpp -o PNNSSeqBitFlip.o
	
PNScanner.o: ../OptFrame/Scanner++/Scanner.cpp
	g++ -c $(GCC_FLAGS) ../OptFrame/Scanner++/Scanner.cpp -o PNScanner.o

#PNprintable.o: ../OptFrame/Util/printable.cpp
#	g++ -c $(GCC_FLAGS) ../OptFrame/Util/printable.cpp -o PNprintable.o


GCC_FLAGS= -g -Wall -O3 --std=c++17 -fconcepts #-fno-exceptions -fno-rtti #TODO: disable exceptions

#.PHONY: all #clean
	
clean: 
	rm -f app_PN
	rm -f PN*.o
