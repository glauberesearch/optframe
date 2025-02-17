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

#ifndef OPTFRAME_EVALUATION_HPP_
#define OPTFRAME_EVALUATION_HPP_

// C
#include <assert.h>
// C++
#include <cmath>
#include <cstdlib>
#include <iostream>
//
#include <OptFrame/BaseConcepts.hpp>
#include <OptFrame/Component.hpp>
#include <OptFrame/Helper/MultiObjValue.hpp>  // inserting this beforehand.. who knows!!!
#include <OptFrame/SingleObjValue.hpp>        // basic value 'evtype' comes from here!

// using namespace std;

namespace optframe {

//! \english The Evaluation class is a container class for the objective function value and the Memory structure M. \endenglish \portuguese A classe Evaluation é uma classe contêiner para o valor da função objetivo e a estrutura de Memória M. \endportuguese

/*!
 \english
 It is also possible to carry an infeasibility measure.
 The evaluation() method returns the sum of objFunction and infMeasure.
 \endenglish

 \portuguese
 Também é possível carregar uma medida de inviabilidade infMeasure.
 O método evaluation() retorna a soma da função objetivo objFunction e a infMeasure.
 \endportuguese
 */

//#include "Util/PackTypes.hpp"
//#define EVALUATION_TYPE PackTypes

// note: for multi-objective problems with distinct objective space types
// such as (int, evtype, long long) you can use PackTypes in Utils or overload
// manually each of the numeric operators +, -, *
//why this isn't a template????????????????
// found a solution using C++20 concepts: now it's an optional template :)

// TODO: this ObjType here can require more than 'totally_ordered',
// as it will require arithmetics (+,-,*) to calculate MoveCosts and Infeasibility
// Thus, we should use another concept here, composed (&&) with arithmetics.
// Good thing is that Evaluation class is generic for most uses...
// This is not the case for Pareto solutions, where composability may become more complex (MultiEvaluation)
// TODO: must see next how to deal with that (on Pareto side), but at least we have a very elegant solution now

// here comes the tricky part, 'totally_ordered' should be enough, but we endup needing arithmetics to compute costs
//template<optframe::totally_ordered ObjType = evtype>
// so we will proceed with basic arithmetics, +, - and *.
// this effectively discard 'string' and others (although comparable)

// THIS IS FOCUSED SINGLE DIRECTION PERSPECTIVE (TOTAL ORDER).
// SEE PARAM 'isMini'
template <optframe::basic_arithmetics ObjType = evtype>
class Evaluation final : public Component {
 public:
  // exporting 'objType' type, based on template 'ObjType'
  using objType = ObjType;
  //
 protected:
  // static Evaluation<ObjType> costZero { Evaluation<ObjType>() };
  ObjType objValZero;  //{ optframe::get_numeric_zero<ObjType>() };

  // ==== Objective Space type: pair<evtype, evtype> ====
  // objective function value (default = double)
  ObjType objFunction;
  // infeasibility measure value (default = double)
  ObjType infMeasure;
  // for lexicographic approaches, use these extra evaluation values
  // vector<pair<ObjType, ObjType>> alternatives;

 protected:
  // boolean field to indicate if Evaluation needs an update
  bool outdated{true};
  // boolean field to indicate if Evaluation value is an estimation (not exact)
  bool estimated{false};

 public:
  // is minimization (WHY???)
  bool isMini{true};

  static constexpr int x{num_zero<int>()};

  // ======================================
  // begin canonical part

  // TODO(IGOR): maybe create empty Evaluation with numeric_zero on 'obj'

  explicit Evaluation(const ObjType& obj,
                      const ObjType& inf,
                      const evtype& w = 1,
                      bool _isMini = true)
      : objFunction(obj), infMeasure(inf), isMini(_isMini) {
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);

    optframe::numeric_zero(objValZero);

    // gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  // TODO(IGOR): I am removing 'explicit' to allow StopCriteria "seamless"
  // passing of Evaluation object.
  Evaluation(const ObjType& obj)
      : objFunction(obj) {
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);

    optframe::numeric_zero(objValZero);

    // infMeasure = optframe::get_numeric_zero<ObjType>();
    optframe::numeric_zero(infMeasure);

    // gos = gos_unknown;
    outdated = false;
    estimated = false;
  }

  explicit Evaluation() {
    // verify that this is valid XEvaluation
    // this SHOULD work... don't know why it's not!
    // static_assert(XEvaluation<decltype(*this)>);
    static_assert(XEvaluation<Evaluation<ObjType>>);

    optframe::numeric_zero(objValZero);
    optframe::numeric_zero(objFunction);
    optframe::numeric_zero(infMeasure);

    // gos = gos_unknown;
    // outdated = false;
    outdated = true;
    estimated = false;
  }

  // never put 'explicit' here!!
  Evaluation(const Evaluation<ObjType>& e)
      : objFunction(e.objFunction), infMeasure(e.infMeasure)
        //, alternatives(e.alternatives)
        //, gos(e.gos)
        ,
        outdated(e.outdated),
        estimated(e.estimated),
        isMini(e.isMini) {
    // verify that this is valid XEvaluation
    static_assert(XEvaluation<Evaluation<ObjType>>);

    optframe::numeric_zero(objValZero);
  }

  virtual ~Evaluation() {
  }

