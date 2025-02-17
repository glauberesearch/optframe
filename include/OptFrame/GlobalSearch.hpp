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

#ifndef OPTFRAME_GLOBAL_SEARCH_HPP_
#define OPTFRAME_GLOBAL_SEARCH_HPP_

// C++
#include <cstring>
#include <iostream>
#include <vector>
//
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/VEPopulation.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/InitialSearch.hpp>
#include <OptFrame/SearchOutput.hpp>
#include <OptFrame/StopCriteria.hpp>

// using namespace std;

namespace optframe {

// Defaulting SearchSpace to XES, it means, <S,XEv> space (typically, single obj search)
//template<XESolution XES, XSearch<XES> XSH = XES, XSearchMethod XM = Component>
//
//template<XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>
//
// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
//
// We need to make all methods compatible, and too many templates is polluting this...
// 1) Explicitly renaming XSH to XBest
// 2) Keeping XSH2 as XIncumbent on specific searchBy methods (Trajectory? Populational?)
template <XESolution XES, XSearch<XES> XSH = XES>  // XESolution XES2, XSearch<XES> XSH2 = XSH>
class GlobalSearch : public Component {
  using XEv = typename XES::second_type;

 public:
  using BestType = XSH;

 public:
  // best known XSH object: solution/pareto/etc
  //std::optional<XSH> best;
  // current/working XSH2 object: population/etc
  //std::optional<XSH2> incumbent;
  // ----------
  //
  // callback action 'onBest' is triggered after best is updated (if 'beforeUpdateBest' is required some day, think about it.. not now)
  // returning 'false' should lead to aborting execution (by target, for instance)
  //
  bool (*onBest)(GlobalSearch<XES, BestType>& self, const BestType& best) =
      [](GlobalSearch<XES, BestType>& self, const BestType& best) { return true; };
  //
  // onIncumbent now will depend on Incumbent type (look on ITrajectory or IPopulational)
  //
  // bool (*onIncumbent)(GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2& incumbent) =
  //  [](GlobalSearch<XES, XSH, XES2, XSH2>& self, const XSH2& incumbent) { return true; };
  //
  // strict or non-strict search
  bool strict{true};

  GlobalSearch() {
  }

  virtual ~GlobalSearch() {
  }

  // Assuming method is not thread-safe.
  // Now, we can easily use flag SearchStatus::RUNNING.
  virtual SearchOutput<XES, BestType> search(
      const StopCriteria<XEv>& stopCriteria) = 0;

  /*
   virtual SearchStatus searchBy(std::optional<XSH>& _best, std::optional<XSH2>& _inc, const StopCriteria<XEv>& stopCriteria)
   {
      best = _best;
      incumbent = _inc;
      return search(stopCriteria);
   }
*/

  virtual std::string log() const {
    return "Empty heuristic log.";
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":GlobalSearch";
    return ss.str();
  }

  std::string toString() const override {
    return id();
  }

  std::string id() const override {
    return idComponent();
  }
};

// 'XES' is the "base concept" for the primary search component.
// 'XSH' is the primary search type ('best' has type XSH)
// 'XES2' is the "base concept" for the secondary search component.
// 'XSH2' is the secondary search type ('incumbent' has type XSH2)
template <XESolution XES, XSearch<XES> XSH = XES, XESolution XES2 = XES, XSearch<XES> XSH2 = XSH>
class Populational : public GlobalSearch<XES, XSH> {
  using XEv = typename XES::second_type;
  using BestType = XSH;

 public:
  using IncumbentType = optframe::VEPopulation<XES2>;  // this uses EPopulation, not "legacy Population"

 public:
  // onIncumbent now will depend on Incumbent type (look on ITrajectory or IPopulational)
  //
  bool (*onIncumbent)(GlobalSearch<XES, XSH>& self, const IncumbentType& incumbent) =
      [](GlobalSearch<XES, XSH>& self, const IncumbentType& incumbent) { return true; };
  //

  // global search method (maybe, someday, create some abstract IGlobalSearch.. not now)
  //virtual SearchStatus search(const StopCriteria<XEv>& stopCriteria) = 0;
  //
  virtual SearchOutput<XES, BestType> search(const StopCriteria<XEv>& stopCriteria) override = 0;

  //
  // virtual method with search signature for populational methods
  //
  virtual SearchOutput<XES, BestType> searchPopulational(
      std::optional<BestType>& _best,
      IncumbentType& _inc,
      const StopCriteria<XEv>& stopCriteria) = 0;

  virtual bool compatible(std::string s) override {
    return (s == idComponent()) || (GlobalSearch<XES, XSH>::compatible(s));
  }

  static std::string idComponent() {
    std::stringstream ss;
    ss << GlobalSearch<XES, XSH>::idComponent() << ":Populational:";
    return ss.str();
  }

  virtual std::string id() const override {
    return idComponent();
  }

  virtual std::string toString() const override {
    return id();
  }
};

template <XESolution XES, XSearch<XES> XSH, XESolution XES2, X2ESolution<XES2> X2ES = MultiESolution<XES2>>  //, XESolution XES2, XSearch<XES> XSH2 = XSH>
class GlobalSearchBuilder : public ComponentBuilder<typename XES::first_type, typename XES::second_type, XSH> {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;

 public:
  virtual ~GlobalSearchBuilder() {
  }

  virtual GlobalSearch<XES, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

  virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") {
    return build(scanner, hf, family);
  }

  virtual vector<pair<string, string>> parameters() = 0;

  virtual bool canBuild(string) = 0;

  virtual std::string toString() const override {
    return id();
  }

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XSH>::idComponent() << "GlobalSearch:";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_GLOBAL_SEARCH_HPP_ */
