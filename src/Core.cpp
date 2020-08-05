//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  Core.cpp
 * Author:  mikolas
 * Created on:  22 Mar 2017 11:24:36
 * Copyright (C) 2017, Mikolas Janota
 */
#include "Core.h"

const std::vector<Lit> Core::empty_lits;

Core::Core(const LSet& core, bool flip) {
    c = new Rec();
    c->ref_count = 1;
    for (int i = 0; i < core.size(); i++)
        c->insert(flip ? ~core[i] : core[i]);
}

Core::Core(const SATSPC::vec<SATSPC::lbool>& m, const vector<Var>& vs) {
    c = new Rec();
    c->ref_count = 1;
    for (Var v : vs) {
        const lbool val = eval(v, m);
        c->insert(val == l_True ? mkLit(v) : ~mkLit(v));
/*        if(val == l_False) c->insert(~mkLit(v));
        else if(val == l_True) c->insert(mkLit(v)); */
    }
}
