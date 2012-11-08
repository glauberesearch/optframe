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

#ifndef OPTFRAME_ILS_HPP_
#define OPTFRAME_ILS_HPP_

#include <math.h>
#include <vector>

#include "../Constructive.h"
#include "../SingleObjSearch.hpp"
#include "../Evaluator.hpp"

template<class H, class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class IteratedLocalSearch: public SingleObjSearch<R, ADS, M>
{
protected:
	Evaluator<R, ADS, M>& evaluator;
	Constructive<R, ADS>& constructive;

public:

	IteratedLocalSearch(Evaluator<R, ADS, M>& _evaluator, Constructive<R, ADS>& _constructive) :
		evaluator(_evaluator), constructive(_constructive)
	{
	}

	virtual ~IteratedLocalSearch()
	{
	}

	virtual H& initializeHistory() = 0;

	virtual void localSearch(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f) = 0;

	virtual void perturbation(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f, H& history) = 0;

	virtual Solution<R, ADS>& acceptanceCriterion(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2, H& history) = 0;

	virtual bool terminationCondition(H& history) = 0;

	pair<Solution<R, ADS>&, Evaluation<M>&>* search(double timelimit = 100000000, double target_f = 0)
	{
		cout << "ILS search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation<M>& e = evaluator.evaluate(s);

		H* history = &initializeHistory();

		// 's0' <- GenerateSolution
		// 's*' <- localSearch 's'


		localSearch(s, e, (timelimit - (tnow.now())), target_f);

		Solution<R, ADS>* sStar = &s.clone();
		Evaluation<M>* eStar = &e.clone();

		cout << "ILS starts: ";
		e.print();

		do
		{
			Solution<R, ADS>* s1 = &sStar->clone();
			Evaluation<M>* e1 = &eStar->clone();

			perturbation(*s1, *e1, (timelimit - (tnow.now())), target_f, *history);

			localSearch(*s1, *e1, (timelimit - (tnow.now())), target_f);

			Solution<R, ADS>* s2 = s1;
			Evaluation<M>* e2 = e1;

			Solution<R, ADS>* sStar1 = &acceptanceCriterion(*sStar, *s2, *history);

			delete sStar;
			delete eStar;
			delete s2;
			delete e2;

			sStar = sStar1;
			eStar = &evaluator.evaluate(*sStar);

		} while (evaluator.betterThan(target_f, eStar->evaluation()) && !terminationCondition(*history) && ((tnow.now()) < timelimit));

		e = *eStar;
		s = *sStar;

		delete eStar;
		delete sStar;

		delete history;

		return new pair<Solution<R, ADS>&, Evaluation<M>&> (s, e);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, M>::idComponent() << "ILS:";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

#endif /*OPTFRAME_ILS_HPP_*/
