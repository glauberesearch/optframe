#ifndef PN_CONSTRUCTIVE_RandomGreedy_H_
#define PN_CONSTRUCTIVE_RandomGreedy_H_

#include <OptFrame/Constructive.hpp>
//#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace PN {

class ConstructiveRandomGreedy : public Constructive<SolutionPN> //<RepPN> //, MY_ADS>
{
private:
   ProblemInstance& pPN;

   // Your private vars

public:
   ConstructiveRandomGreedy(ProblemInstance& _pPN);

   virtual ~ConstructiveRandomGreedy();

   //Solution<ESolutionPN>* generateSolution(double timelimit);
   std::optional<SolutionPN> generateSolution(double timelimit) override;

   void print() const
   {
      cout << "ConstructiveRandomGreedy" << endl;
   }
};

}

#endif /*PN_CONTRUCTIVE_RandomGreedy_H_*/
