%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%token ID
%token integer oct_integer Float scientific string
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring
%token KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo
%token KWif KWthen KWelse KWfor KWto KWprint KWread ASSIGN

%left and or not
%left '<' LE NE GE '>' '='
%left '+' '-'
%left '/' mod
%left '*'

%%
ProgramUnit: Program | Function;

// Program
Program: ID ';' Declarations Functions CompoundStat KWend;
Declarations: Declaration Declarations | %empty;
Functions: Function Functions | %empty;

// Function
Function: FunctionDec | FunctionDef;
FuncHeader: ID '(' FormalArgs ')' ':' ScalerType | ID '(' FormalArgs ')';
FormalArgs: FormalArg ';' FormalArgs | FormalArg | %empty;
FunctionDec: FuncHeader ';';
FunctionDef: FuncHeader CompoundStat KWend;
FormalArg: IDList ':' Type;
IDList: ID ',' IDList | ID;
Type: ScalerType | ArrayType;

// Declaration
Declaration: varDec | constDec;
varDec: KWvar IDList ':' Type ';';
constDec: KWvar IDList ':' Literal ';';
Literal: Int | '-' Int | Real | '-' Real | string | KWtrue | KWfalse;
Int: integer | oct_integer;
Real: Float | scientific;

// Type
ScalerType: KWinteger | KWreal | KWstring | KWboolean;
ArrayType: KWarray Int KWof Type;

// Statement
Statement: SimpleStat | CondStat | FunCallStat | LoopStat | ReturnStat | CompoundStat;
SimpleStat: VarRef ASSIGN Expr ';' | KWprint Expr ';' | KWread VarRef ';';
CondStat: KWif Expr KWthen CompoundStat KWend KWif | KWif Expr KWthen CompoundStat KWelse CompoundStat KWend KWif;
FunCallStat: ID '(' Exprs ')' ';';
Exprs: Expr ',' Exprs | Expr | %empty;
LoopStat: KWwhile Expr KWdo CompoundStat KWend KWdo | KWfor ID ASSIGN Int KWto Int KWdo CompoundStat KWend KWdo;
ReturnStat: KWreturn Expr ';';
CompoundStat: KWbegin Declarations Statments KWend;
Statments: Statement Statments | %empty;

// Expression
Expr: LiteralConst | VarRef | FuncCall | OpExpr;
LiteralConst: Int | Real | string | KWtrue | KWfalse;
VarRef: ID RefExpr;
RefExpr: '[' Expr ']' RefExpr | %empty;
FuncCall: ID  '(' Exprs ')';
Relation: '<' | LE | NE | '>' | GE | '=';
OpExpr: Expr and Expr | Expr or Expr | not Expr | Expr Relation Expr | Expr '-' Expr | Expr '+' Expr
        | Expr '/' Expr | Expr mod Expr | Expr '*' Expr | '-' Expr | '(' Expr ')';

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }
    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
