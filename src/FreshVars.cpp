//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  FreshVars.cpp
 * Author:  mikolas
 * Created on:  08 Apr 2017 11:42:01
 * Copyright (C) 2017, Mikolas Janota
 */
#include"FreshVars.h"
#include"minisat_auxiliary.h"

std::ostream& operator << (std::ostream& o, const FreshVars& f) {
    const Var m = (Var)f.physical_sz();
    bool fst = true;
    o << '[';
    for (Var v = 0; v < m; ++v) {
        if (!f.is_marked(v)) continue;
        if (fst) fst = false;
        else o << ' ';
        o << v;
    }
    return o << ']';
}
