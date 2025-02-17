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

#ifndef OPTFRAME_GENETICALGORITHM_HPP_
#define OPTFRAME_GENETICALGORITHM_HPP_

#include <algorithm>

#include "../../LocalSearch.hpp"
#include "../../MultiSolution.hpp"
#include "../../RandGen.hpp"
#include "../../SingleObjSearch.hpp"
#include "BinarySelection.hpp"
#include "Crossover.hpp"
#include "EA.h"
#include "InitialMultiSolution.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicGeneticAlgorithm : public SingleObjSearch<XES>, public EA {
 protected:
  typedef S Chromossome;
  typedef MultiSolution<S> MSPopulation;

  Evaluator<S>& evaluator;

 private:
  bool maxim;  // true if maximizing
  unsigned popSize;

  double pCross, pMut, pLS;

  unsigned numGenerations;

  InitialMultiSolution<S>& initPop;

  Selection<R, ADS>& selection;
  Crossover<R, ADS>& cross;
  Mutation<R, ADS>& mut;
  LocalSearch<XES, XEv>& ls;

  RandGen& rg;

 public:
  float assert01(float f) {
    if ((f < 0) || (f > 1))
      return 0;
    else
      return f;
  }

  BasicGeneticAlgorithm(Evaluator<S>& _evaluator, InitialMultiSolution<S>& _initPop, unsigned populationSize, float crossoverRate, float mutationRate, float _pLS, unsigned numGenerations, Selection<R, ADS>& _selection, Crossover<R, ADS>& _cross, Mutation<R>& _mut, LocalSearch<XES, XEv>& _ls, RandGen& _rg)
      : evaluator(_evaluator), initPop(_initPop), selection(_selection), cross(_cross), mut(_mut), ls(_ls), rg(_rg) {
    maxim = !evaluator.isMinimization();
    pCross = assert01(crossoverRate);
    pMut = assert01(mutationRate);
    pLS = assert01(_pLS);
    popSize = populationSize;
    if (popSize == 0)
      popSize = 1;
    this->numGenerations = numGenerations;
  }

  void evaluate(const MultiSolution<S>& p, MultiEvaluation<>& mev) {
    for (unsigned i = 0; i < p.size(); i++)
      mev.addEvaluation(&evaluator.evaluate(p.at(i)));
  }

  int getBest(const MultiEvaluation<>& mev) {
    if (mev.size() == 0)
      return -1;
    int best = 0;
    for (unsigned i = 1; i < mev.size(); i++) {
      if (!maxim && (mev.at(i).evaluation() < mev.at(best).evaluation()))
        best = i;
      if (maxim && (mev.at(i).evaluation() > mev.at(best).evaluation()))
        best = i;
    }
    return best;
  }

  // basic implementation of low diversity scheme that prevents populations of clones (TODO: improve!)
  bool lowDiversity(const MultiSolution<S>& p, const MultiEvaluation<>& mev) {
    for (unsigned i = 1; i < mev.size(); i++)
      if (mev.at(i - 1).evaluation() != mev.at(i).evaluation())
        return false;
    return true;
  }

  /*
	 * In the canonical genetic algorithm, fitness (maximization) is defined by: fi/f
	 * where: fi: is the evaluation associated with the i-th Chromossome.
	 *    f: is the average population evaluation.
	 */

  virtual void setFitness(const MultiSolution<S>& p, const MultiEvaluation<>& mev, vector<double>& fv) {
    fv.resize(mev.size());
    for (int i = 0; i < mev.size(); i++)
      fv[i] = mev.at(i).evaluation();

    // convert to maximization
    if (!maxim) {
      double lmax = Selection<R, ADS>::getMax(fv);
      for (int i = 0; i < (int)p.size(); i++)
        fv[i] = lmax - fv[i];
    }

    // calculate average
    double sumEvals = Selection<R, ADS>::getSum(fv);
    double avgEvalsPop = sumEvals / p.size();
    if (avgEvalsPop == 0)
      avgEvalsPop = 1;

    for (int i = 0; i < fv.size(); i++) {
      fv[i] = (fv[i] / avgEvalsPop);
      if (fv[i] != fv[i])  // is nan
      {
        cout << "Selection::setFitness()::NAN VALUE!" << endl;
        cout << "average=" << avgEvalsPop << endl;
        cout << fv << endl;
        exit(1);
      }
    }

    // normalize to [0,1]
    Selection<R, ADS>::normalize(fv);
  }

  void mayMutate(Chromossome& c, Evaluation<>& e) {
    double xMut = rg.rand01();
    if (xMut <= pMut)  // mutate!
      mut.mutate(c, e);
  }

  void mayLocalSearch(Chromossome& c, Evaluation<>& e, double timelimit, double target_f) {
    double xLS = rg.rand01();
    if (xLS <= pLS)  // local search!
      ls.searchFrom(c, e, timelimit, target_f);
  }

  pair<Solution<R, ADS>&, Evaluation<>&>* search(double timelimit = 100000000, double target_f = 0, const Solution<R, ADS>* _s = nullptr, const Evaluation<>* _e = nullptr) {
    Timer t;
    cout << id() << "(timelimit=" << timelimit << "; target_f=" << target_f << ")" << endl;
    cout << "Population Size: " << popSize << " Total of Generations: " << numGenerations << endl;
    cout << "Crossover Rate: " << pCross << " Mutation Rate: " << pMut << " Local Search Rate: " << pLS << endl;

    cout << "Generating the Initial Population" << endl;

    MultiSolution<S>* p = &initPop.generatePopulation(popSize);
    MultiEvaluation<>* mev = new MultiEvaluation;
    evaluate(*p, *mev);
    vector<double> fv;
    setFitness(*p, *mev, fv);
    //cout << fv << " = " << Selection<R, ADS>::getSum(fv) << endl;

    int best = getBest(*mev);

    Chromossome* sStar = new Chromossome(p->at(best));
    Evaluation<>* eStar = new Evaluation(mev->at(best));
    cout << "GA iter=0 ";
    eStar->print();

    unsigned g = 0;

    while ((g < numGenerations) && (evaluator.betterThan(target_f, eStar->evaluation()) && (t.now() < timelimit))) {
      if (lowDiversity(*p, *mev)) {
        //cout << "WARNING: Genetic Algorithm leaving with low diversity at iteration g=" << g << endl;
        //cout << "Try different solution generators or better mutation and crossover operators!" << endl;
        break;
      }

      MultiSolution<S>* p2 = new MultiSolution<S>;
      MultiEvaluation<>* mev2 = new MultiEvaluation;

      best = getBest(*mev);
      p2->push_back(&p->at(best).clone());
      mev2->addEvaluation(&mev->at(best).clone());

      if (evaluator.betterThan(mev->at(best), *eStar)) {
        delete eStar;
        delete sStar;
        eStar = &mev->at(best).clone();
        sStar = &p->at(best).clone();
        cout << "GA iter=" << g << " ";
        eStar->print();
        g = 0;
      }

      while (p2->size() < popSize) {
        pair<unsigned, unsigned> idx = selection.select(*p, *mev, fv);
        if (idx.first == idx.second) {
          cout << "ERROR IN GENETIC SELECTION! SAME ELEMENTS!" << endl;
          exit(1);
        }

        double xCross = rg.rand01();
        if (xCross > pCross)  // do not make any cross
        {
          p2->push_back(p->at(idx.first).clone());
          p2->push_back(p->at(idx.second).clone());
          mev2->addEvaluation(mev->at(idx.first).clone());
          mev2->addEvaluation(mev->at(idx.second).clone());
          continue;
        }

        pair<Chromossome*, Chromossome*> rCross = cross.cross(p->at(idx.first), p->at(idx.second));
        if (!rCross.first && !rCross.second) {
          cout << "ERROR IN GENETIC CROSS! NO RESULT!" << endl;
          exit(1);
        }

        Evaluation<>* e1 = nullptr;
        Evaluation<>* e2 = nullptr;

        if (rCross.first) {
          e1 = &evaluator.evaluate(*rCross.first);
          mayMutate(*rCross.first, *e1);
          mayLocalSearch(*rCross.first, *e1, timelimit, target_f);
          p2->push_back(rCross.first);
          mev2->addEvaluation(e1);
        }

        if (rCross.second) {
          e2 = &evaluator.evaluate(*rCross.second);
          mayMutate(*rCross.second, *e2);
          mayLocalSearch(*rCross.second, *e2, timelimit, target_f);
          p2->push_back(rCross.second);
          mev2->addEvaluation(e2);
        }
      }

      while (p2->size() > popSize) {
        delete &p2->remove(p2->size() - 1);
        mev2->erase(mev2->size() - 1);
      }

      p->clear();
      delete p;
      mev->clear();
      delete mev;

      p = p2;
      mev = mev2;
      setFitness(*p, *mev, fv);

      g++;
    }

    p->clear();
    mev->clear();

    return new pair<Solution<R, ADS>&, Evaluation<>&>(*sStar, *eStar);
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearch<XES>::idComponent() << ":" << EA::family() << ":BasicGeneticAlgorithm";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicGeneticAlgorithmBuilder : public EA, public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  virtual ~BasicGeneticAlgorithmBuilder() {
  }

  virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "") {
    Evaluator<S>* eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    InitialMultiSolution<S>* initPop;
    hf.assign(initPop, *scanner.nextInt(), scanner.next());  // reads backwards!

    int popSize = *scanner.nextInt();
    float pCross = *scanner.nextFloat();
    float pMut = *scanner.nextFloat();
    float pLS = *scanner.nextFloat();
    int nGen = *scanner.nextInt();

    Selection<R, ADS>* sel;
    hf.assign(sel, *scanner.nextInt(), scanner.next());  // reads backwards!

    Crossover<R, ADS>* cross;
    hf.assign(cross, *scanner.nextInt(), scanner.next());  // reads backwards!

    Mutation<R, ADS>* mut;
    hf.assign(mut, *scanner.nextInt(), scanner.next());  // reads backwards!

    string rest = scanner.rest();
    pair<LocalSearch<XES, XEv>*, std::string> method;
    method = hf.createLocalSearch(rest);
    LocalSearch<XES, XEv>* h = method.first;
    scanner = Scanner(method.second);

    return new BasicGeneticAlgorithm<R, ADS>(*eval, *initPop, popSize, pCross, pMut, pLS, nGen, *sel, *cross, *mut, *h, hf.getRandGen());
  }

  virtual vector<pair<string, string>> parameters() {
    vector<pair<string, string>> params;
    params.push_back(make_pair(Evaluator<S>::idComponent(), "evaluation function"));
    params.push_back(make_pair(InitialMultiSolution<S>::idComponent(), "generator for initial population"));
    params.push_back(make_pair("OptFrame:int", "population size"));
    params.push_back(make_pair("OptFrame:float", "cross probability"));
    params.push_back(make_pair("OptFrame:float", "mutation probability"));
    params.push_back(make_pair("OptFrame:float", "local search probability"));
    params.push_back(make_pair("OptFrame:int", "number of generations without improvement"));
    params.push_back(make_pair(Selection<R, ADS>::idComponent(), "selection"));
    params.push_back(make_pair(Crossover<R, ADS>::idComponent(), "crossover"));
    params.push_back(make_pair(Mutation<R, ADS>::idComponent(), "mutation"));
    params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));

    return params;
  }

  virtual bool canBuild(string component) {
    return component == BasicGeneticAlgorithmBuilder<R, ADS>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << EA::family() << ":BasicGeneticAlgorithm";
    return ss.str();
  }

  virtual string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_GENETICALGORITHM_HPP_*/
