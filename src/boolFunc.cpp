
#include <vector>
#include <utility>
#include <math.h>
#include <random>
#include "boolFunc.h"
using namespace std;

//this constructor creates a random boolean function
//it is constructed in the same way as specified in
//"Learning of Boolean Functions Using Support Vector Machines"
//by Ken Sadohara
BoolFunc::BoolFunc(int numOfVars=16, int a=8){
	default_random_engine generator;
	bernoulli_distribution disjuntDistribution(static_cast<double>(a)/numOfVars);
	bernoulli_distribution negationDistribution(0.5);

	formula = vector<vector<pair<int,int>>*>();

	for(int i = 0; i<pow(2,a-2); i++){
		vector<pair<int,int>>* v = new vector<pair<int,int>>();
		for(int j = 0; j < numOfVars; j++){
			if (disjuntDistribution(generator)){
				v->push_back(pair<int, int>(j, negationDistribution(generator)));
			}
		}
		formula.push_back(v);
	}

}


//this returns the value of the boolean function for input x
int BoolFunc::eval(const std::vector<int>& x){
	for(vector<pair<int,int>>* t : formula){
		int covered = 0;
		for(pair<int,int> lit : *t){
			if (x[lit.first] == lit.second){
				covered = 1;
				break;
			}
		}
		if (covered == 0){
			return 0;
		}
	}
	return 1;
}






