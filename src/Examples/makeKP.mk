# User makefile generated by OptFrame MCT (Make a Compilable Thing!)

all: mctAppKP

CC=g++

mctAppKP:      mainKP.o  KPProblemInstance.o  KPEvaluator.o KPConstructiveGreedy.o KPConstructiveRandom.o  KPNSSeqBitFlip.o   KPScanner.o  
	g++ $(GCC_FLAGS) mainKP.o  KPProblemInstance.o  KPEvaluator.o KPConstructiveGreedy.o KPConstructiveRandom.o KPNSSeqBitFlip.o   KPScanner.o -o app_KP

mainKP.o: mainKP.cpp  KP/Representation.h  KP/Solution.h 
	g++ -c $(GCC_FLAGS) mainKP.cpp -o mainKP.o
	
KPProblemInstance.o: KP/ProblemInstance.cpp KP/ProblemInstance.h
	g++ -c $(GCC_FLAGS) KP/ProblemInstance.cpp -o KPProblemInstance.o
	
KPEvaluator.o: KP/Evaluator.cpp KP/Evaluator.h
	g++ -c $(GCC_FLAGS) KP/Evaluator.cpp -o KPEvaluator.o

KPConstructiveRandom.o: KP/ConstructiveRandom.cpp KP/ConstructiveRandom.h
	g++ -c $(GCC_FLAGS) KP/ConstructiveRandom.cpp -o KPConstructiveRandom.o

KPConstructiveGreedy.o: KP/ConstructiveGreedy.cpp KP/ConstructiveGreedy.h
	g++ -c $(GCC_FLAGS) KP/ConstructiveGreedy.cpp -o KPConstructiveGreedy.o

KPNSSeqBitFlip.o: KP/NSSeqBitFlip.cpp KP/NSSeqBitFlip.h
	g++ -c $(GCC_FLAGS) KP/NSSeqBitFlip.cpp -o KPNSSeqBitFlip.o
	
KPScanner.o: ../OptFrame/Scanner++/Scanner.cpp
	g++ -c $(GCC_FLAGS) ../OptFrame/Scanner++/Scanner.cpp -o KPScanner.o



GCC_FLAGS= -g -Wall -O3 --std=c++11

.PHONY: all clean
	
clean: 
	rm -f app_KP
	rm -f KP*.o