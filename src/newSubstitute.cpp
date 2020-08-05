#include "newSubstitute.h"
#include "aig.h"


bool constSubstitute::subs_node(const AigLit& t, AigLit& res){
	if (t.is_var()) {
        const auto v = t.var();
        const auto i = s.find(v);
        if (i != s.end()) {
            const auto& nval = (i->second == l_True) ? factory.mk_true() : factory.mk_false();
            res = t.sign() ? factory.neg(nval) : nval;
        } else {
            res = factory.mk_var(v, t.sign());
        }
        return true;
    }
    if (t.is_node()) {
        const auto ptr = t.ptr();
        const auto i = node_cache.find(ptr);
        if (i == node_cache.end()) return false;
        res = t.sign() ? factory.neg(i->second) : i->second;
        return true;
    }
    assert(factory.is_true(t) || factory.is_false(t));
    res = t;
    return true;
}


bool stratSubstitute::subs_node(const AigLit& t, AigLit& res){
	if (t.is_var()) {
        const auto v = t.var();
        const auto i = s.find(v);
        if (i != s.end()) {
            const auto& nval = i->second;
            res = t.sign() ? factory.neg(nval) : nval;
        } else {
        	const auto j = refresh.find(v);
        	if (j != refresh.end()){
        		const auto newv = j->second;
        		if (newv == 0){
        			res = factory.mk_var(nextFreeVar, t.sign());
        			j->second = nextFreeVar;
        			nextFreeVar++;
        		}else{
        			res = factory.mk_var(newv, t.sign());
        		}
        	}else{
                res = factory.mk_var(v, t.sign());
        	}
        }
        return true;
    }
    if (t.is_node()) {
        const auto ptr = t.ptr();
        const auto i = node_cache.find(ptr);
        if (i == node_cache.end()) return false;
        res = t.sign() ? factory.neg(i->second) : i->second;
        return true;
    }
    assert(factory.is_true(t) || factory.is_false(t));
    res = t;
    return true;
}


void stratSubstitute::mark_used(AigLit l, std::set<int>& vs) {
    std::unordered_set<size_t> seen;
    vector<AigLit> todo;
    AigLit ares, bres;
    todo.push_back(l);
    while (!todo.empty()) {
        const AigLit t = todo.back();
        todo.pop_back();
        //factory.print(std::cerr<<__LINE__<<endl, t)<<endl;
        if (t.is_var()) {
            const auto v = t.var();
            if (s.find(v) != s.end()) vs.insert(v);
        } else if (t.is_node()) {
            if (mark(seen, t.ptr())) {
                const auto n = factory.get_node(t.ptr());
                const auto ac = subs_node(n.a(), ares);
                const auto bc = subs_node(n.b(), bres);
                const bool af = factory.is_false(ares);
                const bool bf = factory.is_false(bres);
                if (af || !bf) todo.push_back(n.a());
                if (!af) todo.push_back(n.b());
            }
        }
    }
}

