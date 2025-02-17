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

#ifndef OPTFRAME_INTERACTIVE_HPP_
#define OPTFRAME_INTERACTIVE_HPP_

/*! \mainpage OptFrame
 * This is the OptFrame documentation.
 */
//Esta é a documentação do OptFrame.
//
// C
#include <stdlib.h>
#include <unistd.h>
// C++
#include <iostream>
#include <map>
#include <ostream>
#include <vector>
//
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/ADSManager.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/InitialPopulation.hpp>
#include <OptFrame/Move.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Timer.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

// ==================================
//       Neighborhood Structure
// ==================================

#include <OptFrame/NS.hpp>
#include <OptFrame/NSEnum.hpp>
#include <OptFrame/NSSeq.hpp>

// ==================================
//        Parallel Support
// ==================================

#include <OptFrame/Parallel/Parallel.h>

// ==================================
//            Heuristic
// ==================================

#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/MultiObjSearch.hpp>
#include <OptFrame/SingleObjSearch.hpp>

// base components
#include <OptFrame/Constructive.hpp>
#include <OptFrame/Helper/CloneConstructive.hpp>

// local search
#include <OptFrame/Heuristics/Empty.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/CircularSearch.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/HillClimbing.hpp>
#include <OptFrame/Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp>
#include <OptFrame/Heuristics/SingleObjSearchToLocalSearch.hpp>

// single obj search
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/BasicILSPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/BasicIteratedLocalSearch.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp>
#include <OptFrame/Heuristics/SimpleLocalSearch.hpp>
#include <OptFrame/Heuristics/VNS/BasicVNS.hpp>
#include <OptFrame/Heuristics/VNS/GeneralVNS.hpp>
#include <OptFrame/Heuristics/VNS/ReducedVNS.hpp>
#include <OptFrame/Heuristics/VNS/VariableNeighborhoodSearch.hpp>

//TODO ERROR on DecoderNSGAII
//#include <OptFrame/Heuristics/EvolutionaryAlgorithms/DecoderNSGAII.hpp"

#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicDecoderRandomKeys.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicInitialEPopulationRK.hpp>

// test local searches
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Heuristics/CompareLocalSearch.hpp>
#include <OptFrame/RandGen.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>

// using namespace std;

namespace optframe {

//template<XESolution XES, XEvaluation XEv = Evaluation<>>
// template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//
//
#ifdef OPTFRAME_LEGACY_R_ADS
template <XRepresentation R, class ADS, XBaseSolution<R, ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = XES, XESolution XES2 = XES, X2ESolution<XES> X2ES = MultiESolution<XES>>
#else
template <XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES, X2ESolution<XES2> X2ES = MultiESolution<XES2>>
#endif
class Loader {

#ifdef OPTFRAME_LEGACY_R_ADS
//
#else
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
#endif

 public:
  HeuristicFactory<S, XEv, XES, X2ES> factory;
  map<string, string> dictionary;
  map<string, vector<string>> ldictionary;

  Loader() {
    loadComponentBuilders();
  }

  Loader(RandGen _rg)
      : factory(HeuristicFactory<S, XEv, XES, X2ES>(_rg)) {
    loadComponentBuilders();
  }

  void loadComponentBuilders() {
    // Independent components
    factory.builders.push_back(new RandGenBuilder<S, XEv>);
    factory.builders.push_back(new TimerBuilder<S, XEv>);

    // Base
    factory.builders.push_back(new CloneConstructiveBuilder<S, XEv>);

    // LocalSearch
    factory.builders.push_back(new EmptyLocalSearchBuilder<S, XEv>);
    factory.builders.push_back(new BestImprovementBuilder<S, XEv>);
    factory.builders.push_back(new FirstImprovementBuilder<S, XEv>);
    factory.builders.push_back(new RandomDescentMethodBuilder<S, XEv>);
    factory.builders.push_back(new CircularSearchBuilder<S, XEv>);
    factory.builders.push_back(new VariableNeighborhoodDescentBuilder<S, XEv>);
#ifdef OPTFRAME_LEGACY_R_ADS
    factory.builders.push_back(new VariableNeighborhoodDescentUpdateADSBuilder<R, ADS, S, XEv>);
#endif
    //factory.builders.push_back(new RVNDBuilder<S, XEv> );
    factory.builders.push_back(new HillClimbingBuilder<S, XEv>);
    factory.builders.push_back(new LateAcceptanceHillClimbingBuilder<S, XEv>);
    factory.builders.push_back(new SingleObjSearchToLocalSearchBuilder<S, XEv>);

    // SingleObjSearch + Parameters
    factory.builders.push_back(new SimpleLocalSearchBuilder<S, XEv>);
    factory.builders.push_back(new BasicSimulatedAnnealingBuilder<XSH, XES2, X2ES>);
    factory.builders.push_back(new BasicIteratedLocalSearchBuilder<S, XEv>);
    factory.builders.push_back(new BasicILSPerturbationBuilder<S, XEv>);
    factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<S, XEv>);
    factory.builders.push_back(new ILSLPerturbationLPlus2Builder<S, XEv>);
    factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<S, XEv>);
    factory.builders.push_back(new BasicGRASPBuilder<S, XEv>);
    factory.builders.push_back(new BasicVNSBuilder<S, XEv>);
    factory.builders.push_back(new ReducedVNSBuilder<S, XEv>);
    factory.builders.push_back(new GeneralVNSBuilder<S, XEv>);

    // RK family (random keys)
    //static_assert(X2ESolution<XES, MultiESolution<XES>>);
    //factory.builders.push_back(new BRKGABuilder<XES>);

    // GlobalSearchBuilder
    //using XES2_BRKGA = std::pair<std::vector<double>, Evaluation<>>;
    //using XSH2_BRKGA = VEPopulation<XES2_BRKGA>;
    //
    // Ignoring other BRKGA type parameters...
    // In the future, perhaps try to make other specific lists,
    // such as ITrajectory or IPopulational (if users want to have specific features,
    // like onIncumbent(...) callback.
    // For Multi Objective, must see benefits.
    factory.builders.push_back(new BRKGABuilder<XES, XES, X2ES>);
    factory.builders.push_back(new BasicInitialEPopulationRKBuilder<S, XEv>);
    factory.builders.push_back(new BasicDecoderRandomKeysBuilder<S, XEv>);

    // test local searches
    factory.builders.push_back(new CompareLocalSearchBuilder<S, XEv>);
  }
};

}  // namespace optframe

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
