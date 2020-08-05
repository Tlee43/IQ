//this code was copied from the QFUN implementation by Mikol� Janota
//found at http://sat.inesc-id.pt/~mikolas/sw/qfun/
/* 
 * File:   Read2Q.hh
 * Author: mikolas
 *
 * Created on Tue Jan 11 15:08:14
 */
#ifndef READ2Q_HH
#define	READ2Q_HH
#include <zlib.h>
#include <utility>
#include <vector>
#include <stdio.h>
#include <unordered_set>
#include "qtypes.h"
#include "Reader.h"
#include "ReadException.h"
#include "minisat_auxiliary.h"
using SATSPC::Lit;
using SATSPC::Var;

class ReadQ {
public:
    ReadQ(Reader& r, bool qube_input=false);
    ~ReadQ();
    void                           read();
    Var                            get_max_id() const;
    const std::vector<Quantification>&  get_prefix() const;
    const std::vector< LitSet >&        get_clauses() const;
    bool                           get_header_read() const;
    int                            get_qube_output() const;
private:
    Reader&                 r;
    bool                    qube_input;
    int                     qube_output;
    Var                     max_id;
    bool                    _header_read;
    std::vector< LitSet >        clause_vector;
    std::vector<Quantification>  quantifications;
    std::unordered_set<Var> quantified_variables;
    std::unordered_set<Var> unquantified_variables;
    void                    read_header();
    void                    read_quantifiers();
    void                    read_clauses();
    void                    read_cnf_clause(Reader& in, std::vector<Lit>& lits);
    void                    read_quantification(Reader& in, Quantification& quantification);
    Var                     parse_variable(Reader& in);
    int                     parse_lit(Reader& in);
    void                    read_word(const char* word, size_t length);
};
#endif	/* READ2Q_HH */
