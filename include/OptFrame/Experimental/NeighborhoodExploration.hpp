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

#ifndef OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_
#define OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_

// C++
#include <iostream>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/LocalSearch.hpp>
#include <OptFrame/SingleObjSearch.hpp>

// using namespace std;

namespace optframe {

// This is  NEx: Neighborhood Exploration

template <XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>
//using MoveWithCost = pair< uptr< Move<XES, XEv> >, XEv >;
struct RichMove {
  uptr<Move<XES, XEv, XSH>> move;
  XEv cost;
  SearchStatus status;

  RichMove() {
  }

  RichMove(RichMove&& rmove)
      : move(std::move(rmove.move)) {
  }

  RichMove& operator=(RichMove&& rmove) {
    // self-reference
    if (&rmove == this)
      return *this;
    move = std::move(rmove.move);
    cost = std::move(rmove.cost);
    status = rmove.status;
    return *this;
  }
};

template <XESolution XES, XEvaluation XEv = Evaluation<>, XSearch<XES> XSH = XES>  // defaults to XSH = XES
class NeighborhoodExploration : public LocalSearch<XES, XEv, XSH>                  //: public Component
{
 public:
  NeighborhoodExploration() {
  }

  virtual ~NeighborhoodExploration() {
  }

  // implementation of a "default" local search for this NEx
  virtual SearchStatus searchFrom(XES& se, const StopCriteria<XEv>& stopCriteria) {
    bool improved = false;
    // searching new move
    op<RichMove<XES, XEv>> movec = searchMove(se, stopCriteria);
    // check if move exists
    if (!movec)
      return SearchStatus::NO_REPORT;
    //
    // accept if it's improving by flag (avoid double verification and extra Evaluator class here)
    // using !!e idiom, to check against value 0
    while (!!(movec->status & SearchStatus::IMPROVEMENT)) {
      improved = true;
      // apply move to solution
      movec->move->apply(se);
      // update cost
      movec->cost.update(se.second);
      // searching new move
      movec = searchMove(se, stopCriteria);
      // check if move exists
      if (!movec)
        return SearchStatus::IMPROVEMENT;
    }
    // finished search
    return improved ? SearchStatus::IMPROVEMENT : SearchStatus::NO_REPORT;
  }

  // Output move may be nullptr. Otherwise it's a pair of Move and its Cost.
  virtual op<RichMove<XES, XEv>> searchMove(const XES& se, const StopCriteria<XEv>& stopCriteria) = 0;

  virtual bool compatible(string s) {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << ":NEx";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = XES>
class NeighborhoodExplorationBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~NeighborhoodExplorationBuilder() {
  }

  virtual NeighborhoodExploration<XES, XEv, XSH>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") = 0;

  virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") {
    return build(scanner, hf, family);
  }

  virtual vector<pair<string, string>> parameters() = 0;

  virtual bool canBuild(string) = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent() << "NEx";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#include <OptFrame/BaseConcepts.ctest.hpp>
#include <OptFrame/BaseConcepts.hpp>

namespace optframe {

struct TestSimpleRich {
  uptr<int> ptr;

  TestSimpleRich() {
  }

  TestSimpleRich(TestSimpleRich&& tsr)
      : ptr(std::move(tsr.ptr)) {
  }
};

using TestRichMove = RichMove<IsESolution<int>, IsEvaluation<int>>;

//static_assert(std::is_destructible<TestRichMove>);

struct TestCompilationRichMove {
  /*
   op<RichMove<IsESolution<int>, IsEvaluation<int>>> getRichOptional()
   {
      op<RichMove<IsESolution<int>, IsEvaluation<int>>> empty;
      //
      RichMove<IsESolution<int>, IsEvaluation<int>> rmove;
      //return make_optional(rmove);
      return op<RichMove<IsESolution<int>, IsEvaluation<int>>>(std::move(rmove));
   }
   */

  op<TestSimpleRich> getRichOptional() {
    op<TestSimpleRich> empty;
    //
    TestSimpleRich simple;
    return op<TestSimpleRich>(std::move(simple));
  }

  void test() {
    auto rmv = getRichOptional();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_NEIGHBORHOOD_EXPLORATION_HPP_ */
