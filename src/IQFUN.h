#include <vector>
#include <utility>
#include <set>
#include <unordered_map>
#include "aig.h"
#include "qtypes.h"
#include "MiniSatExt.h"
#include "Encoder.h"
#include "SatVarManager.h"

class IQFUN {

    public:

	    IQFUN(AigFactory& factory, Prefix p, int refreshFrequency = 12, int hasLearntBound = 64, Var maxvar = 0):
	    	factory (factory), prefix (p), refreshFrequency (refreshFrequency), hasLearntBound (hasLearntBound),
			nextFreeVar (maxvar+1), originalMaxVar (maxvar) {}

	    bool solve(){
	    	return prefix[0].first == EXISTENTIAL ? win().first : !win().first;
	    }

	    virtual std::pair<bool, std::unordered_map<Var, AigLit>> win() = 0;

	    virtual void addGame(AigLit matrix, int maxvar) = 0;

	    void addVars(int numOfVars){
	    	nextFreeVar = nextFreeVar+numOfVars;
	    }

	    void setNextFreeVar(int maxvar){
	    	nextFreeVar = maxvar;
	    }

	    virtual void addOuterVar(int var){
	    	if (nextFreeVar <= var) {
	    		nextFreeVar = var + 1;
	    	}//else throw exception -- var already assigned
	    	prefix[0].second.push_back(var);
	    }

	    Var getNextFreeVar(){
	    	return nextFreeVar;
	    }

	    virtual ~IQFUN() {}


    protected:

	    Var originalMaxVar;

	    Var nextFreeVar; //this stores the max var value plus one of the games and is used to assign fresh vars when refining

	    //hyperparameters for when to refresh the learners and when to refine with a strategy

	    int refreshFrequency;

	    int hasLearntBound;

	    AigFactory& factory; //this stores the reference to the factory used to create all Aigs, which uses has consing and simplifications to improve performance

	    Prefix prefix; //this stores the prefix of the games

};


class nIQFUN : public IQFUN{

    public:

        nIQFUN(AigFactory& factory, Prefix p, std::vector<AigLit> games, int refreshFrequency = 12, int hasLearntBound = 64, Var maxvar = 0, int blocking = 4):
    	    IQFUN(factory, p, refreshFrequency, hasLearntBound, maxvar),  games (games), blocking (blocking) {}

	    std::pair<bool, std::unordered_map<Var, AigLit>> win();

	    //when adding a game through refinement must also remember to freshen the vars
	    void addGame (AigLit matrix, int maxvar){
	    	if (nextFreeVar <= maxvar){
	    		nextFreeVar = maxvar+1;
	    	}
	    	games.push_back(matrix);
	    }

	    int getNoOfRefinements(){
	    	return noOfRefinements;
	    }

	    int getNoOfStratRefinements(){
	    	return noOfStratRefinements;
	    }

	    virtual ~nIQFUN() {}


    private:

	    bool calc_counterMove(std::unordered_map<Var, AigLit>& candidate, std::unordered_map<Var, AigLit>& counter, const AigLit& game, SATSPC::MiniSatExt* counterMoveSolver, std::set<int>& used, int maxvar);

	    std::vector<AigLit> games; //this stores the matrices of each game

	    std::vector<AigLit> abs; //this stores the matrices of the QBFs in the abstraction

	    int noOfRefinements = 0;

	    int noOfStratRefinements = 0;

	    int blocking;

};


class  BaseIQFUN : public IQFUN{

    public:

	    BaseIQFUN(AigFactory& factory, Prefix p, std::vector<AigLit> games, int refreshFrequency = 12, int hasLearntBound = 64, Var maxvar = 0):
		   	IQFUN(factory, p, refreshFrequency, hasLearntBound, maxvar){

		   	//this code creates the solver which finds winning moves in the abstraction
	    	SatVarManager candidateMovesvm = SatVarManager(candidateMoveSolver);
		   	candidateMoveEncoder = std::make_unique<Encoder>(Encoder(candidateMovesvm, 0, factory, candidateMoveSolver));
		   	candidateMoveEncoder->alloc_var(maxvar);
		   	AigLit r = factory.mk_true();
		   	for (AigLit a : games){
		   		r = factory.mk_and(r, prefix[0].first == EXISTENTIAL ? a : a.neg());
		   	}
		   	candidateMoveEncoder->add((*candidateMoveEncoder)(r));
		}

	    std::pair<bool, std::unordered_map<Var, AigLit>> win(){
	    	std::unordered_map<Var, AigLit> t;
	    	if (candidateMoveSolver.solve()){
	    		for(int i = 0; i < prefix[0].second.size(); i++){
	    			t[prefix[0].second[i]] = (l_True == candidateMoveSolver.model[prefix[0].second[i]] ? factory.mk_true() : factory.mk_false());
	    	    }
	    	    return make_pair(true, t);
	    	}else{
	    		return make_pair(false, std::unordered_map<Var, AigLit>());
	    	}
	    }

	    void addGame(AigLit matrix, int maxvar){
	    	if (nextFreeVar <= maxvar){
	    		nextFreeVar = maxvar+1;
	    		candidateMoveEncoder->alloc_var(maxvar);
	    	}
	    	candidateMoveEncoder->add((*candidateMoveEncoder)(prefix[0].first == EXISTENTIAL ? matrix : matrix.neg()));
	    	nextFreeVar = candidateMoveEncoder->nextFree();
	    }

	    virtual void addOuterVar(int var){
	    	if (nextFreeVar <= var) {
	    		nextFreeVar = var + 1;
	    		candidateMoveEncoder->alloc_var(var);
	    	}//else throw exception -- var already assigned
	    	prefix[0].second.push_back(var);
	    }

	    void addVars(int numOfVars){
	    	  nextFreeVar = nextFreeVar+numOfVars;
	    	  candidateMoveEncoder->alloc_var(nextFreeVar-1);
	    }

	    virtual ~BaseIQFUN() {}


    private:

		SATSPC::MiniSatExt candidateMoveSolver;

		std::unique_ptr<Encoder> candidateMoveEncoder;


};
