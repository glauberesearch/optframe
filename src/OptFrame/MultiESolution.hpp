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

#ifndef OPTFRAME_MULTIESOLUTION_HPP_
#define OPTFRAME_MULTIESOLUTION_HPP_

#include <vector>

#include "Component.hpp"
#include "Solution.hpp"
#include "MultiSolution.hpp"
#include "Evaluation.hpp"

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class MultiESolution: public Component
{
protected:
	vector<S*> p;
   vector<XEv*> pev;

public:

	MultiESolution()
	{
	}

	MultiESolution(const MultiESolution& pop)
	{
		for (unsigned i = 0; i < pop.size(); i++)
			p.push_back(&pop.at(i).clone());
	}

	virtual ~MultiESolution()
	{
		clear();
	}

	unsigned size() const
	{
		return p.size();
	}

	S& at(unsigned c)
	{
		return (*p.at(c));
	}

	const S& at(unsigned c) const
	{
		return (*p.at(c));
	}

	void insert(unsigned pos, S& c)
	{
		p.insert(p.begin() + pos, new S(c));
	}

	void push_back(S* c)
	{
		if (c) // not null
			p.push_back(c);
	}

	void push_back(const S& c)
	{
		p.push_back(&c.clone());
	}

	S& remove(unsigned pos)
	{
		S& c = *p.at(pos);
		p.erase(p.begin() + pos);
		return c;
	}

	void add(const MultiSolution<S>& pop)
	{
		for (unsigned i = 0; i < pop.size(); i++)
		{
			const S& s = pop.at(i);
			push_back(s);
		}
	}

	// clear and kill
	void clear()
	{
		for (unsigned i = 0; i < p.size(); i++)
			delete p.at(i);

		p.clear();
	}

	void clearNoKill()
	{
		p.clear();
	}

	bool empty()
	{
		return p.empty();
	}

	virtual MultiESolution<S, XEv>& operator=(const MultiESolution<S, XEv>& p)
	{
		if (&p == this) // auto ref check
			return *this;

		unsigned sizePop = this->p.size();

		for (unsigned i = 0; i < sizePop; i++)
		{
			if (this->p.at(i)) // If no nullptr pointing.
			{
				delete this->p.at(i);
			}
		}

		this->p.clear();

		sizePop = p.size();

		for (unsigned i = 0; i < sizePop; i++)
		{
			if (&p.at(i)) // If no nullptr pointing.
			{
				this->p.push_back(new S(p.at(i)));
			}
			else
			{
				this->p.push_back(nullptr);
			}
		}

		return (*this);
	}

	virtual MultiESolution<S>& clone() const
	{
		return *new MultiESolution<S, XEv>(*this);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":MultiESolution";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << "MultiESolution(" << p.size() << ")";
		cout << endl;

		for (unsigned i = 0; i < p.size(); i++)
		{
			p.at(i)->print();
		}
	}

};

}

#endif /* OPTFRAME_MULTISOLUTION_HPP_ */