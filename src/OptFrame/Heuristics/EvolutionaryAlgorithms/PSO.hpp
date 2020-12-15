// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_PSO_HPP_
#define OPTFRAME_PSO_HPP_

#include <algorithm>

#include "../../IPopulational.hpp"
///#include "../../InitialPopulation.h"
#include "../../EPopulation.hpp"
#include "../../SingleObjSearch.hpp"

#include "../../Util/printable.h" // TODO: remove!

/*
#include "Crossover.hpp"
#include "DecoderRandomKeys.hpp"
#include "Elitism.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"
*/

// Particle Swarm Optimization - PSO

namespace optframe {

// begin PSO
//template<XEvaluation XEv = Evaluation<>, optframe::comparability KeyType = double>
///template<optframe::comparability KeyType = double>
struct Bird
{
   vector<double> velocity;  // assuming 'double' for now
   vector<double> position;  // assuming 'double' for now
   vector<double> localbest; // assuming 'double' for now

   // TODO: cache localbest XEv here, and make Bird some IESolution... looks promising!

   friend std::ostream& operator<<(std::ostream& os, const Bird& b)
   {
      os << "bird{"
         << "velocity: " << b.velocity
         << " position:" << b.position
         << " localbest:" << b.localbest << "}";
      return os;
   }
};

template<
  ///optframe::comparability KeyType = double,
  XEvaluation XEv = Evaluation<>,
  XESolution XES = std::pair<std::vector<double>, XEv>,
  XESolution XES2 = std::pair<Bird, Evaluation<>>,
  XSearch<XES2> XSH2 = Population<XES2>>
class PSO : public SingleObjSearch<XES, XES2, XSH2>
// TODO: add IPopulational
//, public IPopulational<XES, XES, XES2>
{
   using S = typename XES::first_type;
   //using XEv = typename XES::second_type;
   using XSH = XES;

   //private:
   //   Bird Global;

protected:
   Evaluator<S, XEv, XES>& evaluator; // Check to avoid memory leaks
   //InitialPopulation<XES2>& initPop; // TODO: add if necessary
   /*
      RANDOM PSO
    * pop_size -> Número de pájaros (o partículas)
    * iter_max -> Número máximo de iteraciones de PSO
    * cI -> Vector con los topes inferiores de los parámetros
    * cS -> Vector con los topes superiores de los parámetros
    * FUNCION -> Función que pretendemos minimizar
    * 
    * */
   // population size
   unsigned pop_size;
   // number of generations (stop criteria)
   int iter_max;
   // -> Vector con los topes inferiores de los parámetros
   vector<double> cI;
   // -> Vector con los topes superiores de los parámetros
   vector<double> cS;
   // random number generator
   RandGen& rg;

public:
   PSO(Evaluator<S, XEv, XES>& evaluator, unsigned pop_size, int iter_max, const vector<double>& cI, const vector<double>& cS, RandGen& _rg)
     : evaluator(evaluator)
     , pop_size(pop_size)
     , iter_max(iter_max)
     , cI(cI)
     , cS(cS)
     , rg{ _rg }
   {
   }

   virtual EPopulation<XES2> generatePopulation()
   {
      EPopulation<XES2> swarm;
      for (unsigned i = 0; i < this->pop_size; i++) {
         // create a new Bird
         Bird b;
         for (unsigned j = 0; j < this->cI.size(); j++) {
            b.position[j] = this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01();
            b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01());
         }
         // add bird to swarm (no evaluation is given)
         swarm.push_back(b);
      }
      return swarm;
   }

   // update bird position (random mutation)
   void birdMutation(Bird& b)
   {
      if (rg.rand01() < 0.1 * 0.1) {
         for (unsigned j = 0; j < this->cS.size(); j++) {
            double r = rg.rand01();
            if (r < 0.5)
               b.position[j] += (cS[j] - cI[j]) * (::pow(2.0 * r, 1.0 / 21) - 1);
            else
               b.position[j] += (cS[j] - cI[j]) * (::pow(2.0 * (1 - r), 1.0 / 21) + 1);
         }
      }
   }

