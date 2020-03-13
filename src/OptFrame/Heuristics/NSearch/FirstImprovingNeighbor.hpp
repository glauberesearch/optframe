// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_
#define OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_

#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"
#include "../../Evaluator.hpp"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class FirstImprovingNeighbor: public NeighborhoodSearch<XES, XEv>
{
private:
	GeneralEvaluator<XES, XEv, XSH>& eval;
	NSSeq<XES, XEv, XSH>& nsSeq;

public:

	FirstImprovingNeighbor(GeneralEvaluator<XES, XEv>& _eval, NSSeq<XES, XEv, XSH>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovingNeighbor()
	{
	}

	virtual uptr<Move<XES, XEv>> searchMove(const XES& se, const StopCriteria<XEv>& stopCriteria) override
	{
      // get valid iterator
		uptr<NSIterator<XES, XEv>> it = nsSeq.getIterator(se);
      //
      assert(it);
      //
		it->first();
      //
		if (it->isDone())		
			return nullptr; // no neighbor
      //
		do
		{
         // get move
			uptr<Move<XES, XEv, XSH>> move = it->current();
         //
			if (move->canBeApplied(se))
			{
				if(this->acceptsImprove(*move, se))
				{
					// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
					//e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of Local Optimum

					return;
				}
			}

			it->next();
		}
		while (!it->isDone());

		// TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
		//if(bestMoveId != "")
		//	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local Optimum
	}

   // returns 'cost' if it's improving, otherwise std::nullopt
   op<XEv> isImprovingCost(Move<XES, XEv>& m, const XSH& cse, bool allowEstimated = false)
   {
      // try to get a cost
      op<XEv> p = m.cost(cse, allowEstimated);
      // if p not null => much faster (using cost)
      if (p) {
         // verify if m is an improving move
         //if (p->isStrictImprovement()) {
         if (eval.isStrictImprovement(*p))
            return p;
         else
            return nullopt;
      } else {
         // need to update 's' together with reevaluation of 'e' => slower (may perform reevaluation)

         // TODO: in the future, consider moves with nullptr reverse (must save original solution/evaluation)
         assert(m.hasReverse());

         // remove constness! must make sure 'cse' is never changed from its original state
         XSH& se = const_cast<XSH&>(cse);
         XEv& e = se.second;

         // saving previous evaluation
         XEv ev_begin(e);
 
         // apply move to both XEv and Solution
         uptr<Move<XES, XEv>> rev = eval.applyMoveReevaluate(m, se);

         // compute cost directly on Evaluation
         XEv mcost = ev_begin.diff(se.second);

         // check if it is improvement
         if (eval.isStrictImprovement(mcost)) {
            return true;
         }

         // must return to original situation

         // apply reverse move in order to get the original solution back
         //TODO - Vitor, Why apply Move with e is not used???
         //			Even when reevaluate is implemented, It would be hard to design a strategy that is faster than copying previous evaluation
         //==================================================================
         //pair<Move<S, XEv>*, XEv> ini = applyMove(*rev, s);

         // if XEv wasn't 'outdated' before, restore its previous status
         //			if (!outdated)
         //				e.outdated = outdated;

         // go back to original evaluation
         //			e = ini.second;
         //			delete ini.first;

         uptr<Move<XES, XEv>> ini = rev->apply(se);
         // for now, must be not nullptr
         assert(ini != nullptr);
         // TODO: include management for 'false' hasReverse()
         assert(rev->hasReverse() && ini);
         e = std::move(ev_begin);
         //==================================================================

         return false;
      }

   }


	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NeighborhoodSearch<XES, XEv>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << NeighborhoodSearch<XES, XEv>::idComponent() << ":FirstImprovingNeighbor";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "FirstImprovingNeighbor: " << nsSeq.toString();
		return ss.str();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class FirstImprovingNeighborBuilder : public NeighborhoodSearchBuilder<S, XEv, XES, X2ES>
{
public:
	virtual ~FirstImprovementBuilder()
	{
	}

	virtual NeighborhoodSearch<XES, XEv>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		GeneralEvaluator<XES, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<XES, XEv, XSH>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<XES, XEv, XSH>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovingNeighbor<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<S, XEv>::idComponent() << ":FirstImprovingNeighbor";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_FIRST_IMPROVING_NEIGHBOR_HPP_*/