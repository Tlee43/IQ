#include <iostream>
#include <fstream>
#include "LitSet.h"
#include "qtypes.h"
#include "minisat_auxiliary.h"
using namespace std;


void saveModQBF(const string& filename, vector<LitSet> clauses, Prefix p, int v, int numOfClauses){

	ofstream outfile;
	outfile.open(filename);

	outfile << "p cnf " << v << " " << numOfClauses << endl;

	for (Quantification q : p){
		if (q.first == UNIVERSAL){
			outfile << "a";
		}else{
			outfile << "e";
		}
		for(int x : q.second){
			outfile << " " << x;
		}
		outfile << " 0" << endl;
	}

	for (LitSet c : clauses){
		for (LitSet::const_iterator i = c.begin(); i != c.end(); ++i){
			outfile << (SATSPC::sign(*i) ? "-" : "") << SATSPC::var(*i) << " ";
		}
		outfile << "0" << endl;
	}

	outfile.close();

}
