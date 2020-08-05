//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  aig2cnf.cpp
 * Author:  mikolas
 * Created on:  15 Jan 2017 16:28:04
 * Copyright (C) 2017, Mikolas Janota
 */
#include"aig2cnf.h"
aig2cnf::aig2cnf(VarManager& vm, Level ql, AigFactory& factory,
        Minisat::MiniSatExt& _sat_solver)
    : var_mng(vm)
    , qlevel(ql)
    , factory(factory)
    , sat_solver(_sat_solver)
    , max_id(-1)
    , true_lit(mkLit(0))
{
   const Var tv = sat_solver.newVar();
   if (tv) assert(0);
   sat_solver.addClause(true_lit);
}
