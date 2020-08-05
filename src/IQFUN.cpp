#include "IQFUN.h"
#include <vector>
#include <set>
#include <chrono>
#include <utility>
#include <unordered_map>
#include "aig.h"
#include <math.h>
#include "qtypes.h"
#include "MiniSatExt.h"
#include "Encoder.h"
#include "SatVarManager.h"
#include "CDL4.h"
#include "substitute.h"
#include "newSubstitute.h"
#include "FreshVars.h"
using namespace std;
using namespace SATSPC;
using SATSPC::lbool;
using SATSPC::vec;
using std::max;


//this function frees the decisions list in a vector of decision list pointers
inline void free_DL(vector<DL*> dls){
	for (DL* dl : dls){
		delete dl;
	}
}

inline void free_solvers(vector<MiniSatExt*> solvers){
	for(MiniSatExt* solver : solvers){
		delete solver;
	}
}

//this function frees the decisions list in a vector of decision list pointers
inline void free_DL(unordered_map<int, vector<DL*>> dlss){
	for (pair<int, vector<DL*>> dls : dlss){
	    for (DL* dl : dls.second){
		    delete dl;
	    }
    }
}


//this function is used for debugiing and returns true if a given move is the same as the model returned by the solver s
inline bool match(const MiniSatExt& s, const vector<lbool>& move, const VarVector& v){
	for (int j = 0; j < v.size(); j++){
		if(move[j] != s.model[v[j]]){
			return false;
		}
	}
	return true;
}


inline void printResults(double time, int iterations, int noOfStratRefinememts, bool result){
	cout << "[" << result << ", " << time << ", " << iterations << ", " << noOfStratRefinememts << "]," << endl;
}



inline pair<bool, unordered_map<Var, AigLit>> wins1(AigFactory& factory, const Quantification& q, const vector<AigLit>& games){

	MiniSatExt candidateMoveSolver;
	SatVarManager candidateMovesvm = SatVarManager(candidateMoveSolver);
	Encoder candidateMoveEncoder(candidateMovesvm, 0, factory, candidateMoveSolver);
	candidateMoveEncoder.alloc_var(maxv(q.second));
	AigLit r = factory.mk_true();
	for (AigLit a : games){
		r = factory.mk_and(r, q.first == EXISTENTIAL ? a : a.neg());
	}
	candidateMoveEncoder.add(candidateMoveEncoder(r));

	unordered_map<Var, AigLit> t;
	if (candidateMoveSolver.solve()){
		for(int i = 0; i < q.second.size(); i++){
			t[q.second[i]] = (l_True == candidateMoveSolver.model[q.second[i]] ? factory.mk_true() : factory.mk_false());
		}
		return make_pair(true, t);
	}else{
		return make_pair(false, unordered_map<Var, AigLit>());
	}
}


//this is the function that decides whether to augment the abstraction using the counter move or the learnt Aig from our CLD4 learner
//The "hasLearntBound" argument is a hyperparameter, where it describes when the algorithm thinks the learnt DL's have learnt good
//enough strategys to be put into the abstraction.
inline bool shouldLearn(const vector<int>& counts, int vars, int hasLearntBound){
	//this should be modified until a good heuristic is found
	int acc = 0;
	for(int count : counts){
		acc += (count >= hasLearntBound);
	}
	return acc > vars*0.8;
}


