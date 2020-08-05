//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  aig2cnf.h
 * Author:  mikolas
 * Created on:  15 Jan 2017 16:27:59
 * Copyright (C) 2017, Mikolas Janota
 */
#ifndef AIG2CNF_H_6496
#define AIG2CNF_H_6496
#include "minisat_auxiliary.h"
#include "aig.h"
#include "VarManager.h"
#include "qtypes.h"
class aig2cnf {
  public:
    aig2cnf(VarManager& vm, Level ql, AigFactory& factory, Minisat::MiniSatExt& _sat_solver);
    inline void alloc_vars(Var _max_id) {
        if (max_id > _max_id) return;
        max_id = _max_id;
        sat_solver.new_variables(max_id);
    }

    inline Var nextFree(){
    	return max_id+1;
    }

    inline Var fresh() {
      const auto retv = sat_solver.newVar();
      assert(max_id < retv);
      max_id = retv;
      return retv;
    }
    inline Lit operator() (const AigLit l) { return encode(l); }
  private:
    VarManager& var_mng;
    Level qlevel;
    AigFactory& factory;
    Minisat::MiniSatExt& sat_solver;
    std::unordered_map<size_t,Lit> representatives;
    Var max_id;
    const Lit true_lit;
    Lit encode(const AigLit l) {
      //factory.print(std::cerr <<"encoding:", l, 2)<<std::endl;
      if(factory.is_true(l)) return true_lit;
      if(factory.is_false(l)) return ~true_lit;
      if (l.is_var()) {
          if (!(l.var() && l.var() <= max_id)){
        	  std::cerr<<"v:"<<l.var()<< " lv:" << qlevel << std::endl;
          }
          assert(l.var() && l.var() <= max_id);
          return mkLit(l.var(), l.sign());
      }
      return encode_node_lit(l);
    }

    inline Lit encode_node_lit(AigLit l) {
      assert(l.is_node());
      const auto ptr = l.ptr();
      const AigNode node = factory.get_node(ptr);
      const auto i=representatives.find(ptr); //memoize check
      const bool is_new = i == representatives.end();
      const Lit node_enc = is_new ? encode_and(node.a(), node.b()) : i->second;
	  //insert_chk adds the aig to lit converstion to representatives 
      if (is_new) insert_chk(representatives,i,ptr,node_enc);
      return l.sign() ? ~node_enc : node_enc;
    }
	
	//this creates a new var thats true iff a and b is true.
    inline Lit encode_and(AigLit a, AigLit b) {
        const auto ae = encode(a);
        const auto be = encode(b);
        const auto retv = mkLit(fresh());//representative
        sat_solver.addClause(~ae,~be,retv);
        sat_solver.addClause(be,~retv);
        sat_solver.addClause(ae,~retv);
        return retv;
    }
};
#endif /* AIG2CNF_H_6496 */
