// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// ===================================
// Main.cpp file generated by OptFrame
// Project PMedCap
// ===================================

#include <stdlib.h>
#include <math.h>
#include <iostream>


#include "../OptFrame/Move.hpp" // for testing
#include "../OptFrame/Loader.hpp"
#include "../OptFrame/Timer.hpp"
#include "PMedCap.h"


using namespace std;
using namespace optframe;
using namespace pmedcap;

int main(int argc, char **argv)
{
	Timer walltimer;

	srand(time(NULL));
	RandGen rg(time(NULL));


	Scanner scanner(new File("./Examples/PMedCap/InstancesLorena/SJC1"));

	//double tempo = 120;

	PCAPProblemInstance p(scanner);
	PCAPEvaluator e(p);
   GeneralEvaluator<ESolutionPCAP>& e2 = e;
	PCAPInitialSolutionGreedy is_greedy(p, e2, rg);
	//SolutionPCAP s = *is_greedy.generateSolution(100); // timelimit 100
   ESolutionPCAP se = *is_greedy.initialSearch(StopCriteria<EvaluationPCAP>(100)); // timelimit 100

   XSolution& s = se.first;
   ////XEvaluation& e = se.second;

	NSSeqSWAP nsSwap(p, rg);


	s.print();

	e.evaluate(s).print();

	PCAPSolCheck(p, s);

	Loader<RepPCAP, OPTFRAME_DEFAULT_ADS, SolutionPCAP, EvaluationPCAP, ESolutionPCAP> optframe(rg);
	optframe.factory.addComponent(is_greedy);
	optframe.factory.addComponent(e);
	optframe.factory.addComponent(nsSwap);

	// do something!

	cout << "Program ended successfully" << endl;

	return 0;
}
