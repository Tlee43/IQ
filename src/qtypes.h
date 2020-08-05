//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:  qtypes.h
 * Author:  mikolas
 * Created on:  Fri, Aug 01, 2014 4:42:21 PM
 * Copyright (C) 2014, Mikolas Janota
 */
#ifndef QTYPES_HH_15436
#define QTYPES_HH_15436
#include <vector>
#include <unordered_map>
#include <ostream>
#include <vector>
#include "ImmutableVector.h"
#include "LitSet.h"
#include "aig.h"
using std::vector;
typedef vector<LitSet> CNF;
//typedef ImmutableVector<Var, std::hash<int> > VarVector;
typedef vector<Var> VarVector;
enum QuantifierType {UNIVERSAL=0, EXISTENTIAL=1};
typedef std::pair<QuantifierType, VarVector> Quantification;
typedef std::vector<Quantification> Prefix;
typedef int Level;
inline size_t index(Level ql) {
   assert(ql>=0);
   return (size_t) ql;
}

template<class T>
class LevelMap {
   public:
     LevelMap(size_t sz, const T& e) : v(sz,e) {}
     LevelMap(size_t sz) : v(sz) {}
     const T& operator[] (Level ql) const {
       const auto idx=index(ql);
       assert(idx<v.size());
       return v[idx];
     }
     T& operator[] (Level ql) {
       const auto idx=index(ql);
       assert(idx<v.size());
       T& val=v[idx];
       return val;
     }
   private:
     std::vector<T> v;
};


inline QuantifierType opponent(QuantifierType qt) {
  assert(qt==UNIVERSAL||qt==EXISTENTIAL);
  return (qt==UNIVERSAL) ? EXISTENTIAL : UNIVERSAL;
}

typedef struct {
  Prefix pref;
  CNF matrix;
} QFla;

typedef struct {
    Prefix pref;
    AigLit matrix;
} QAigFla;

inline bool lev_cmp(const std::pair<Lit,Level>& a,
                    const std::pair<Lit,Level>& b) {
  return a.second<b.second;}

inline QuantifierType neg(const QuantifierType& qt) {return qt==EXISTENTIAL?UNIVERSAL:EXISTENTIAL;}
QFla neg(const QFla& f);

QFla robustify(const QFla& f);

namespace std {
  inline std::ostream& operator << (std::ostream& outs, QuantifierType qt) {
    return outs<<(qt==UNIVERSAL?"A":"E");
  }

  inline std::ostream& operator << (std::ostream& outs, const Prefix& pref) {
    outs<<"[";
    FOR_EACH(i,pref) {
      const Quantification& q=*i;
      outs<<"[";
      outs<<(q.first==UNIVERSAL?"A":"E");
      for (Var v : q.second) outs<<" "<<v;
      outs<<"]";
    }
    return outs<<"]";
  }
}

inline size_t var_count(const Prefix& p) {
    size_t rv = 0;
    for (auto& q : p) rv += q.second.size();
    return rv;
}

inline QAigFla robustify(const QAigFla& f) {
    QAigFla retv;
    retv.pref = f.pref;
    retv.matrix = f.matrix;
    if (retv.pref.empty() || retv.pref.back().first != EXISTENTIAL)
        retv.pref.push_back(make_pair(EXISTENTIAL, VarVector()));
    retv.pref.push_back(make_pair(UNIVERSAL, VarVector()));
    return retv;
}
#endif /* QTYPES_HH_15436 */
