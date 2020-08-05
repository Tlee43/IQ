#include <vector>
#include <utility>
#include <random>
#include <math.h>
#include <chrono>
#include <iostream>
#include "CDL4.h"
#include "boolFunc.h"
#include "testCDL4.h"
using namespace std;


//this assumes x is a numOfVars bit int
inline vector<int> intToBitVec(int x, int numOfVars){
	vector<int> ret {};
	for (int i = 0; i<numOfVars; i++){
		ret.push_back(x & 1);
		x = x >> 1;
	}
	return ret;
}


//this populates the training and tests sets used to evaluate the performance
inline void createTrainingAndTestSet(BoolFunc& f, vector<pair<vector<int>,int>>& train,
		vector<pair<vector<int>,int>>& test, int numOfVars, int trainSize, int testSize){

	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	int numOfinputs = pow(2, numOfVars);
	uniform_int_distribution<int> dis(0, numOfinputs);
	vector<bool> availible = vector<bool>(numOfinputs, true);

	while (train.size() < trainSize){
		int i = dis(gen);
		if (availible[i]){
			availible[i] = false;
			vector<int> x = intToBitVec(i, numOfVars);
			train.push_back(pair<vector<int>,int>(x, f.eval(x)));
		}
	}

	while (test.size() < testSize){
		int i = dis(gen);
		if (availible[i]){
		    availible[i] = false;
			vector<int> x = intToBitVec(i, numOfVars);
			test.push_back(pair<vector<int>,int>(x, f.eval(x)));
		}
	}

}


//this function finds if the sample index, i, matches one of the indexes we record the data of
int inDataPointSample(int i){
	switch (i){
	    case 9: return 0;
	    case 49: return 1;
	    case 99: return 2;
	    case 199: return 3;
	    case 499: return 4;
	    case 999: return 5;
	    case 1999: return 6;
	    case 4999: return 7;
	    default: return -1;
	}
}


//this class contains all the performance metrics used to evaulate the performance of the DL
class DLdata{
    public:
	    DLdata(DLMetric met, double accuracy, double exetime): m {met.length, met.averageClauseLength, met.maxClauseLength},
			    acc {accuracy}, time {exetime} {}
	    DLMetric m;
	    double acc;
	    double time;
};


