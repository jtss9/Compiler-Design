# hw2 report

|||
|-:|:-|
|Name|田松翰|
|ID|110550071|

## How much time did you spend on this project

I spent an hour on the lecture notes, reading the spec for another hour and about 4 hours on coding.

## Project overview

### Scanner
In `scanner.l`, we need to modify the file from hw1. When the scanner recognizes a token, it should pass to the parser using `return`. For example:
`"var"   { listToken("KWvar"); return KWvar;}`

### Parser
In `parser.y`, we should declare the tokens, and finish the grammer rules.
#### Declaration
We should declare the token passed from scanner in the beginning. For example: `%token ID` or `%left '*'`.
#### Grammer Rules
- It starts with `ProgramUnit`, which can be a program or function. The rules of program and function are:
```c++
Program: ID ';' Declarations Functions CompoundStat KWend;
Function: FunctionDec | FunctionDef;
// other grammer rules can be found in parser.y
```
- **Declaration** can be variable declaration or constant declaration. `Declaration: varDec | constDec;`
- **Type** can be scaler type or array type. `Type: ScalerType | ArrayType;` The former can be int, real, string, or bool, and the latter can be nested format, like: `var L2darray: array 4 of array 2 of integer;`
- There are multiple type of **statements**: SimpleStat, CondStat, FunCallStat, LoopStat, ReturnStat, CompoundStat. Each one has its own grammer to follow. The compound statement is the interesting one, which can be a nested statement like this:
```c
begin
    var a: 3;       // declare a
    var b: real;    // declare b
    begin            
        b := 20;    // a statement inside compound statement
    end              
    b := a+b;
end
```
- **Expression** can be one of the followings: LiteralConst, Variable Reference, Function Call, OpExpr (binary and unary operation), and **(** expression **)**. The precedence of operation can be determined by `%left`

## What is the hardest you think in this project

- I forgot to add the return part in scanner, so that it cannot read any token from the input file and gave the yyerror in the end, which made me think that something went wrong in the grammer part, and I will read the implementation part more carefully in next assignment.
- { item } which means zero or more items in the grammer rules. I think it's an interesting coding part that it should be in the recursive way. For example: 
```c
Program: ID ';' Declarations Functions CompoundStat KWend;
// Declarations and Functions can be zero or more
Declarations: Declaration Declarations | %empty;
Declaration: varDec | constDec;
// Take Declarations for example, if the amount is zero, it will take %empty, 
// otherwise, it will go through the recursive part for more-than-zero amount.

```

## Feedback to T.A.s

I'm sorry for not reading the spec completely. The questions I met are all mentioned in Implementation Notes in the spec. Thanks TA.