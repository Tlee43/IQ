#include <utility>
#include <stdlib.h>
#include <zlib.h>
#include "qtypes.h"
#include "Reader.h"
#include "ReadQ.h"
#include "ReadException.h"
#include "aig_util.h"
using namespace std;

//this function takes a filename (.qcnf or .qdimacs) and returns the QBF formula stored in the file
pair<Prefix, AigLit> loadQBFFromFile(const string& filename, AigFactory& factory){
	//some of this code was copied from the file main of the QFUN code
	//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/ by Mikoláš Janota

	scoped_ptr<Reader> fr;
	gzFile ff = gzopen(filename.c_str(), "rb");
	if (ff == Z_NULL) {
        cerr << "ERROR: " << "Unable to open file: " << filename << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
	fr.attach(new Reader(ff));
	ReadQ rq(*fr,false);
	try {
        rq.read();
    } catch (ReadException& rex) {
        cerr << "ERROR: " << rex.what() << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    //cout<<"c done reading: "<<read_cpu_time()<<std::endl;
    if (!rq.get_header_read()) {
        cerr << "ERROR: Missing header." << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    AigUtil au(factory);
    Prefix pref = rq.get_prefix();
    AigLit matrix = au.convert(rq.get_clauses());
    if(pref.size()==0) {
        pref.push_back(make_pair(EXISTENTIAL,VarVector()));
    }
    //if(pref.size()>2){
    //	cerr << "formula has more than two quantifiers" << endl;
    //	exit(100);
    //}
    return pair<Prefix, AigLit>(pref, matrix);
}


//this function takes a filename (.qcnf or .qdimacs) and returns the QBF formula stored in the file
//the int parameter numOfClauses makes the loader only uses the first 'numOfClauses' of the QBF formula
//so as to reduce problem complexity
pair<Prefix, AigLit> loadQBFFromFile(const string& filename, AigFactory& factory, int numOfClauses){
	//some of this code was copied from the file main of the QFUN code
	//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/ by Mikoláš Janota

	scoped_ptr<Reader> fr;
	gzFile ff = gzopen(filename.c_str(), "rb");
	if (ff == Z_NULL) {
        cerr << "ERROR: " << "Unable to open file: " << filename << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
	fr.attach(new Reader(ff));
	ReadQ rq(*fr,false);
	try {
        rq.read();
    } catch (ReadException& rex) {
        cerr << "ERROR: " << rex.what() << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    //cout<<"c done reading: "<<read_cpu_time()<<std::endl;
    if (!rq.get_header_read()) {
        cerr << "ERROR: Missing header." << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    AigUtil au(factory);
    Prefix pref = rq.get_prefix();
    vector<LitSet> clauses = rq.get_clauses();
    //cout << clauses.size() << endl;
    AigLit matrix = (clauses.size() < numOfClauses ? au.convert(clauses) : au.convert(vector<LitSet>(clauses.begin(), clauses.begin()+numOfClauses)));
    if(pref.size()==0) {
        pref.push_back(make_pair(EXISTENTIAL,VarVector()));
    }
    //if(pref.size()>2){
    //	cerr << "formula has more than two quantifiers" << endl;
    //	exit(100);
    //}
    return pair<Prefix, AigLit>(pref, matrix);
}


//this function takes a filename (.qcnf or .qdimacs) and returns the QBF formula stored in the file
//using a LitSet tor represent the matrix
pair<Prefix, vector<LitSet>> loadQBFFromFileLitSet(const string& filename, AigFactory& factory, int numOfClauses){
	//some of this code was copied from the file main of the QFUN code
	//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/ by Mikoláš Janota

	scoped_ptr<Reader> fr;
	gzFile ff = gzopen(filename.c_str(), "rb");
	if (ff == Z_NULL) {
        cerr << "ERROR: " << "Unable to open file: " << filename << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
	fr.attach(new Reader(ff));
	ReadQ rq(*fr,false);
	try {
        rq.read();
    } catch (ReadException& rex) {
        cerr << "ERROR: " << rex.what() << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    cout<<"c done reading: "<<read_cpu_time()<<std::endl;
    if (!rq.get_header_read()) {
        cerr << "ERROR: Missing header." << endl;
        cerr << "ABORTING" << endl;
        exit(100);
    }
    AigUtil au(factory);
    Prefix pref = rq.get_prefix();
    vector<LitSet> clauses = rq.get_clauses();
    if(pref.size()==0) {
        pref.push_back(make_pair(EXISTENTIAL,VarVector()));
    }
    //if(pref.size()>2){
    //	cerr << "formula has more than two quantifiers" << endl;
    //	exit(100);
    //}
    return pair<Prefix, vector<LitSet>>(pref, vector<LitSet>(clauses.begin(), clauses.begin()+numOfClauses));
}