//this function test the performance of my implementation of CDL4
void testCDL4Imp(){
	//this is how many boolean formulas to sample
	int sampleSize = 2000;
	int numOfVars = 16;
	int averageFuncClauseSize = 8;
	int trainSize = 5000;
	int testSize = 1000;

	//this contains the collected data
	vector<vector<DLdata>*> data {};

	//this calculates for each sampled boolean function the performance of the DL
	for (int i = 0; i < sampleSize; i++){

		cout << i << endl;

		//this creates a random boolean function of 16 vars and an average clause size of 8
		BoolFunc f {numOfVars, averageFuncClauseSize};

		//this vector holds the data record about the decision list for analysis
		vector<DLdata>* functionData = new vector<DLdata>();

		//this creates the training and test set used to train the DL and evaluate its performace respectivly
		vector<pair<vector<int>,int>> trainingSet {};
		vector<pair<vector<int>,int>> testSet {};
		createTrainingAndTestSet(f, trainingSet, testSet, numOfVars, trainSize, testSize);

		//this is the decision list with length minimisation optimisations which we evaluate the performance of
		DL dl_unOpt = DL();
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		chrono::steady_clock::time_point end;
		for (int j = 0; j < trainSize; j++){
			pair<vector<int>, int> x = trainingSet[j];
			dl_unOpt.learn(x.first, x.second, false);
			int index = inDataPointSample(j);
			if (index >= 0){
				end = chrono::steady_clock::now();

				//calculate the metrics of the decision list
				DLMetric m = dl_unOpt.get_metrics();

				//calculate the accuracy of the DL
				double acc = 0.0;
				for(pair<vector<int>, int> y : testSet){
					if(dl_unOpt.eval(y.first) == y.second){
						acc += 1.0;
					}
				}
				acc /= testSet.size();

				//Calculate the time between samples in miliseconds
				double time = chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/10000.0;

				functionData->push_back(DLdata(m, acc, time));
				begin = chrono::steady_clock::now();
			}
		}

		data.push_back(functionData);
	}

	cout << endl << endl << "---------------------------------------" << endl;
	cout << "--------------------------------------------" << endl;
	cout << "results: " << endl << endl << endl;

	//these store data about clauses
	vector<double> averageClauseLength = vector<double>(8, 0.0);
	vector<double> averageACL = vector<double>(8, 0.0);
	vector<double> averageMaxClauseLength = vector<double>(8, 0.0);
	vector<double> stddivClauseLength = vector<double>(8, 0.0);
	vector<double> stddivMaxClauseLength = vector<double>(8, 0.0);
	vector<double> stddivACL = vector<double>(8, 0.0);

	//these store data about predictive performance
	vector<double> averageAcc = vector<double>(8, 0.0);
	vector<double> stddivAcc = vector<double>(8, 0.0);

	//these store data about time performance
	vector<double> averageTime = vector<double>(8, 0.0);
	vector<double> stddivTime = vector<double>(8, 0.0);

	//after calculating the performance data we work out the means
	for(vector<DLdata>* d : data){
		for (int i = 0; i < averageACL.size(); i++){
			averageClauseLength[i] += (*d)[i].m.length;
			averageACL[i] += (*d)[i].m.averageClauseLength;
			averageMaxClauseLength[i] += (*d)[i].m.maxClauseLength;
			averageAcc[i] += (*d)[i].acc;
			averageTime[i] += (*d)[i].time;
		}
	}

	for (int i = 0; i < averageACL.size(); i++){
		averageClauseLength[i] /= sampleSize;
		averageACL[i] /= sampleSize;
		averageMaxClauseLength[i] /= sampleSize;
		averageAcc[i] /= sampleSize;
		averageTime[i] /= sampleSize;
	}

	//next we calculate the standard deviations

	for(vector<DLdata>* d : data){
		for (int i = 0; i < averageACL.size(); i++){
			stddivClauseLength[i] += pow((*d)[i].m.length - averageClauseLength[i], 2);
			stddivACL[i] += pow((*d)[i].m.averageClauseLength-averageACL[i], 2);
			stddivMaxClauseLength[i] += pow((*d)[i].m.maxClauseLength-averageMaxClauseLength[i], 2);
			stddivAcc[i] += pow((*d)[i].acc-averageAcc[i],2);
			stddivTime[i] += pow((*d)[i].time-averageTime[i], 2);
		}
	}

	for (int i = 0; i < averageACL.size(); i++){
		stddivClauseLength[i] = sqrt(stddivClauseLength[i]/(sampleSize-1));
		stddivACL[i] = sqrt(stddivACL[i]/(sampleSize-1));
		stddivMaxClauseLength[i] = sqrt(stddivMaxClauseLength[i]/(sampleSize-1));
		stddivAcc[i] = sqrt(stddivAcc[i]/(sampleSize-1));
		stddivTime[i] = sqrt(stddivTime[i]/(sampleSize-1));
	}

	//finally we print out the results

	cout << "number of clauses/rules, avrg " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << averageClauseLength[i] << ", ";
	}
	cout << endl;
	cout << "std dev: " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << stddivClauseLength[i] << ", ";
	}
	cout << endl;
	cout << "acl, avrg " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << averageACL[i] << ", ";
	}
	cout << endl;
	cout << "std dev: " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << stddivACL[i] << ", ";
	}
	cout << endl;
	cout << "max clause length, avrg " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << averageMaxClauseLength[i] << ", ";
	}
	cout << endl;
	cout << "std dev: " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << stddivMaxClauseLength[i] << ", ";
	}
	cout << endl;
	cout << "acc, avrg " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << averageAcc[i] << ", ";
	}
	cout << endl;
	cout << "std dev: " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << stddivAcc[i] << ", ";
	}
	cout << endl;
	cout << "time, avrg " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << averageTime[i] << ", ";
	}
	cout << endl;
	cout << "std dev: " << endl;
	for (int i = 0; i < averageACL.size(); i++){
		cout << stddivTime[i] << ", ";
	}
	cout << endl;

}