//this function implements the IQFUN algorithm
pair<bool, unordered_map<Var, AigLit>> nIQFUN::win(){

	//if all the games are propositional just solve with a single call to sat
	if (prefix.size() == 1){
		return wins1(factory, prefix[0], games);
    }


	//this code initialises the abstraction and variables used to decide when to put the DL formula into the abstraction
    int vars = prefix[1].second.size();

    unordered_map<int, vector<int>> learntCounts;
    for (int i = 0; i < games.size(); i++){
    	learntCounts[i] = vector<int>(vars, 0);
    }


    //this is only used for 2QBFs, ugly code to put it here so when cleaning up make this fit better
    vector<MiniSatExt*> counterMoveSolvers;
    if (prefix.size() == 2){
    	for (int i = 0; i < games.size(); i++){
    		//this code creates the solver which finds counter moves given a wining move in the abstraction
    		MiniSatExt* counterMoveSolver = new MiniSatExt();
    		SatVarManager counterMovesvm(*counterMoveSolver);
    		Encoder counterMoveEncoder(counterMovesvm, 0, factory, *counterMoveSolver);
    		counterMoveEncoder.alloc_var(nextFreeVar-1); //should this be originalMaxVar?
    		counterMoveEncoder.add(counterMoveEncoder(prefix[0].first == UNIVERSAL ? games[i] : factory.neg(games[i])));
    		counterMoveSolvers.push_back(counterMoveSolver);
     	}
    }

	//this code creates the decision lists used for learning strategies
	unordered_map<int, vector<DL*>> dls;
	for (int i = 0; i < games.size(); i++){
		vector<DL*> stratLearner;
		for (int j = 0; j < prefix[1].second.size(); j++){
		    stratLearner.push_back(new DL());
	    }
		dls[i] = stratLearner;
	}

	int mavvarofCounter = 0;
	for (int i = 1; i < prefix.size(); i++){
		int tempv = maxv(prefix[i].second);
		if (tempv > mavvarofCounter){
			mavvarofCounter = tempv;
		}
	}

	IQFUN* abstraction_solver;
	if (prefix.size() > 3){
		int maxvar = maxv(prefix[0].second);
		Prefix abs_prefix (1, prefix[0]);
		for (int i = 3; i < prefix.size(); i++){
			abs_prefix.push_back(prefix[i]);
			int tempv = maxv(prefix[i].second);
			if (tempv > maxvar){
				maxvar = tempv;
			}
		}
		abstraction_solver = new nIQFUN(factory, abs_prefix, vector<AigLit>(1, (prefix[0].first == EXISTENTIAL ? factory.mk_true() : factory.mk_false())),
				refreshFrequency, hasLearntBound, maxvar); //to save compute time maxvar could be nextFreeVar-1 but this could lead to worse general performance
	}else{
		abstraction_solver = new BaseIQFUN(factory, vector<Quantification>(1, prefix[0]),
				vector<AigLit>(1, (prefix[0].first == EXISTENTIAL ? factory.mk_true() : factory.mk_false())), refreshFrequency, hasLearntBound, maxv(prefix[0].second));
	}


	noOfRefinements = 1;
	noOfStratRefinements = 0;
	double time;

	//this is the main loop of the iQFUN algorithm
	while(1){

		//this is the stage in the loop where it calculates the next set of moves, returning if one of the players has won.

		//must reset the nextFreeVar of the abstraction solver on each solve
		pair<bool, unordered_map<Var, AigLit>> unFilteredCandidate = abstraction_solver->win();
		if (!unFilteredCandidate.first){
			delete abstraction_solver;
			free_DL(dls);
			free_solvers(counterMoveSolvers);
			return pair<bool, unordered_map<Var, AigLit>>(false, unordered_map<Var, AigLit>());
		}

		unordered_map<Var, AigLit> candidate;
		for (int i = 0; i < prefix[0].second.size(); i++){
			candidate[prefix[0].second[i]] = unFilteredCandidate.second[prefix[0].second[i]];
		}

		bool noCounterMove = true;
		unordered_map<Var, AigLit> counter;
		std::set<int> used;
		for(int l = 0; l < games.size(); l++){

			if(calc_counterMove(candidate, counter, games[l], (prefix.size() == 2 ? counterMoveSolvers[l] : nullptr), used, mavvarofCounter)){

				//do blocking, probs should pt this in another function
				if (blocking <= prefix.size()){
				    AigLit blockclause = factory.mk_false();
				    for(int v : prefix[0].second){
					    if(used.find(v) != used.end()){
						    blockclause = factory.mk_or(blockclause, (candidate[v].is_true() ? factory.mk_var(v, true) : factory.mk_var(v, false)));
					    }
				    }
				    abstraction_solver->addGame(blockclause, abstraction_solver->getNextFreeVar()-1);
				}

				noCounterMove = false;


        		//this is the learning stage of the loop where we update our decision list using the CDL4 algorithm
        		for (int i = 0; i < prefix[1].second.size(); i++){
        			bool y = counter[prefix[1].second[i]].is_true();
        			vector<int> x = vector<int>();
        			for (int j = 0; j < prefix[0].second.size(); j++){
        				x.push_back(candidate[prefix[0].second[j]].is_true());
        			}
        			//this is a differnt 'should learn procedure counting if the dl has learnt a good strat
        			int hasLearnt = dls[l][i]->learn(x, y, false); //this updates the DL with the new data point and increments learntCount if the DL is modified
        			if (learntCounts[l][i] && hasLearnt){
    			        learntCounts[l][i] = 0;
       			    }else{
    			       learntCounts[l][i] += !hasLearnt;
	    		    }
	    	    }

        		unordered_map<Var,Var> refresh;
        		if (prefix.size() > 2){
        		    for(int i = 0; i < prefix[2].second.size(); i++){
        		    	refresh[prefix[2].second[i]] = abstraction_solver->getNextFreeVar();
        		    	abstraction_solver->addOuterVar(abstraction_solver->getNextFreeVar());
        		    }
        	    }


    	    	//this is the update stage of the loop where we update the abstraction with the counter move or our learnt decision list
    	    	if (shouldLearn(learntCounts[l], vars, hasLearntBound)){ //update the abstraction using the learnt decision list

     	    		// we reset the learn counters each time we insert an learnt strategy into the abstraction (i.e add to the candidate solver the clauses of the current learnt strategy)
    	    		for (int i = 0; i < prefix[1].second.size(); i++){
    	     			learntCounts[l][i] = 0;
    	     		}

	        		noOfStratRefinements++;

    	    		//this code creates the substitution for the inserting the learnt strategy's into the matrix (i.e the propositional bit of a QBF formula in prenex form) of the QBF formula we are solving
    	    		unordered_map<Var,AigLit> s {};
     	    		for (int i = 0; i < prefix[1].second.size(); i++){
    	    			s.insert({prefix[1].second[i], dls[l][i]->toAig(factory, prefix[0].second)});
    	    		}


    		    	unique_ptr<stratSubstitute> sub(new stratSubstitute(factory, s, refresh, abstraction_solver->getNextFreeVar()));


    		    	abstraction_solver->addGame((*sub)(games[l]), abstraction_solver->getNextFreeVar()-1);



    	    		//this deletes DLs if they become to large
    	    		for (int i = 0; i < prefix[1].second.size(); i++){
    	    			if (dls[l][i]->size() > vars/refreshFrequency){
    	    				delete dls[l][i];
    	     			    dls[l][i] = new DL();
    		   		    }
    		    	}


    		    }else{ //update abstraction using just the counter move

    	    		unique_ptr<stratSubstitute> sub(new stratSubstitute(factory, counter, refresh,  abstraction_solver->getNextFreeVar()));

    	    		abstraction_solver->addGame((*sub)(games[l]), abstraction_solver->getNextFreeVar()-1);
     	    	}

    	    	break;

    		}
		}

		if (noCounterMove){
			delete abstraction_solver;
			free_DL(dls);
			free_solvers(counterMoveSolvers);
			return make_pair(true, candidate);
		}

		noOfRefinements++;

	}

}


