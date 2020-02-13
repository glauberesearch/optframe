// ===================================
// Main.cpp file generated by OptFrame
// Project EFP
// ===================================

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <numeric>
#include "../../../OptFrame/RandGen.hpp"
#include "../../../OptFrame/Util/RandGenMersenneTwister.hpp"
#include "../Util/treatREDD.hpp"


using namespace std;
using namespace optframe;
using namespace HFM;
bool SPEED_UP_FLAG;
int nThreads;

int APEN_SI_DemandForecasting(int argc, char **argv)
{
	SPEED_UP_FLAG = false;
	cout << "Welcome to REW2016 -- SI -- REED " << endl;

	RandGenMersenneTwister rg;
	//long  1412730737
	long seed = time(nullptr); //CalibrationMode
	//force seed
	//seed = 9;
	cout << "Seed = " << seed << endl;
	srand(seed);
	rg.setSeed(seed);

	if (argc != 6)
	{
		cout << "Parametros incorretos!" << endl;
		cout << "Os parametros esperados sao: 1- instance 2- output 3- granularidade 4-argvforecastingHorizonteMinutes 5-timeES " << endl;

		exit(1);
	}

	const char* instance = argv[1];
	const char* outputFile = argv[2];
	double granularityMin = atoi(argv[3]);
	int argvforecastingHorizonteMinutes = atoi(argv[4]);
	int argvTimeES = atoi(argv[5]);
	int forecastingHorizonteMinutes = argvforecastingHorizonteMinutes;
	string nomeOutput = outputFile;
	string nomeInstace = instance;

	if (forecastingHorizonteMinutes < granularityMin)
	{
		cout << "EXIT WITH ERROR! Forecasting horizon lower than granularity" << endl;
		cout << "granularityMin: " << granularityMin << "\t forecastingHorizonteMinutes:" << forecastingHorizonteMinutes << endl;
		return 0;
	}

	treatREEDDataset alexandreTreatObj;
	vector<pair<double, double> > datasetDemandCut = alexandreTreatObj.cutData(nomeInstace.c_str(), "REED/saida.dat");
	cout << "data has ben cut with success" << endl;
	cout << datasetDemandCut.size() << endl;
	//	getchar();

	vector<pair<double, double> > datasetInterpolated = alexandreTreatObj.interpolate(datasetDemandCut, 1);
	cout << "data has been interpolated  with success" << endl;
	cout << datasetInterpolated.size() << endl;
	//	getchar();

	vector<pair<double, double> > datasetWithSpecificGranularity = alexandreTreatObj.separateThroughIntervals(datasetInterpolated, 60 * granularityMin, "./REED/instance", false);
	cout << "data has been split" << endl;
	cout << "dataSize =" << datasetWithSpecificGranularity.size() << endl;
	//cout<<dataset60Seconds<<endl;
	cout << "data saved with sucess" << endl;

	alexandreTreatObj.createInstance(datasetWithSpecificGranularity, "./REED/instance");

	cout << "instance created...." << endl;
	vector<string> explanatoryVariables;
	string instanceREED = "./REED/instance";
	vector<string> vInstances;
	vInstances.push_back(instanceREED);
	explanatoryVariables.push_back(vInstances[0]);
	treatForecasts rF(explanatoryVariables);

	int nBatches = 1;
	vector<vector<double> > vfoIndicatorCalibration; //vector with the FO of each batch
	vector<SolutionEFP> vSolutionsBatches; //vector with the solution of each batch
	for (int n = 0; n < nBatches; n++)
	{
		int mu = 10;
		int lambda = mu * 6;
		int evalFOMinimizer = MAPE_INDEX;
		int contructiveNumberOfRules = 100;
		int evalAprox = 0;
		double alphaACF = 0.5;
		int construtive = 2;
		// ============ END FORCES ======================

		// ============= METHOD PARAMETERS=================
		HFMParams methodParam;
		//seting up Continous ES params
		methodParam.setESInitialDesv(10);
		methodParam.setESMutationDesv(20);
		methodParam.setESMaxG(100000);

		//seting up ES params
		methodParam.setESMU(mu);
		methodParam.setESLambda(lambda);

		//seting up ACF construtive params
		methodParam.setConstrutiveMethod(construtive);
		methodParam.setConstrutivePrecision(contructiveNumberOfRules);
		vector<double> vAlphaACFlimits;
		vAlphaACFlimits.push_back(alphaACF);
		methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);

		//seting up Eval params
		methodParam.setEvalAprox(evalAprox);
		methodParam.setEvalFOMinimizer(evalFOMinimizer);
		// ==========================================

		// ================== READ FILE ============== CONSTRUTIVE 0 AND 1
		ProblemParameters problemParam;

		int nSA = forecastingHorizonteMinutes / granularityMin;
		problemParam.setStepsAhead(nSA);
		int stepsAhead = problemParam.getStepsAhead();

		int nTrainningDays = 7;
		double pointsPerHour = 60.0 / granularityMin;

		cout << "pointsPerHour:" << pointsPerHour << endl;
		cout << "granularityMin:" << granularityMin << endl;

		//========SET PROBLEM MAXIMUM LAG ===============
		//		problemParam.setMaxLag(pointsPerHour*24*3); // with maxLag equals to 2 you only lag K-1 as option
		problemParam.setMaxLag(pointsPerHour * 24 * 3); // with maxLag equals to 2 you only lag K-1 as option
		int maxLag = problemParam.getMaxLag(0);

		//If maxUpperLag is greater than 0 model uses predicted data
		problemParam.setMaxUpperLag(0);
//		int maxUpperLag = problemParam.getMaxUpperLag();
		//=================================================

		int numberOfTrainingPoints = 24 * pointsPerHour * nTrainningDays; //24hour * 7days * 4 points per hour
		int nTotalForecastingsTrainningSet = maxLag + numberOfTrainingPoints;

		//TODO remove for speed up
		//nTotalForecastingsTrainningSet = argSamplesTrainingSet - nSA;

		int beginTrainingSet = 1;

		int totalNumberOfSamplesTarget = rF.getForecastsSize(0);
		cout << "BeginTrainninningSet: " << beginTrainingSet << endl;
		cout << "\t #nTotalForecastingsTrainningSet: " << nTotalForecastingsTrainningSet << endl;
		cout << "#sizeTrainingSet: " << totalNumberOfSamplesTarget << endl;
		cout << "maxNotUsed: " << problemParam.getMaxLag(0) << endl;
		cout << "#StepsAhead: " << stepsAhead << endl;
		cout << "#forecastingHorizonteMinutes: " << forecastingHorizonteMinutes << endl;
		cout << "#granularityMin: " << granularityMin << endl << endl;

		int timeES = argvTimeES;
		vector<double> foIndicatorCalibration;
		vector<double> vectorOfForecasts;
		double averageError = 0;
		int countSlidingWindows = 0;

		for (int begin = 0; (nTotalForecastingsTrainningSet + begin + stepsAhead) <= totalNumberOfSamplesTarget; begin += stepsAhead)
		{
			vector<vector<double> > trainningSet; // trainningSetVector
			trainningSet.push_back(rF.getPartsForecastsBeginToEnd(0, begin, nTotalForecastingsTrainningSet));

			ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

			std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
			sol = forecastObject.run(timeES, 0, 0);

			vector<vector<double> > validationSet; //validation set for calibration
			cout << "blind test begin: " << nTotalForecastingsTrainningSet + begin << " end:" << nTotalForecastingsTrainningSet + begin + stepsAhead << endl;

			validationSet.push_back(rF.getPartsForecastsBeginToEnd(0, nTotalForecastingsTrainningSet + begin - maxLag, maxLag + stepsAhead));
			vector<double> foIndicatorsWeeks;
			foIndicatorsWeeks = *forecastObject.returnErrors(sol->first.getR(), validationSet);
			foIndicatorCalibration.push_back(foIndicatorsWeeks[MAPE_INDEX]);
			averageError += foIndicatorsWeeks[MAPE_INDEX];

			vector<double> currentForecasts = *forecastObject.returnForecasts(*sol, validationSet);
			for (int cF = 0; cF < (int) currentForecasts.size(); cF++)
				vectorOfForecasts.push_back(currentForecasts[cF]);

			cout << foIndicatorCalibration << "\t average:" << averageError / (countSlidingWindows + 1) << endl;
			countSlidingWindows++;
		}
		cout << foIndicatorCalibration << endl;

		double finalAverage = 0;
		for (int e = 0; e < (int) foIndicatorCalibration.size(); e++)
			finalAverage += foIndicatorCalibration[e];
		finalAverage /= foIndicatorCalibration.size();

		vector<double> parametersResults;
		parametersResults.push_back(finalAverage);
		parametersResults.push_back(stepsAhead);
		parametersResults.push_back(forecastingHorizonteMinutes);
		parametersResults.push_back(granularityMin);
		parametersResults.push_back(numberOfTrainingPoints);
		parametersResults.push_back(maxLag);
		parametersResults.push_back(mu);
		parametersResults.push_back(lambda);
		parametersResults.push_back(timeES);
		parametersResults.push_back(seed);

		string calibrationFile = "./results_APEN_SI";

		FILE* fResults = fopen(calibrationFile.c_str(), "a");

		for (int i = 0; i < (int) parametersResults.size(); i++)
			fprintf(fResults, "%.7f\t", parametersResults[i]);

		for (int i = 0; i < (int) vectorOfForecasts.size(); i++)
			fprintf(fResults, "%.3f\t", vectorOfForecasts[i]);

		fprintf(fResults, "\n");

		fclose(fResults);

	}

	cout << "APEN Batch SI finished!" << endl;
	return 0;
}

