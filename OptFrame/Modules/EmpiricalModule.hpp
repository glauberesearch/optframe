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

#ifndef EMPIRICALMODULE_HPP_
#define EMPIRICALMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class EmpiricalModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~EmpiricalModule()
	{
	}

	string id()
	{
		return "empirical";
	}
	string usage()
	{
		string u = "empirical N T TF BF EVAL METHOD OUTPUTFILE\n WHERE:\n";
		u += "N is the number of tests to be executed;\n";
		u += "T is the timelimit, in seconds, for each test; (0 for no timelimit)\n";
		u += "TF is the target evaluation function value;\n";
		u += "BF is the best known evaluation function value;\n";
		u += "METHOD is the method to be tested with its own parameters;\n";
		u += "OUTPUTFILE is the output file.\n";

		return u;
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return true;
		}

		int n = scanner.nextInt();
		int t = scanner.nextDouble();
		double tf = scanner.nextDouble();
		double bf = scanner.nextDouble();

		pair<SingleObjSearch<R, ADS, M>*, string> method = factory.createSingleObjSearch(scanner.rest());

		SingleObjSearch<R, ADS, M>* h = method.first;

		string filename = method.second;

		if (t == 0) // if no timelimit
			t = 1000000000;

		long timelimit = t;

		if (bf == 0) // if best is zero
			bf = 0.00001;

		FILE* file = fopen(filename.c_str(), "a");
		if (!file)
		{
			cout << "Error creating file '" << filename << "'" << endl;
			return true;
		}

		double t_now = 0;
		double fo_now = 0;

		vector<double> time_spent;

		for (int i = 0; i < n; i++)
		{
			long seed = time(NULL) + i;
			fprintf(file, "%ld\t", seed);

			cout << "Test " << i << " {seed=" << seed << "}... Running";
			Timer t(false);

			pair<Solution<R, ADS>&, Evaluation<M>&>* result = h->search(timelimit, tf);

			if(!result)
			{
				cout << "EMPIRICAL ERROR!" << endl;
				return false;
			}

			Solution<R, ADS>* s2 = &result->first;

			t_now = t.now();
			fo_now = result->second.evaluation();
			fprintf(file, "%f\t%.3f\t", fo_now, t_now);
			time_spent.push_back(t_now);

			cout << "... Finished! (" << t.now() << "secs.)" << endl;

			delete s2;

			fprintf(file, "\n");
		}

		sort(time_spent.begin(), time_spent.end());

		fprintf(file, "EMPIRICAL PROBABILITY DISTRIBUTION:\ni\ttime(i)\tp(i)=(i-0.5)/N\n0\t0\t0\n");
		for (int i = 1; i <= n; i++)
			fprintf(file, "%i\t%.3f\t%.3f\n", i, time_spent[i - 1], (i - 0.5) / n);

		fprintf(file, "PARAMETERS:%s\n", input.c_str());

		fclose(file);
		return true;
	}

};

#endif /* EMPIRICALMODULE_HPP_ */
