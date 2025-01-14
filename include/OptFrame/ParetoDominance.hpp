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

/*
 * ParetoDominance.hpp
 *
 * LGPLv3
 *
 */

#ifndef OPTFRAME_PARETODOMINANCE_HPP_
#define OPTFRAME_PARETODOMINANCE_HPP_

// C++
#include <cmath>
#include <iostream>
#include <utility>
//
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Evaluator.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/MultiEvaluator.hpp>

// using namespace std;

namespace optframe {

template <XESolution XMES>
class ParetoDominance {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;
  using XES = std::pair<S, XEv>;

 public:
  //
  // TODO: why we need v_d???
  // vsref<Direction<XEv>> v_d;
  //
  sref<MultiEvaluator<XMES>> mev;

 public:
  explicit ParetoDominance(sref<MultiEvaluator<XMES>> _mev)
      : mev(_mev) {
  }

  explicit ParetoDominance(vsref<Evaluator<S, XEv, XES>> _v_e)
      : mev{new MultiEvaluator<XMES>{_v_e}} {
  }

  virtual ~ParetoDominance() {
  }

  // void insertEvaluators(vector<Evaluator<XES, XEv>*> _v_e)
  // {
  //    mev.addEvaluator(_v_e);
  // }

  // vector<Evaluator<XES, XEv>*> getEvaluators()
  // {
  //    return v_e;
  // }

  sref<MultiEvaluator<XMES>> getMultiEvaluator() {
    return mev;
  }

  /*
   // true if 's1' dominates 's2'
	virtual bool dominates(const S& s1, const S& s2)
	{
		if (mev.size() == 0)
		{
			cout << "ParetoDominance::error! not implemented for direction only!" << endl;
			exit(1);
		}

		MultiEvaluation<> mev1(std::move(mev.evaluate(s1)));
		MultiEvaluation<> mev2(std::move(mev.evaluate(s2)));


		bool r = dominates(mev1, mev2);

		delete &mev1;
		delete &mev2;


		return r;
	}
*/

  // true if 's1' dominates 's2'
  virtual bool dominates(const XMEv& mev1,
                         const XMEv& mev2) {
    pair<int, int> betterEquals = checkDominates(mev1, mev2);
    int better = betterEquals.first;
    int equals = betterEquals.second;

    return ((better + equals == (int)mev1.size()) && (better > 0));
  }

  // return a pair of better and equals
  pair<int, int> checkDominates(const XMEv& mev1,
                                const XMEv& mev2) {
    if ((mev1.size() != mev2.size()) ||
        (mev1.size() == 0) || (mev2.size() == 0)) {
      // if (Component::warning)
      cout << "WARNING in ParetoDominance: different sizes or empty!" << endl;
      return make_pair(-1, -1);
    }

    int better = 0;
    int equals = 0;

    for (int eIndex = 0; eIndex < (int)mev1.size(); eIndex++) {
      if (mev->betterThanAt(mev1[eIndex], mev2[eIndex], eIndex))
        better++;

      if (mev->equalsAt(mev1[eIndex], mev2[eIndex], eIndex))
        equals++;

      // if (abs(mev1[e].evaluation() - mev2[e].evaluation()) < 0.0001)
      //
    }

    return make_pair(better, equals);
  }

  // returns pair: (true, if 's1' dominates 's2'; true, if 's2' dominates 's1')
  virtual pair<bool, bool> birelation(const XMEv& v1,
                                      const XMEv& v2) {
    bool b1 = dominates(v1, v2);
    bool b2 = dominates(v2, v1);
    return make_pair(b1, b2);
  }

  /*
  virtual pair<bool, bool> birelation(const XMEv& mev1,
                                      const XMEv& mev2) {
    pair<int, int> betterEquals = checkDominates(mev1, mev2);
    int better = betterEquals.first;
    int equals = betterEquals.second;

    int N = mev1.size();
    int better2 = N - better - equals;
    // 'v1' dominates 'v2'?
    bool b1 = (better + equals == N) && (better > 0);
    // 'v2' dominates 'v1'?
    bool b2 = (better2 + equals == N) && (better2 > 0);

    return make_pair(b1, b2);
  }
  */
};

}  // namespace optframe

#endif  // OPTFRAME_PARETODOMINANCE_HPP_