inline bool nIQFUN::calc_counterMove(unordered_map<Var, AigLit>& candidate, unordered_map<Var, AigLit>& counter, const AigLit& game, MiniSatExt* counterMoveSolver, std::set<int>& used, int maxvar){

	if (prefix.size() == 2){

	    //the vector a stores the candidate move
	    vec<Lit> a;
	    for(int i = 0; i < prefix[0].second.size(); i++){
		    a.push(candidate[prefix[0].second[i]].is_true() ? mkLit(prefix[0].second[i]) : ~mkLit(prefix[0].second[i]));
	    }

	    //counterMoveSolver.solve(a) calculates the next counter move, if there is one
	    if (!counterMoveSolver->solve(a)){
	    	return false;
	    }else{
	     	for(int i = 0; i < prefix[1].second.size(); i++){
	     		counter[prefix[1].second[i]] = (l_True == counterMoveSolver->model[prefix[1].second[i]] ? factory.mk_true() : factory.mk_false());
	     	}
	     	return true;
	    }

	}else{

       	unordered_map<Var,Var> refresh;
		unique_ptr<stratSubstitute> sub(new stratSubstitute(factory, candidate, refresh, nextFreeVar));
		Prefix InnerQuantifiers = Prefix(prefix.begin()+1, prefix.end());
		IQFUN* counterMoveSolver = new nIQFUN(factory, InnerQuantifiers, vector<AigLit>(1, ((*sub)(game))), refreshFrequency, hasLearntBound, maxvar);
		pair<bool, unordered_map<Var, AigLit>> solution = counterMoveSolver->win();
		delete counterMoveSolver;

		if (solution.first && blocking <= prefix.size()){
			sub->mark_used(game, used);
		}

		counter = solution.second;
		return solution.first;
	}

}




