# hw4 report

|||
|-:|:-|
|Name|田松翰|
|ID|110550071|

## How much time did you spend on this project

About 2 days.

## Project overview

The recommended workflow is helpful for this assignment.

#### Implement Symbol Table
In `sema/SymbolTable.hpp`, I defined 3 classes as the spec mentioned: *SymbolEntry*, *SymbolTable*, *SymbolManager*.
- **SymbolEntry**: An entry needs to store these information for a symbol: *Name, Kind, Level, Type, Attribute*.
- **SymbolTable**: A table saves several entries. Also, I define some basic function like `addSymbol`(for adding new entries), `dumpSymbol`(for printing the table the spec require), etc.
- **SymbolManager**: The manager stores the tables in the scope with LIFO property. 

#### Implement Switch Button 'D'
In `scanner.l` to find the pesudocomment D and pass to `parser.y`. If D is turned on, then it will dump the symbol table.

#### Implement Semantic Checks
Finish the analyzer in `sema/SemanticAnalyzer.cpp`, and add some functions in each class if necessary.

- ProgramNode, FunctionNode: Create a new symbol table first, and push it to the manager. Perform semantic analyses for the node and its children. At last, pop the symbol table out.

- VariableNode: This is for new variable. If the variable is declared correctly, it will be pushed to the top table. \
If it got redeclaration or array index <= 0, it will get a error message.

- ConstantValueNode: Some of these nodes are used for setting the constant value of variable node. Hence, it should be changed the kind from variable to constant.

- CompoundStatementNode: If the compound statement is used for parameters, it does not need to create a new table. Otherwise, it will add a new table to the manager.

- PrintNode, ReadNode: If the type of expression is not scaler type, it will return the error message.

- BinaryOperatorNode, UnaryOperatorNode: Check the operands and the operator. If the type of the operands corresponding different operator got wrong, it will return the error message.

- FunctionInvocationNode: First, check whether the function has been declared. Second, check whether the kind is function. Third, check the number of arguments. Last, check the type of arguments.

- VariableReferenceNode: First, check whether id has been declared. Second, make sure the kind is not program or function. Third, check the index of array is integer and not over array subscript.  \
Also, in this node, it can simple the array type. (e.g array: real a[5][3], type of a[1] is real[3])

- AssignmentNode: Check the type and the kind of the result of the variable reference.

- IfNode, WhileNode: Check the condition is boolean or not.

- ForNode: Check the declaration of loop_var and check the incremental order of iteration.

- ReturnNode: First, check the node is not in program or procedure. (their return type is void) Second, check the return type.

## What is the hardest you think in this project
1. Array is a hard part of this assignment, I want to change the type directly in the beginning (store `integer[1][1]` as `integer`), but it will got different in the symbol table and couldn't check the array dimensions. Finally, i got the dimensions vector in PType, and derive the type of referenced array.

2. For the undeclared function or variable, I forgot to handle the `nullptr` and the result of test cases will become catastropic and no idea where to start debugging. It can only test one by one cases to find where it wrong.

## Feedback to T.A.s

The implementation hints help a lot. It seems that this assignment is slightly easier to get started than hw3, but it gets much more things need to be handled.
