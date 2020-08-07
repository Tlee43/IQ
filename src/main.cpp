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

AigFactory factory;
void read_qcir(string filename);
extern int qcir_line;
extern QAigFla qcir_qfla;
extern unordered_map<const char*,int,cstrHash,cstrEq> name2var;


std::set<int> getVars(AigLit l){
	if(factory.is_true(l)) return std::set<int>();
	if(factory.is_false(l)) return std::set<int>();
	if (l.is_var()) return std::set<int> {l.var()};
	const auto ptr = l.ptr();
	const AigNode node = factory.get_node(ptr);
	std::set<int> combinedVars;
	std::set<int> left = getVars(node.a());
	std::set<int> right = getVars(node.b());
	for (int v : left){
		combinedVars.insert(v);
	}
	for (int v: right){
			combinedVars.insert(v);
		}
	return combinedVars;
}


inline pair<Prefix, AigLit> run_cnf(string filename){
    return loadQBFFromFile(filename, factory);
}


//this function makes sure the quantifiers alternate properly -- I think
void normalize_prefix(Prefix& p) {
    if (p.empty()) {
        p.push_back(Quantification(EXISTENTIAL, VarVector()));
        return;
    }
    size_t last = 0;// maitain the first quantifier
    for (size_t i = 1; i < p.size(); ++i) {
        auto& vs_i = p[i].second;
        if (vs_i.empty()) continue;
        if (p[i].first == p[last].first) {
            auto& vs_last = p[last].second;
            vs_last.insert(vs_last.end(), vs_i.begin(), vs_i.end());
        } else {
            ++last;
            if (last != i) p[last] = p[i];
        }
    }
    size_t new_sz = last+1;
    p.resize(new_sz);
}


inline pair<Prefix, AigLit> run_qcir(string filename){
	read_qcir(filename);
	normalize_prefix(qcir_qfla.pref);
	return pair<Prefix, AigLit>(qcir_qfla.pref, qcir_qfla.matrix);
	//factory.print(cerr<<"read:\n", qcir_qfla.matrix)<<endl;
	//ps=new QSolver(qcir_qfla,options,factory);
}


int main(int argc, char** argv) {

    if (argc != 2){
    	cout << "you must give only the filename of the QBF to be solved" << endl;
    	return 0;
    }

    string filename = argv[1];

    pair<Prefix, AigLit> p;

    if (ends_with(filename, ".qcnf") || ends_with(filename, ".qdimacs")) {
           p = run_cnf(filename);
    } else {
           p = run_qcir(filename);
    }

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