  virtual Evaluation<ObjType>& operator=(const Evaluation<ObjType>& e) {
    if (&e == this)  // auto ref check
      return *this;

    objFunction = e.objFunction;
    infMeasure = e.infMeasure;
    outdated = e.outdated;
    estimated = e.estimated;
    // alternatives = e.alternatives;
    // gos = e.gos;
    isMini = e.isMini;

    return *this;
  }

  virtual Evaluation<ObjType>& operator=(const Evaluation<ObjType>&& e) {
    if (&e == this)  // auto ref check
      return *this;

    // steal from 'e'
    objFunction = e.objFunction;
    infMeasure = e.infMeasure;
    outdated = e.outdated;
    estimated = e.estimated;
    // alternatives = e.alternatives;
    // gos = e.gos;
    isMini = e.isMini;

    return *this;
  }

  virtual Evaluation<ObjType>& clone() const {
    return *new Evaluation(*this);
  }

  // end canonical part
  // ======================================
  // begin Evaluation methods

  // getters/setters
  bool isOutdated() const {
    return outdated;
  }

  void invalidate() {
    outdated = true;
  }

  bool isEstimated() const {
    return estimated;
  }

  static constexpr ObjType getZero() {
    ObjType objValZero;
    optframe::numeric_zero(objValZero);
    return objValZero;
  }

  ObjType getObjFunction() const {
    return objFunction;
  }

  ObjType getInfMeasure() const {
    return infMeasure;
  }

  void setObjFunction(ObjType obj) {
    objFunction = obj;
    // I THINK this is right... but let's warn, for now!
    // Otherwise, where else could we set outdated to false??
    if (Component::warning)
      std::cout << "WARNING: setObjFunction sets outdated=false" << std::endl;

    outdated = false;
  }

  void setInfMeasure(ObjType inf) {
    infMeasure = inf;
  }

  ObjType evaluation() const {
    return objFunction + infMeasure;
  }

  // ========= TAKEN FROM MoveCost =======

  // update target Evaluation with *this cost
  virtual void update(Evaluation<ObjType>& evTarget) const {
    // this task was performed before by MoveCost... now unifying in Evaluation
    // how to do this?
    assert(false);
  }

  // returns the difference/cost between evFinal - evThis
  virtual Evaluation<ObjType> diff(const Evaluation<ObjType>& evFinal) const {
    const Evaluation<ObjType>& e = evFinal;  // pointing to legacy code

    // take my own information
    // -----------------------

    pair<ObjType, ObjType> e_begin = make_pair(
        this->getObjFunction(),
        this->getInfMeasure());

    // -----------------------
    // compute cost difference
    // -----------------------
    Evaluation<ObjType> mcost(
        e.getObjFunction() - e_begin.first,
        e.getInfMeasure() - e_begin.second,
        1);  // no outdated or estimated

    // ======================================================
    // For alternative/lexicographic costs, see WLxEvaluation
    // ======================================================

    return Evaluation<ObjType>(mcost);
  }

  virtual bool equals(const Evaluation<ObjType>& e) {
    return evaluation() == e.evaluation();
  }

  // =========
  // finish MoveCost
  // =========

  // leave option to rewrite tolerance (or consider lexicographic values)
  virtual bool isFeasible() const {
    // return optframe::numeric_is_zero<ObjType>(infMeasure);
    // IMPORTANT: numeric_is_zero can come from anywhere!
    return optframe::numeric_is_zero(infMeasure);
    // return (EVALUATION_ABS(infMeasure) <= optframe::get_numeric_zero<ObjType>()); // deprecated
  }

  // ======================
  // Object specific part
  // ======================

  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":Evaluation";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }

  void print() const override {
    std::cout << toString() << endl;
  }

  std::string toString() const override {
    // ONE SHOULD NEVER PRINT AN EVALUATION WITH OUTDATED FLAG... SANITY CHECK!
    assert(!outdated);
    std::stringstream ss;
    ss << fixed;  // disable scientific notation
    ss << "Evaluation function value = " << evaluation();
    ss << (isFeasible() ? " " : " (not feasible) ");
    ss << (outdated ? " OUTDATED " : " ");
    ss << (estimated ? " ESTIMATED " : " ");
    return ss.str();
  }

  bool toStream(std::ostream& os) const override {
    // forward to operator<<
    os << (*this);
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const Evaluation& me) {
    if (&os == &optframe::cjson) {
      os << "{";
      os << "\"evaluation\": " << me.evaluation() << ",";
      os << "\"feasible\": " << (me.isFeasible() ? "true" : "false") << ",";
      os << "\"outdated\": " << (me.outdated ? "true" : "false") << ",";
      os << "\"estimated\": " << (me.estimated ? "true" : "false") << "}";
    } else {
      os << me.toString();
    }
    return os;
  }
};

// compilation test (for concepts)
// debugging error on XEvaluation for IsEvaluation<int>
static_assert(optframe::evgoal<Evaluation<>>);
static_assert(HasClone<Evaluation<>>);
static_assert(HasToString<Evaluation<>>);
static_assert(HasGetObj<Evaluation<>>);
static_assert(optframe::ostreamable<Evaluation<>>);
//
static_assert(XEvaluation<Evaluation<>>);  // check default

// ==================== end Evaluation ===========

}  // namespace optframe

struct basic_ev_test_copy {
  void f() {
    optframe::Evaluation<> e1;
    optframe::Evaluation<> e2 = e1;  // test 'explicit'
  }
};

// Compilation tests for XEvaluation concepts
// These are NOT unit tests... Unit Tests are on tests/ folder
#include "Evaluation.test.hpp"

#endif  // OPTFRAME_EVALUATION_HPP_
