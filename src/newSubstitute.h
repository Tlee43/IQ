#include "substitute.h"
#include <set>
#include <unordered_map>
#include "MiniSatExt.h"

//this class is used to substitute constant functions into a propositional formula
class constSubstitute : public SubstituteBase {
    public:
	    constSubstitute(AigFactory& factory,
	    		const std::unordered_map<Var,lbool>& s)
            : SubstituteBase(factory)
            , s(s)
            {}

    protected:
        const std::unordered_map<Var,lbool>& s;
        virtual bool subs_node(const AigLit& t, AigLit& res);

};

//this class is used to substitute skolem/herbrand functions into a propositional formula
class stratSubstitute : public SubstituteBase {
    public:
	    stratSubstitute(AigFactory& factory,
	    		const std::unordered_map<Var,AigLit>& s, std::unordered_map<Var,Var>& refresh, int nextFreeVar)
            : SubstituteBase(factory)
            , s(s)
            , refresh (refresh)
            , nextFreeVar (nextFreeVar)
            {}

	    void mark_used(AigLit l, std::set<int>& vs);

    protected:
        const std::unordered_map<Var,AigLit>& s;
        std::unordered_map<Var,Var>& refresh;
        int nextFreeVar;
        virtual bool subs_node(const AigLit& t, AigLit& res);


};


