#include <vector>
#include <map>
#include <utility>
#include "aig.h"
#include "CDL4.h"
using namespace std;

//return wether the input,x, matched the rule and if it did return also what the decision is
pair<int, int>  Rule::match(const vector<int>& x){
	for (pair<int,int> i : terms){
		if (x[i.first] != i.second){
			return pair<int, int> (0,-1);
		}
	}
	return pair<int, int> (1, decision);
}


//this adds x to the rule y, i.e "x and y"
inline vector<pair<int,int>> conjoin(pair<int,int> x, const vector<pair<int, int>>& y){
	vector<pair<int,int>> ret (y);
	ret.push_back(x);
	return ret;
}


//calculates how many examples in E do not match the ith value of x
int calc_score(int i, const vector<int>& x, const vector<vector<int>*>& E){
	int score = 0;
	for (vector<int>* y : E){
		if (x[i] != (*y)[i])
			score++;
	}
	return score;
}


//finds the max value of a int vector
inline int max(const vector<int>& x){
	int max = 0;
	int j = 0;
	for(int i = 0; i < x.size(); i++){
		if (x[i] > max){
			max = x[i];
			j = i;
		}
	}
	return j;
}


//finds if for a given example, y, one of the rules, in ds, is true for that example, 
//returning the index of the rule or -1 if no rule is true for the example 
inline int covered(const vector<int>& ds, const vector<int>& x, const vector<int>& y){
	for (int i = 0; i < ds.size(); i++){
		if (x[ds[i]] != y[ds[i]])
			return i;
	}
	return -1;
}


inline int negateLit(int x){
	if (x == 1)
		return 0;
	return 1; //could you shorten this to return x != 1?
}


//this is just like the other learn except starts from the ith rule, 
//i.e it learns for the decision list starting at the ith rule
int DL::learn(const vector<int>& x, int y, int i){
	
	//this finds the next rule that matches the new example, x, starting from the ith rule
	pair<int,int> m = dl[i]->match(x);
	while (m.first == 0){
		i++;
		m = dl[i]->match(x);
	}
	
	//if the matched rule give the correct value for y then you don't 
	//need to do any learning so return (with a 0 signaling no learning has taken place)
	if (m.second == y){
		seenData[i]->push_back(new vector<int> (x));
		return 0;
	}
	
	//find discriminators vec ints ds and covering set es, alg used is greedy but fast perhaps could improve
	
	vector<vector<int>*>& E = *seenData[i]; 
	vector<int> score {};
	vector<int> remain {};
	for (int j = 0; j < x.size(); j++){
		score.push_back(calc_score(j, x, E));
		remain.push_back(j);
	}
	
	//calculating the set of new rules, ds and the examples that are covered by each rule
	
	int r = max(score);
	vector<int> ds {r};
	vector<vector<vector<int>*>*> es {};
	es.push_back(new vector<vector<int>*>());
	remain.erase(remain.begin()+r);
	for (vector<int>* v : E){
		int c = covered(ds, x, *v);
		if (c == -1){
			int maxScore = 0;
			int index = 0;
			for (int j = 0; j < remain.size(); j++){
				if (x[remain[j]] != (*v)[remain[j]] && maxScore < score[remain[j]]){
					maxScore = score[remain[j]];
					index = j;
				}
			}
			ds.push_back(remain[index]);
			remain.erase(remain.begin()+index);
			es.push_back(new vector<vector<int>*> {v});
		}else{
			es[c]->push_back(v);
		}
	}
	
	//inserting new rules and example sets into the decision list 
	//and deleting the old rule and example set
		
	for (int j = ds.size()-1; j >=0; j--){
		dl.insert(dl.begin()+i,
				new Rule (conjoin(pair<int,int> (ds[j], negateLit(x[ds[j]])), dl[i]->get_rule()), m.second));
		seenData.insert(seenData.begin()+i, es[j]);
	}
	delete dl[i+ds.size()];
	dl.erase(dl.begin()+i+ds.size());
	delete &E;
	seenData.erase(seenData.begin()+i+ds.size());
	int next = i+ds.size();
	if (next == dl.size()){
		dl.push_back(new Rule (vector<pair<int,int>> (), y));
		seenData.push_back(new vector<vector<int>*> {new vector<int>(x)});
		return 1;
	}
	learn(x,y,next); //maybe put "this->" infront?, and/or remove tail recursion and just use a loop?
	
	return 1;
}