int APEN_SI_SpeedUp_DemandForecasting(int argc, char **argv)
{
	SPEED_UP_FLAG = true;
	cout << "Welcome to APEN -- SI -- SPEED UP " << endl;

	RandGenMersenneTwister rg;
	//long  1412730737
	long seed = time(nullptr); //CalibrationMode
	//force seed
	seed = 9;
	cout << "Seed = " << seed << endl;
	srand(seed);
	rg.setSeed(seed);

	if (argc != 6)
	{
		cout << "Parametros incorretos!" << endl;
		cout << "Os parametros esperados sao: instance(notused) output(notused) ; samplesTraining set; argNSA ; argNThreads" << endl;
		// ./REED/channel_1.dat ./UBUNTU1604 60	24	120 1000 24
		exit(1);
	}

	const char* instance = argv[1];
	const char* outputFile = argv[2];
	int argSamplesTrainingSet = atoi(argv[3]);
	int argNSA = atoi(argv[4]);
	int argNThreads = atoi(argv[5]);
	string nomeOutput = outputFile;
	string nomeInstace = instance;
	nThreads = argNThreads;


	//force granularity todo
	double granularityMin = 1 / 60.0; // can be used for generating instance

	//===================================
	cout << "Parametros:" << endl;
	cout << "nomeOutput=" << nomeOutput << endl;
	cout << "granularityMin=" << granularityMin << endl;
	cout << "nomeOutput=" << nomeOutput << endl;

	vector<string> explanatoryVariables;
	string instanceREED = "./REED/instanceForSpeedUp";

	vector<string> vInstances;
	vInstances.push_back(instanceREED);
	explanatoryVariables.push_back(vInstances[0]);
	treatForecasts rF(explanatoryVariables);


	int nBatches = 1;
	vector<vector<double> > vfoIndicatorCalibration; //vector with the FO of each batch
	vector<SolutionEFP> vSolutionsBatches; //vector with the solution of each batch

	for (int n = 0; n < nBatches; n++)
	{
		int mu = 10;
		int lambda = mu * 6;
		int evalFOMinimizer = MAPE_INDEX;
		int contructiveNumberOfRules = 100;
		int evalAprox = 0;
		double alphaACF = 0.5;
		int construtive = 2;
		// ============ END FORCES ======================

		// ============= METHOD PARAMETERS=================
		HFMParams methodParam;
		//seting up Continous ES params
		methodParam.setESInitialDesv(10);
		methodParam.setESMutationDesv(20);
		methodParam.setESMaxG(100000);

		//seting up ES params
		methodParam.setESMU(mu);
		methodParam.setESLambda(lambda);

		//seting up ACF construtive params
		methodParam.setConstrutiveMethod(construtive);
		methodParam.setConstrutivePrecision(contructiveNumberOfRules);
		vector<double> vAlphaACFlimits;
		vAlphaACFlimits.push_back(alphaACF);
		methodParam.setConstrutiveLimitAlphaACF(vAlphaACFlimits);

		//seting up Eval params
		methodParam.setEvalAprox(evalAprox);
		methodParam.setEvalFOMinimizer(evalFOMinimizer);
		// ==========================================

		// ================== READ FILE ============== CONSTRUTIVE 0 AND 1
		ProblemParameters problemParam;
		//ProblemParameters problemParam(vParametersFiles[randomParametersFiles]);

		//TODO remove for speed up
		cout << "forcing number of steps ahead nSA - line 200 - Than forecastingHorizonteMinutes is not used" << endl;
		int nSA = argNSA;

		problemParam.setStepsAhead(nSA);
		int stepsAhead = problemParam.getStepsAhead();

//		int nTrainningDays = 10;
		double pointsPerHour = 60.0 / granularityMin;

		cout << "pointsPerHour:" << pointsPerHour << endl;
		cout << "granularityMin:" << granularityMin << endl;

		//========SET PROBLEM MAXIMUM LAG ===============
//		problemParam.setMaxLag(pointsPerHour*24*3); // with maxLag equals to 2 you only lag K-1 as option
		problemParam.setMaxLag(4000); // with maxLag equals to 2 you only lag K-1 as option
		int maxLag = problemParam.getMaxLag(0);

		//If maxUpperLag is greater than 0 model uses predicted data
		problemParam.setMaxUpperLag(0);
//		int maxUpperLag = problemParam.getMaxUpperLag();
		//=================================================

		int nTotalForecastingsTrainningSet =  argSamplesTrainingSet - nSA;


		int beginTrainingSet = 1;

		int totalNumberOfSamplesTarget = rF.getForecastsSize(0);
		cout << "BeginTrainninningSet: " << beginTrainingSet << endl;
		cout << "\t #nTotalForecastingsTrainningSet: " << nTotalForecastingsTrainningSet << endl;
		cout << "#sizeTrainingSet: " << totalNumberOfSamplesTarget << endl;
		cout << "maxNotUsed: " << problemParam.getMaxLag(0) << endl;
		cout << "#StepsAhead: " << stepsAhead << endl;
		cout << "#granularityMin(NotUSED): " << granularityMin << endl << endl;

		int timeES = 1200;
		vector<double> foIndicatorCalibration;
		vector<double> vectorOfForecasts;
		double averageError = 0;
		int countSlidingWindows = 0;
//getchar();
		//SPEED UP teste
		string speedUpFile = "./apen_SI_speedUpFile";
		FILE* fSpeedUp = fopen(speedUpFile.c_str(), "a");
		int speedUpNTR = (nTotalForecastingsTrainningSet - maxLag) / nSA;
		fprintf(fSpeedUp, "%d\t%d\t%d\t%d\t", nSA, nTotalForecastingsTrainningSet, speedUpNTR,nThreads);
		fclose(fSpeedUp);

		///END SPEED UP TEST

		for (int begin = 0; (nTotalForecastingsTrainningSet + begin + stepsAhead) <= totalNumberOfSamplesTarget; begin += stepsAhead)
		{
			vector<vector<double> > trainningSet; // trainningSetVector
			trainningSet.push_back(rF.getPartsForecastsBeginToEnd(0, begin, nTotalForecastingsTrainningSet));

			ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

			std::optional<pair<SolutionHFM, Evaluation<>>> sol = std::nullopt;
			sol = forecastObject.run(timeES, 0, 0);

			vector<vector<double> > validationSet; //validation set for calibration
			cout << "blind test begin: " << nTotalForecastingsTrainningSet + begin << " end:" << nTotalForecastingsTrainningSet + begin + stepsAhead << endl;

			validationSet.push_back(rF.getPartsForecastsBeginToEnd(0, nTotalForecastingsTrainningSet + begin - maxLag, maxLag + stepsAhead));
			vector<double> foIndicatorsWeeks;
			foIndicatorsWeeks = *forecastObject.returnErrors(sol->first.getR(), validationSet);
			foIndicatorCalibration.push_back(foIndicatorsWeeks[MAPE_INDEX]);
			averageError += foIndicatorsWeeks[MAPE_INDEX];

			vector<double> currentForecasts = *forecastObject.returnForecasts(*sol, validationSet);
			for (int cF = 0; cF < (int) currentForecasts.size(); cF++)
				vectorOfForecasts.push_back(currentForecasts[cF]);

			cout << foIndicatorCalibration << "\t average:" << averageError / (countSlidingWindows + 1) << endl;

			countSlidingWindows++;
		}
	}

	cout << "REW apen SI finished!" << endl;
	return 0;
}
