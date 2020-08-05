//this code was copied from the QFUN implementation by Mikol� Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  qtypes.cpp
 * Author:  mikolas
 * Created on:  Thu, Jul 16, 2015 7:22:12 PM
 * Copyright (C) 2015, Mikolas Janota
 */
#include "qtypes.h"
QFla robustify(const QFla& f) {
    QFla retv;
    retv.pref = f.pref;
    retv.matrix = f.matrix;
    if (retv.pref.empty() || retv.pref.back().first != EXISTENTIAL)
        retv.pref.push_back(make_pair(EXISTENTIAL, VarVector()));
    retv.pref.push_back(make_pair(UNIVERSAL, VarVector()));
    return retv;
}

