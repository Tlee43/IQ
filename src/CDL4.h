#include <vector>
#include <map>
#include <utility>
#include "qtypes.h"
#include "aig.h"

//this class implments a single rule of a decision list
class Rule{
	public:
	    Rule(const std::vector<std::pair<int, int>>& t, int d): terms (t), decision {d} {}
		
		//return wether the input,x, matched the rule and if it did what the decision
		std::pair<int, int> match(const std::vector<int>& x);
		
		const std::vector<std::pair<int, int>>& get_rule(){
			return terms;
		}

		int get_decision(){
			return decision;
		}
	
	private:
	    //this represents the conjuction of literals 
		//(represented as an index and a value saying if the lit is negated or not) making up the rule
	    std::vector<std::pair<int, int>> terms;
		
		//if the rule is matched this is the decision it gives
		int decision;
		
};

class DLMetric {
	public:
	    DLMetric(int l, double acl, int mcl): length {l}, averageClauseLength {acl}, maxClauseLength {mcl} {}
	    int length;
	    double averageClauseLength;
	    int maxClauseLength;
};

//this class implements decision lists, where learn uses the CDL4 alg to update the list
class DL {
	public:
	    DL(): dl {new Rule(std::vector<std::pair<int,int>>(), 1)}, seenData {new std::vector<std::vector<int>*> {}} {}
		~DL(){
			for (Rule* r : dl)
				delete r;
			for (std::vector<std::vector<int>*>* v : seenData){
				for (std::vector<int>* u : *v){
					delete u;
				}
				delete v;
			}
		}

		//maybe add move methods as well
		
		//this method takes an input and its function rtuen value and updates
		//the DL using CDL4 to make the list lean this data point
		//the optimise param is used to say if you want to use DL rule minimisation optimisations or not
		//this method uses CDL4 to update the decision list
		inline int learn(const std::vector<int>& x, int y, bool optimise){
			if (optimise){
			    return learn_opt(x, y, 0);
			}
			return learn(x, y, 0);
		}

		//this returns the decision lists prediction of the function for the input x
		int eval(const std::vector<int>& x);

		//prints the metrics used in analysing the decision list implementation
		DLMetric get_metrics(){
			int length = dl.size();
			double averageClauseLength = 0.0;
			int maxClauseLength = 0;
			for(Rule* r : dl){
				int l = r->get_rule().size();
				averageClauseLength += l;
				if (maxClauseLength < l)
					maxClauseLength = l;
			}
			averageClauseLength = averageClauseLength/length;
			return DLMetric(length, averageClauseLength, maxClauseLength);
		}

		//this method converts the decision list into an and-inverter graph
		AigLit toAig(AigFactory& factory, const VarVector& v){
			return toAig(factory, v, 0);
		}

		int size(){
			return dl.size();
		}
	
	private:
	    
		//this contains all the rules making up the decision list 
	    std::vector<Rule*> dl;
		
		//this stores for each rule the previously seen x's that are covered by that rule
		std::vector<std::vector<std::vector<int>*>*> seenData;
		
		//this is just like the public learn except starts from the ith rule, 
		//i.e it learns for the decision list starting at the ith rule
		int learn(const std::vector<int>& x, int y, int i);

		//same as learn but with DL length minimisation optimisations
		int learn_opt(const std::vector<int>& x, int y, int i);

		//same as public toAig but you start at the ith rule
		AigLit toAig(AigFactory& factory, const VarVector& v, int i);

};
