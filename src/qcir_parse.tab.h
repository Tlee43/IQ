/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_QCIR_QCIR_PARSE_TAB_H_INCLUDED
# define YY_QCIR_QCIR_PARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int qcir_debug;
#endif
/* "%code requires" blocks.  */
#line 81 "qcir_parse.y" /* yacc.c:1909  */


#line 47 "qcir_parse.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    VAR_ID = 258,
    FORMAT_ID = 259,
    NUM = 260,
    NL = 261,
    FORALL = 262,
    EXISTS = 263,
    XOR = 264,
    AND = 265,
    OR = 266,
    ITE = 267,
    OUTPUT = 268,
    LP = 269,
    RP = 270,
    EQ = 271,
    SEMI = 272,
    PLUS = 273,
    MINUS = 274,
    COMMA = 275,
    ERROR_TOKEN = 276
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 84 "qcir_parse.y" /* yacc.c:1909  */

  unsigned long long val;
  char* str;

#line 86 "qcir_parse.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE qcir_lval;

int qcir_parse (void);

#endif /* !YY_QCIR_QCIR_PARSE_TAB_H_INCLUDED  */
