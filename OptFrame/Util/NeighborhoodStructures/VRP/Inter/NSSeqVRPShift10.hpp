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


#ifndef NSSeqVRPShift10_HPP_
#define NSSeqVRPShift10_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPShift10: public Move<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

protected:

	int r1, r2; //routes id's
	int cli;//cli shifted in r1
	int pos;// insertion position in r2

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	MoveVRPShift10(int _r1, int _r2, int _cli, int _pos, OPTFRAME_DEFAULT_PROBLEM* _problem = NULL) :
	r1(_r1), r2(_r2), cli(_cli), pos(_pos), problem(_problem)
	{
	}

	virtual ~MoveVRPShift10()
	{
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_cli()
	{
		return cli;
	}

	int get_pos()
	{
		return pos;
	}

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool numRoutes = rep.size() >= 2;
		return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
	}

	virtual Move<Routes, ADS, DS>& apply(Routes& rep, ADS&)
	{
		//pegando o cliente
		int c = rep.at(r1).at(cli);

		//removendo os clientes de cada rota
		rep.at(r1).erase(rep.at(r1).begin() + cli);

		//fazendo a inserção
		rep.at(r2).insert(rep.at(r2).begin() + pos, c);
		return *new MoveVRPShift10(r2, r1, pos, cli);
	}


	virtual bool operator==(const Move<Routes, ADS, DS>& _m) const
	{
		const MoveVRPShift10& m = (const MoveVRPShift10&) _m;
		return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
	}

	void print() const
	{
		cout << "MoveVRPShift10( ";
		cout << r1 << " , ";
		cout << r2 << " , ";
		cout << cli << " , ";
		cout << pos << " )";
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
		class MOVE = MoveVRPShift10<T, ADS, DS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPShift10: public NSIterator<vector<vector<T> >, ADS, DS >
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRPShift10<T, ADS, DS >* m;
	vector<MoveVRPShift10<T, ADS, DS >*> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last

public:

	NSIteratorVRPShift10(const Routes& _r, P* _p = NULL) :
	r(_r)
	{
		m = NULL;
		index = 0;
	}

	virtual ~NSIteratorVRPShift10()
	{
		/*if (moves.size() > 0) //todo see this with Igor
		 {
		 for (unsigned int i = 0; i < moves.size(); i++)
		 delete moves[i];
		 moves.clear();
		 }*/
	}

	void first()
	{
		for (int r1 = 0; r1 < r.size(); r1++)
		{
			for (int r2 = 0; r2 < r.size(); r2++)
			{
				if (r1 != r2)
				{
					for (int cli = 0; cli < r.at(r1).size(); cli++)
					{
						for (int pos = 0; pos <= r.at(r2).size(); pos++)
						{
							moves.push_back(new MOVE(r1, r2, cli, pos, p));
						}
					}
				}
			}
		}
		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return m == NULL;
	}

	Move<Routes, ADS, DS>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPShift10. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS,
		class MOVE = MoveVRPShift10<T, ADS, DS> , class P = OPTFRAME_DEFAULT_PROBLEM>
class NSSeqVRPShift10: public NSSeq<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPShift10(P* _p = NULL) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPShift10()
	{
	}

	virtual Move<Routes, ADS, DS>& move(const Routes& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, p);

		int r1 = rand() % rep.size();
		if (rep.at(r1).size() == 0)
		{
			return *new MOVE(-1, -1, -1, -1, p);
		}

		int r2;
		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		int cli = rand() % rep.at(r1).size();

		int pos = rand() % (rep.at(r2).size() + 1);
		return *new MOVE(r1, r2, cli, pos, p); // return a random move
	}

	virtual NSIteratorVRPShift10<T, ADS, DS, MOVE, P >& getIterator(const Routes& r, const ADS&)
	{
		return *new NSIteratorVRPShift10<T, ADS, DS, MOVE, P >(r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPShift10 with move: " << MOVE::idComponent();
	}
};

#endif /*NSSeqVRPShift10_HPP_*/
