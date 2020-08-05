#include <chrono>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include "aig.h"
#include "CDL4.h"
#include "testCDL4.h"
#include "qtypes.h"
#include "MiniSatExt.h"
#include "Encoder.h"
#include "SatVarManager.h"
#include "LoadQBF.h"
#include "IQFUN.h"
#include "SaveQBF.h"
using namespace std;
using namespace SATSPC;


int main(int argc, char** argv) {

    if (argc != 2){
    	cout << "you must give only the filename of the QBF to be solved" << endl;
    	return 0;
    }

    string filename = argv[1];

	AigFactory factory;

	//cout << filename << endl;
    pair<Prefix, AigLit> p = loadQBFFromFile(filename, factory);

	int maxvar = 0;
	for (Quantification q : p.first){
	    for(int v : q.second){
		    if (v > maxvar){
			    maxvar = v;
		    }
	    }
	}

	nIQFUN* solver = new nIQFUN(factory, p.first, vector<AigLit>(1, p.second), 12, 64, maxvar, 4);


	//this is used to record how long the solver has been runing for
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	chrono::steady_clock::time_point next;

	bool result = p.first[0].first == EXISTENTIAL ? solver->win().first : !solver->win().first;
	next = chrono::steady_clock::now();
	cout << "[" << result << ", " << (chrono::duration_cast<std::chrono::seconds>(next - begin).count())
		 << ", " << solver->getNoOfRefinements() << ", " << solver->getNoOfStratRefinements() << "]" << endl;

	delete solver;

	//cout << "done" << endl;

}


