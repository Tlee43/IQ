#include <vector>
#include <utility>

class BoolFunc{

public:

	//this constructor creates a random boolean function
	BoolFunc(int numOfVars, int a);

	BoolFunc(const std::vector<std::vector<std::pair<int,int>>*>& f): formula (f) {}
	~BoolFunc(){
		for(std::vector<std::pair<int,int>>* t : formula){
			delete t;
		}
	}

	//this returns the value of the boolean function for input x
	int eval(const std::vector<int>& x);


private:
    //this stores the formula in DNF form,
	//that is each element points to a term containing a disjunction of literals
	std::vector<std::vector<std::pair<int,int>>*> formula;

};

