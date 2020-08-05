#include <vector>
#include "aig.h"
#include "qtypes.h"
#include "MiniSatExt.h"

void saveModQBF(const std::string& filename, std::vector<LitSet> clauses, Prefix p, int v, int numOfClauses);
