#include <utility>
#include <string.h>
#include "aig.h"
#include "qtypes.h"


//this function takes a filename (.qcnf or .qdimacs) and returns the QBF formula stored in the file
std::pair<Prefix, AigLit> loadQBFFromFile(const std::string& filename, AigFactory& factory);

//this function takes a filename (.qcnf or .qdimacs) and returns the QBF formula stored in the file
//the int parameter numOfClauses makes the loader only uses the first 'numOfClauses' of the QBF formula
//so as to reduce problem complexity
std::pair<Prefix, AigLit> loadQBFFromFile(const std::string& filename, AigFactory& factory, int numOfClauses);

//this version of loadQBFFromFile returns a LitSet instead of the Aig representation
std::pair<Prefix, vector<LitSet>> loadQBFFromFileLitSet(const std::string& filename, AigFactory& factory, int numOfClauses);