   // checks if bird position/velocity is within valid bounds
   void boundCheck(Bird& b)
   {
      unsigned count = 0;
      while (count < this->cS.size()) {
         if (b.position[count] < cI[count] || b.position[count] > cS[count]) {
            for (unsigned j = 0; j < this->cS.size(); j++) {
               b.position[j] = this->cI[j] + (this->cS[j] - this->cI[j]) * rg.rand01();
               b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[j] - this->cI[j]) * rg.rand01());
            }
            break;
         }
         count++;
      } // while
   }    // BoundCheck

   virtual ~PSO()
   {
      // nothing to delete... NICE!
   }

   SearchStatus search(const StopCriteria<XEv>& stopCriteria) override
   {
      if (Component::debug)
         (*Component::logdata) << "PSO search():"
                               << " pop_size=" << pop_size << " iter_max=" << iter_max
                               << std::endl;

      // beware that 'star' may not be a Bird...
      op<XES>& star = this->best;

      // check if time/target conditions are met
      if (stopCriteria.shouldStop(std::nullopt))
         return SearchStatus::NO_REPORT;

      // generates initial swarm (of size 'pop_size', see parameter)
      EPopulation<XES2> swarm = generatePopulation();

      // update position of birds and check/adjust bounds
      for (unsigned i = 0; i < this->pop_size; i++) {
         Bird& b = swarm.at(i).first;
         for (unsigned j = 0; j < this->cS.size(); j++) {
            // update positions of bird b, based on its velocities
            b.position[j] += b.velocity[j];
         }
         boundCheck(b);
         // register first 'localbest' of particle
         b.localbest = b.position;
         XEv e = evaluator.evaluate(b.position);
         swarm.setFitness(i, e);
      }

      // first global best
      XES2 global = swarm.at(0);
      for (unsigned i = 1; i < this->pop_size; i++) {
         // compares localbest of every particle with global best
         if (evaluator.betterThan(swarm.getFitness(i), global.second))
            global = swarm.at(i);
      }

      // count generations
      int count_gen = 0;

      // main loop
      while (count_gen < iter_max) {
         // Particle update
         for (unsigned i = 0; i < this->pop_size; i++) {
            Bird& b = swarm.at(i).first;

            // 0.1 chance of generating a random guy
            if (rg.rand01() < 0.1) {
               for (unsigned j = 0; j < this->cS.size(); j++) {
                  b.position[j] = this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01();
                  b.velocity[j] = 0.1 * (this->cI[j] + (this->cS[i] - this->cI[i]) * rg.rand01());
               }
            } else {
               // 0.9 chance to just update the particle
               for (unsigned j = 0; j < this->cS.size(); j++)
                  b.position[j] += b.velocity[j];
            }

            // 0.01 chance to do specific mutation
            birdMutation(b);

            // After updating, check to see if there's local upgrades
            boundCheck(b);
            // execute evaluation function
            //XEv e = evaluator.evaluate(b);
            //swarm.setFitness(i, e);
            //if (evaluator(b.position) < evaluator(b.localbest)))
            //b.localbest = b.position;
         }

         // evaluate each bird and update best
         int localBest = -1;
         for (unsigned i = 0; i < this->pop_size; i++) {
            Bird& b = swarm.at(i).first;
            // execute evaluation function
            XEv e = evaluator.evaluate(b.position);
            swarm.setFitness(i, e);
            // check if improves local best
            if (
              (localBest < 0) ||
              (evaluator.betterThan(e, swarm.getFitness(localBest)))) {
               localBest = i;
            }
         }

         // Speed update
         for (unsigned i = 0; i < this->pop_size; i++) {
            Bird& b = swarm.at(i).first;
            for (unsigned j = 0; j < this->cS.size(); j++) {
               b.velocity[j] =
                 (0.25 + this->rg.rand01() / 2) * b.velocity[j] + 1.5 * this->rg.rand01() * (swarm.at(localBest).first.position[j] - b.position[j]) + 1.5 * this->rg.rand01() * (global.first.position[j] - b.position[j]);
            }
         }

         // Global Best Maintenance
         if (evaluator.betterThan(swarm.getFitness(localBest), global.second))
            global = swarm.at(localBest);
         //if (
         //  !star ||
         //  (evaluator.betterThan(swarm.getFitness(localBest), star->second))) {
         //   star = std::make_optional(swarm.at(localBest));
         //}

         count_gen++;
      } //while count_gen

      // This is where magic happens...

      this->best = star;
      return SearchStatus::NO_REPORT;
   }

   // reimplementing searchBy, just to make it more explicit (visible)
   // maybe add some specific logs?
   virtual SearchStatus searchBy(
     std::optional<XSH>& _best,
     std::optional<XSH2>& _inc,
     const StopCriteria<XEv>& stopCriteria) override
   {
      this->best = _best;
      this->incumbent = _inc;
      return search(stopCriteria);
   }

   virtual bool
   setSilentR() override
   {
      this->setSilent();
      // force execution over all components
      return evaluator.setSilentR();
   }

   virtual bool
   setVerboseR() override
   {
      this->setVerbose();
      // force execution over all components
      return evaluator.setVerboseR();
   }
};

} // namespace optframe

#endif /*OPTFRAME_PSO_HPP_*/
