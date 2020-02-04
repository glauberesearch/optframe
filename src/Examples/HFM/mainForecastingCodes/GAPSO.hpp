// ===================================
// Main.cpp file generated by OptFrame
// Project EFP
// ===================================

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <numeric>
#include "../../../OptFrame/RandGen.hpp"
#include "../../../OptFrame/Util/RandGenMersenneTwister.hpp"

using namespace std;
using namespace optframe;
using namespace HFM;

int GAPSO_SKU(int argc, char **argv)
{
	cout << "Welcome to GAPSO-SKU calibration" << endl;
	RandGenMersenneTwister rg;
	//long  1412730737
	long seed = time(nullptr); //CalibrationMode
	seed = 111212101990;
	cout << "Seed = " << seed << endl;
	srand(seed);
	rg.setSeed(seed);

	if (argc != 5)
	{
		cout << "Parametros incorretos!" << endl;
		cout << "Os parametros esperados sao: nomeOutput targetTS construtiveNRulesACF timeES" << endl;
		exit(1);
	}

	const char* caminhoOutput = argv[1];
	int argvTargetTimeSeries = atoi(argv[2]);
	int argvMaxLagRate = atoi(argv[3]);
	int argvTimeES = atoi(argv[4]);

	string nomeOutput = caminhoOutput;
	//===================================
	cout << "Parametros:" << endl;
	cout << "nomeOutput=" << nomeOutput << endl;
	cout << "argvTargetTimeSeries=" << argvTargetTimeSeries << endl;
	cout << "argvMaxLagRate=" << argvMaxLagRate << endl;
	cout << "argvTimeES=" << argvTimeES << endl;

	File* fileWCCIInstances;
	string gapsoTimeSeries = "./MyProjects/HFM/Instance/GAPSO/SKUPure";
//	gapsoTimeSeries = "./MyProjects/HFM/Instance/GAPSO/SKU";
	try
	{
		fileWCCIInstances = new File(gapsoTimeSeries.c_str());
	} catch (FileNotFound& f)
	{
		cout << "File '" << gapsoTimeSeries.c_str() << "' not found" << endl;
		exit(1);
	}

	Scanner* scannerWCCI = new Scanner(fileWCCIInstances);
	for (int i = 0; i < (argvTargetTimeSeries - 1); i++)
		scannerWCCI->nextLine();

	string testProblemWCCI = scannerWCCI->nextLine();
	delete scannerWCCI;
	scannerWCCI = new Scanner(testProblemWCCI);

//	cout << scannerWCCI->nextInt() << endl;
	int GAPSOStepsAhead = 12;
//	cout << "Required steps ahead:" << WCCIStepsAhead << endl;

	vector<double> forecastsTSWCCI;
	while (scannerWCCI->hasNext())
		forecastsTSWCCI.push_back(scannerWCCI->nextDouble());
	cout << forecastsTSWCCI << endl;

	vector<vector<double> > forecastingWCCIExogenousVariables;
	forecastingWCCIExogenousVariables.push_back(forecastsTSWCCI);

	treatForecasts rF(forecastingWCCIExogenousVariables);

	int nBatches = 1;

	vector<vector<double> > vfoIndicatorCalibration; //vector with the FO of each batch

	vector<SolutionEFP> vSolutionsBatches; //vector with the solution of each batch

	vector<double> vForecasts;

	for (int n = 0; n < nBatches; n++)
	{
//		int contructiveNumberOfRules = rg.rand(maxPrecision) + 10;
//		int evalFOMinimizer = rg.rand(NMETRICS); //tree is the number of possible objetive function index minimizers
//		int evalAprox = rg.rand(2); //Enayatifar aproximation using previous values
//		int construtive = rg.rand(3);
//		double initialDesv = rg.rand(maxInitialDesv) + 1;
//		double mutationDesv = rg.rand(maxMutationDesv) + 1;
//		int mu = rg.rand(maxMu) + 1;
//		int lambda = mu * 6;

		//limit ACF for construtive ACF
//		double alphaACF = rg.rand01();
//		int alphaSign = rg.rand(2);
//		if (alphaSign == 0)
//			alphaACF = alphaACF * -1;

		// ============ FORCES ======================
//		initialDesv = 10;
//		mutationDesv = 20;
		int mu = 100;
		int lambda = mu * 6;
		int evalFOMinimizer = WMAPE_INDEX;
		int contructiveNumberOfRules = 10;
		int evalAprox = 0;
		double alphaACF = -1;
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
		int nSA = GAPSOStepsAhead;
		problemParam.setStepsAhead(nSA);
		int stepsAhead = problemParam.getStepsAhead();

		int nTotalForecastingsTrainningSet = rF.getForecastsSize(0) - stepsAhead;

		//========SET PROBLEM MAXIMUM LAG ===============
		cout << "argvMaxLagRate = " << argvMaxLagRate << endl;

		int iterationMaxLag = ((nTotalForecastingsTrainningSet - stepsAhead) * argvMaxLagRate) / 100.0;
		iterationMaxLag = ceil(iterationMaxLag);
		if (iterationMaxLag > (nTotalForecastingsTrainningSet - stepsAhead))
			iterationMaxLag--;
		if (iterationMaxLag <= 0)
			iterationMaxLag = 1;

		problemParam.setMaxLag(iterationMaxLag);
		int maxLag = problemParam.getMaxLag(0);

		//If maxUpperLag is greater than 0 model uses predicted data
		problemParam.setMaxUpperLag(0);
		//int maxUpperLag = problemParam.getMaxUpperLag();
		//=================================================

		int timeES = argvTimeES; // online training time

		vector<double> foIndicators;

		int beginTrainingSet = 0;
		//int nTrainningRounds = 3;
		//int nTotalForecastingsTrainningSet = maxLag + nTrainningRounds * stepsAhead;

		cout << std::setprecision(9);
		cout << std::fixed;
		double NTRaprox = (nTotalForecastingsTrainningSet - maxLag) / double(stepsAhead);
		cout << "BeginTrainninningSet: " << beginTrainingSet << endl;
		cout << "#nTotalForecastingsTrainningSet: " << nTotalForecastingsTrainningSet << endl;
		cout << "#~NTR: " << NTRaprox << endl;
		cout << "#sizeTrainingSet: " << rF.getForecastsSize(0) << endl;
		cout << "#maxNotUsed: " << maxLag << endl;
		cout << "#StepsAhead: " << stepsAhead << endl << endl;

//		getchar();
		vector<vector<double> > trainningSet; // trainningSetVector
		trainningSet.push_back(rF.getPartsForecastsEndToBegin(0, stepsAhead, nTotalForecastingsTrainningSet));
		cout << trainningSet << endl;

		ForecastClass forecastObject(trainningSet, problemParam, rg, methodParam);

//		forecastObject.runMultiObjSearch();
//		getchar();
		pair<SolutionHFM, Evaluation<>>* sol;
		sol = forecastObject.run(timeES, 0, 0);
		cout << sol->first.getR() << endl;
//		getchar();

		vector<double> foIndicatorCalibration;
		vector<vector<double> > validationSet;
		validationSet.push_back(rF.getPartsForecastsEndToBegin(0, 0, maxLag + stepsAhead));

		cout << validationSet << endl;
//		getchar();

		vector<double> errors = *forecastObject.returnErrors(sol->first.getR(), validationSet);
		vForecasts = *forecastObject.returnForecasts(sol, validationSet);
		cout << "Vector of forecasts: \n " << vForecasts << endl;
		vector<vector<double> > blind = validationSet;
		blind[0].erase(blind[0].end() - stepsAhead, blind[0].end());
		cout << "Blind forecasts: \n " << forecastObject.returnBlind(sol->first.getR(), blind) << endl;

		foIndicators.push_back(errors[WMAPE_INDEX]);
		foIndicators.push_back(sol->second.evaluation());
		foIndicators.push_back(argvTargetTimeSeries);
		foIndicators.push_back(argvMaxLagRate);
		foIndicators.push_back(maxLag);
		foIndicators.push_back(NTRaprox);
		foIndicators.push_back(contructiveNumberOfRules);
		foIndicators.push_back(timeES);
		foIndicators.push_back(seed);
		vfoIndicatorCalibration.push_back(foIndicators);
	}

	cout << setprecision(3);

//	double averageError = 0;
//	for (int t = 0; t < (vfoIndicatorCalibration[0].size() - 1); t++)
//	{
//		averageError += vfoIndicatorCalibration[0][t];
//	}
//
//	averageError /= (vfoIndicatorCalibration[0].size() - 1);
//	vfoIndicatorCalibration[0].push_back(averageError);
	// =================== PRINTING RESULTS ========================
	for (int n = 0; n < nBatches; n++)
	{

		for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
			cout << vfoIndicatorCalibration[n][i] << "\t";

		cout << endl;
	}
	// =======================================================

	// =================== PRINTING RESULTS ON FILE ========================
	string calibrationFile = "./GAPSO_InitialResults";
	FILE* fResults = fopen(calibrationFile.c_str(), "a");
	for (int n = 0; n < nBatches; n++)
	{
		for (int i = 0; i < int(vfoIndicatorCalibration[n].size()); i++)
			fprintf(fResults, "%.7f\t", vfoIndicatorCalibration[n][i]);
		fprintf(fResults, "\n");
	}

	string calibrationFileForecasts = "./GAPSO_InitialResultsForecasts";
	FILE* fResultsForecasts = fopen(calibrationFileForecasts.c_str(), "a");
	for (int n = 0; n < nBatches; n++)
	{
		fprintf(fResultsForecasts, "%d\t", argvTargetTimeSeries);
		for (int i = 0; i <  (int) vForecasts.size(); i++)
			fprintf(fResultsForecasts, "%.7f\t", vForecasts[i]);
		fprintf(fResultsForecasts, "\n");
	}

	fclose(fResults);
	fclose(fResultsForecasts);
	// =======================================================

	return 0;
}

//
//for (int w = 4; w >= 1; w--)
//	{
//		vector<double> foIndicatorsMAPE;
//		vector<double> foIndicatorsRMSE;
//
//		for (int day = 1; day <= 7; day++)
//		{
//			vector<vector<double> > validationSet; //validation set for calibration
//			validationSet.push_back(rF.getPartsForecastsEndToBegin(0, w * 168 - stepsAhead * day, nValidationSamples));
//			vector<double> foIndicators;
//			foIndicators = forecastObject.returnErrors(sol, validationSet);
//			foIndicatorsMAPE.push_back(foIndicators[MAPE_INDEX]);
//			foIndicatorsRMSE.push_back(foIndicators[RMSE_INDEX]);
//		}
//		double sumMAPE = accumulate(foIndicatorsMAPE.begin(), foIndicatorsMAPE.end(), 0.0);
//		double sumRMSE = accumulate(foIndicatorsRMSE.begin(), foIndicatorsRMSE.end(), 0.0);
//
//		foIndicatorCalibration.push_back(sumMAPE/foIndicatorsMAPE.size());
//		foIndicatorCalibration.push_back(sumRMSE/foIndicatorsRMSE.size());
//	}
