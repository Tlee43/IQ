//this code was copied from the QFUN implementation by Mikol� Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  LitSet.cc
 * Author:  mikolas
 * Created on:  Wed Oct 12 15:43:36 CEDT 2011
 * Copyright (C) 2011, Mikolas Janota
 */
#include "LitSet.h"
#include "minisat/mtl/Sort.h"
using SATSPC::sort;
using SATSPC::LessThan_default;
using SATSPC::LSet;
#define CTOR(COLT,IXT) LitSet::LitSet(const COLT& lits) { \
  const auto vsz = lits.size(); \
  if (vsz==0) { \
    _literals=NULL; \
    _size=0; \
    _hash_code = EMPTY_HASH; \
    return; \
  } \
  _literals = new Lit[(size_t)vsz+1]; \
  _literals[0].x=1; \
  for (IXT i=0; i<vsz; ++i) _literals[(size_t)i+1]=lits[i]; \
  sort(_literals+1, vsz, LessThan_default<Lit>() ); \
  _size = (size_t)vsz; \
  size_t j=2; \
  Lit last = _literals[1]; \
  const size_t conv_sz = (size_t) vsz; \
  for (size_t i=2; i <= conv_sz; ++i) { \
    if (_literals[i] == last) continue; \
    _literals[j]=_literals[i]; \
    last=_literals[i]; \
    ++j; \
  } \
  _size=j-1; \
  _hash_code = 7; \
  for (size_t i=1; i <= _size; ++i) _hash_code = _hash_code*31 + toInt(_literals[i]); \
}

CTOR(LiteralVector,size_t)
CTOR(LSet,int)

bool LitSet::equal(const LitSet& other) const {
  if (other._size!=_size) { return false; }
  if (other._literals==_literals) return true;
  for (size_t i=1; i <= _size; ++i) if (_literals[i]!=other._literals[i]) return false;
  return true;
}

LitSet::~LitSet() {
  decrease();
}

ostream& LitSet::print(ostream& out) const {
  bool f=true;
  FOR_EACH(i,*this) {
    if (!f) out<<' ';
    else f=false;
    if (SATSPC::sign(*i)) out << '-';
    out<<SATSPC::var(*i);
  }
  return out;
}

ostream & operator << (ostream& outs, const LitSet& ls) { return ls.print(outs); }
