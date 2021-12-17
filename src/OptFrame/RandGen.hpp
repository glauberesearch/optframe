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

#ifndef OPTFRAME_RANDGEN_HPP_
#define OPTFRAME_RANDGEN_HPP_

// C includes
#include <math.h>
#include <time.h>
#include <unistd.h>

// C++ includes
#include <array>
#include <cmath>
#include <vector>
//#include <tgmath.h>
#include <random> //used for Binomial distribution requested by NGES
//#include <tr1/random>

//#include "Action.hpp"
#include "Component.hpp"
#include "ComponentBuilder.h"

namespace optframe {

// =================
// RandGen interface
// =================

// -------------
// uniform rands
// -------------
// rand() -> int32
// rand(uint32 limit) -> [0, limit-1]  (note that rand(0) is zero, by default, no exceptions thrown)
// rand(uint32 a, uint32 b) -> [a, b]
// rand01() -> double [0.0,1.0)
// randP(double p) -> bool (pass or not pass, probability p in [0,1])

// --------------
//    gaussian
// --------------
// randG() -> double (gaussian mean 0.0 stddev 1.0)
// randG(double mean, double stdev) -> double

// --------------
//   binomial
// --------------
// randB(double base, int tries) -> double (non-negative)
// randBN(double base, int tries) -> double (including negative values)

class RandGen : public Component
{
protected:
   // usual seeds has this standard (uint32)
   unsigned seed;

private:
   // using c++11 mersenne twister, by default
   std::mt19937 _gen;
   // not using c++ default
   //std::default_random_engine _gen;

public:
   // returns the random generator seed
   long getSeed() const
   {
      return seed;
   }

   // sets the random generator seed (uint32)
   RandGen& setSeed(unsigned _seed)
   {
      seed = _seed;
      initialize();
      // returning self-reference
      return *this;
   }

public:
   RandGen()
   {
      seed = generateRandomSeed();
      mt_initialize();
   }

   explicit RandGen(unsigned _seed)
     : seed(_seed)
   {
      mt_initialize();
   }

   virtual ~RandGen()
   {
   }

private:
   void mt_initialize()
   {
      unsigned _seed = this->seed;
      //std::array<decltype(this->_seed), this->_gen.state_size> seed_data;
      std::array<decltype(_seed), std::mt19937::state_size> seed_data;
      std::generate(seed_data.begin(), seed_data.end(), [&_seed]() { return _seed++; });
      std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());
      _gen.seed(seed_sequence);
   }

public:
   // initialize random number generation
   virtual void initialize()
   {
      mt_initialize();
   }

   /*
@Anderson
https://stats.stackexchange.com/questions/436733/is-there-such-a-thing-as-a-good-bad-seed-in-pseudo-random-number-generation
Check Mars answer in StackExchange. He made a short summary of why we should adequately initialize the Mersenne Twister algorithm while linking several studies on the matter.
He explains: "[...] one of the issues with Mersenne Twisters is that if its internal 624x32-bit state has many zero bits, it takes many iterations to get out of that pattern."
When you look into the implementation of std::, one can see that the constructor generates at least one random number for each seed given. The static variable (engine size) from std::mt19937 gives the degree of recurrence, in this case, 624. By providing that many seeds to the engine, the quality of the following iterations is significantly higher and, in most cases, independent of the seed size.
*/

public:
   // random integer
   virtual int rand()
   {
      std::uniform_int_distribution<int> dis;
      return dis(_gen);
   }

   // random positive integer between 0 and n-1
   virtual unsigned rand(unsigned n)
   {
      // protect against underflow
      n = (n == 0) ? 0 : n - 1; // no exceptions thrown
      std::uniform_int_distribution<unsigned> dis(0, n);
      return dis(_gen);

      // TODO(igormcoelho): https://ericlippert.com/2013/12/16/how-much-bias-is-introduced-by-the-remainder-technique/
      //return ::rand() % n; // old way, abandoned (too much bias!)
   }

   // randomized number between non-negatives [i, j]
   virtual unsigned rand(unsigned i, unsigned j)
   {
      std::uniform_int_distribution<unsigned> dis(i, j);
      return dis(_gen);
   }

   // random with probability 'p'
   virtual bool randP(double p)
   {
      std::uniform_real_distribution<double> distribution(0.0, 1.0);
      return rand01() < p;
   }

   // random uniform between [0,1)
   virtual double rand01()
   {
      std::uniform_real_distribution<double> dis(0.0, 1.0);
      return dis(_gen);
   }

   // rand binomial
   virtual int randB(double p, int tries)
   {
      std::binomial_distribution<int> dis(tries, p);
      return dis(_gen);
   }

   // rand binomial (positive or negative)
   virtual int randBN(double p, int tries)
   {
      int y = randB(p, tries);
      int sign = this->rand(2);
      if (sign == 1)
         y *= -1;
      return y;
   }

   virtual double randG(double mean, double stdev)
   {
      std::normal_distribution<double> dis(mean, stdev);
      return dis(_gen);
   }

   // random gaussian mean 0.0 stdev 1.0
   virtual double randG()
   {
      std::normal_distribution<double> dis(0.0, 1.0);
      return dis(_gen);
   }

   unsigned generateRandomSeed() const
   {
      // using random_device just for default seeding (if not provided otherwise)
      std::random_device rd;
      std::uniform_int_distribution<unsigned> dist;
      return dist(rd);
   }

   // deterministic shuffle using RandGen object
   template<class T>
   void shuffle(vector<T>& v)
   {
      if (v.size() > 0)
         for (unsigned int i = 0; i < v.size() - 1; i++) {
            int x = i + this->rand(v.size() - i - 1) + 1;
            //v.swap(v.begin()+i, v.begin()+x);
            T elem = v.at(i);
            v.at(i) = v.at(x);
            v.at(x) = elem;
         }
   }

   static string idComponent()
   {
      return "OptFrame:RandGen";
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual std::string toString() const override
   {
      return id();
   }

   virtual bool compatible(string s)
   {
      return (s == idComponent()) || (Component::compatible(s));
   }
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class RandGenBuilder : public ComponentBuilder<S, XEv, XES, X2ES>
{
public:
   virtual ~RandGenBuilder()
   {
   }

   virtual Component*
   buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
   {
      if (!scanner.hasNext())
         return nullptr;

      long seed = *scanner.nextLong();

      return new RandGen(seed);
   }

   virtual vector<pair<string, string>> parameters()
   {
      vector<pair<string, string>> params;
      params.push_back(make_pair("long", "seed"));
      return params;
   }

   virtual bool canBuild(string component)
   {
      return component == RandGen::idComponent();
   }

   static string idComponent()
   {
      stringstream ss;
      ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "RandGen";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};
}

#endif /* OPTFRAME_RANDGEN_HPP_ */
