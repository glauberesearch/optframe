// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef EtII_EVALUATOR_HPP_
#define EtII_EVALUATOR_HPP_

//#include <cmath>
#include <math.h>
#include <stdlib.h>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>

#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

using namespace optframe;

#define EPSILON_EtII 0.0001

extern int numEvs;

namespace EtII {

class EtIIEvaluator : public Evaluator<SolutionEtII, EvaluationEtII>
{
private:
   ProblemInstance& pEtII;

public:
   using Evaluator<SolutionEtII, EvaluationEtII>::evaluate;

   EtIIEvaluator(ProblemInstance& _pEtII)
     : Evaluator<SolutionEtII, EvaluationEtII>(new Maximization<EvaluationEtII>(), true)
     , pEtII(_pEtII) // DISALLOW COSTS (DEFAULT)
   {
      // Put the rest of your code here
   }

   Evaluation<> evaluate(const SolutionEtII& s) override
   {
      const RepEtII& rep = s.getR();
      //counting evaluations.
      numEvs++;
      //cout<<"##### Number of evaluates: "<<numEvs<<endl;

      int fo = 0; // Evaluation<> Function Value

      // horizontal
      for (unsigned int i = 0; i < rep.getNumRows(); i++)
         for (unsigned int j = 0; j < rep.getNumCols() - 1; j++)
            if (rep(i, j).right == rep(i, j + 1).left)
               fo++;

      // vertical
      for (unsigned int j = 0; j < rep.getNumCols(); j++)
         for (unsigned int i = 0; i < rep.getNumRows() - 1; i++)
            if (rep(i, j).down == rep(i + 1, j).up)
               fo++;

      return Evaluation<>(fo);
   }

   virtual bool betterThan(double f1, double f2)
   {
      return (f1 > (f2 - EPSILON_EtII));
   }

   virtual bool isMinimization() const
   {
      return false;
   }
};
}

#endif /*EtII_EVALUATOR_HPP_*/