//this is just like the other learn except starts from the ith rule,
//i.e it learns for the decision list starting at the ith rule
//Also it does some simplifications to the DL when you add new rules
int DL::learn_opt(const vector<int>& x, int y, int i){

	//this finds the next rule that matches the new example, x, starting from the ith rule
	pair<int,int> m = dl[i]->match(x);
	while (m.first == 0){
		i++;
		m = dl[i]->match(x);
	}

	//if the matched rule give the correct value for y then you don't
	//need to do any learning so return (with a 0 signaling no learning has taken place)
	if (m.second == y){
		seenData[i]->push_back(new vector<int> (x));
		return 0;
	}

	//find discriminators vec ints ds and covering set es, alg used is greedy but fast perhaps could improve

	vector<vector<int>*>& E = *seenData[i];
	vector<int> score {};
	vector<int> remain {};
	for (int j = 0; j < x.size(); j++){
		score.push_back(calc_score(j, x, E));
		remain.push_back(j);
	}

	//calculating the set of new rules, ds and the examples that are covered by each rule

	int r = max(score);
	vector<int> ds {r};
	vector<vector<vector<int>*>*> es {};
	es.push_back(new vector<vector<int>*>());
	remain.erase(remain.begin()+r);
	for (vector<int>* v : E){
		int c = covered(ds, x, *v);
		if (c == -1){
			int maxScore = 0;
			int index = 0;
			for (int j = 0; j < remain.size(); j++){
				if (x[remain[j]] != (*v)[remain[j]] && maxScore < score[remain[j]]){
					maxScore = score[remain[j]];
					index = j;
				}
			}
			ds.push_back(remain[index]);
			remain.erase(remain.begin()+index);
			es.push_back(new vector<vector<int>*> {v});
		}else{
			es[c]->push_back(v);
		}
	}

	//inserting new rules and example sets into the decision list
	//and deleting the old rule and example set

	for (int j = ds.size()-1; j >=0; j--){
		dl.insert(dl.begin()+i,
				new Rule (conjoin(pair<int,int> (ds[j], negateLit(x[ds[j]])), dl[i]->get_rule()), m.second));
		seenData.insert(seenData.begin()+i, es[j]);
	}
	delete dl[i+ds.size()];
	dl.erase(dl.begin()+i+ds.size());
	delete &E;
	seenData.erase(seenData.begin()+i+ds.size());
	int next = i+ds.size();
	if (next == dl.size()){
		dl.push_back(new Rule (vector<pair<int,int>> (), y));
		seenData.push_back(new vector<vector<int>*> {new vector<int>(x)});
		return 1;
	}
	learn_opt(x,y,next); //maybe remove tail recursion and just use a loop?

	return 1;
}

		
//this method converts the decision list into an and-inverter graph
AigLit DL::toAig(AigFactory& factory, const VarVector& v, int i){
	if (i == dl.size()-1){
		return dl[i]->get_decision() ? factory.mk_true() : factory.mk_false();
	}

	const vector<pair<int, int>>& clause = dl[i]->get_rule();
	AigLit tree = factory.mk_var(v[clause[0].first], !clause[0].second);
	for(int j = 1; j < clause.size(); j++){
		tree = factory.mk_and(tree, factory.mk_var(v[clause[j].first], !clause[j].second));
	}

	return dl[i]->get_decision() ? factory.mk_or(tree, toAig(factory, v, i+1))
			                     : factory.mk_and(factory.neg(tree), toAig(factory, v, i+1));
}

//this returns the decision lists prediction of the function for the input x
int DL::eval(const std::vector<int>& x){
	//this finds the next rule that matches the new example, x, starting from the ith rule
	int i = 0;
	pair<int,int> m = dl[i]->match(x);
	while (m.first == 0){
		i++;
		m = dl[i]->match(x);
	}

	return m.second;
}

