//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  SatVarManager.h
 * Author:  mikolas
 * Created on:  16 Mar 2017 17:41:52
 * Copyright (C) 2017, Mikolas Janota
 */
#ifndef SATVARMANAGER_H_16536
#define SATVARMANAGER_H_16536
#include"MiniSatExt.h"
#include"VarManager.h"
class SatVarManager : public VarManager {
  public:
      SatVarManager(SATSPC::MiniSatExt& s) : s(s) {}
  private:
      SATSPC::MiniSatExt& s;
      virtual void alloc_var(Var v, Level ql) { s.new_variables(v); }
      virtual Var fresh_var(Level ql) { return s.newVar(); }
};
#endif /* SATVARMANAGER_H_16536 */
