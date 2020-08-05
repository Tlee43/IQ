//this code was copied from the QFUN implementation by Mikoláš Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/*
 * File:   MiniSatExt.hh
 * Author: mikolas
 *
 * Created on November 29, 2010, 5:40 PM
 */
#ifndef MINISATEXT_HH
#define	MINISATEXT_HH
#include "auxiliary.h"
#if USE_SIMP
#include "minisat/simp/SimpSolver.h"
#define BASE_SOLVER SimpSolver
#else
#include "minisat/core/Solver.h"
#define BASE_SOLVER Solver
#endif

namespace SATSPC {
  class MiniSatExt : public BASE_SOLVER {
  public:
    inline void bump(const Var var)        { varBumpActivity(var); }
    inline void new_variables(Var max_id);
    inline void new_variables(const std::vector<Var>& variables);
    inline lbool swap_pol(Var v, lbool def);
  };

  inline void MiniSatExt::new_variables(Var max_id) {
    const int target_number = (int)max_id+1;
    while (nVars() < target_number) newVar();
  }

  inline void MiniSatExt::new_variables(const std::vector<Var>& variables) {
    Var max_id = 0;
    FOR_EACH(variable_index, variables) {
      const Var v = *variable_index;
      if (max_id < v) max_id = v;
    }
    new_variables(max_id);
  }

  inline lbool MiniSatExt::swap_pol(Var v, lbool def) {
      assert(v < nVars());
      const auto o = user_pol[v];
      const auto r = o == l_Undef ? def : (o == l_True ? l_False : l_True);
      setPolarity(v, r);
      return r;
  }
}; /* namespace SATSPC */
#endif	/* MINISATEXT_HH */

