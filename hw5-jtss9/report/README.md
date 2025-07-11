# hw5 report

|||
|-:|:-|
|Name|田松翰|
|ID|110550071|

## How much time did you spend on this project

I spent 1 hour to read the spec and about 20 hours for coding part.(but i have not got all the points lol)

## Project overview

#### ProgramNoode
In this node, dump the instructions for the file prologue, and dump the instruction for string and reals of the bonus part.

#### VariableNode
Separate to several parts: global variable, global constant, local variable (local constant same as local variable). Also, function parameters are treated as local variable but dumping different instructions.

#### ConstantValueNode
Get the constant value and push it.

#### FunctionNode
It needs to reset the offset here because the program will use a new stack in the calling function.

#### PrintNode / ReadNode
Find the primitive type first and dump the instructions.

#### BinaryOperatorNode / UnaryOperatorNode
Just need to node what operand can do what kind of operation.

#### FunctionInvocationNode
Pass the arguments to a0~a7, if more than 8 args, pass to t0, t1, ... 

#### VariableReferenceNode
1. check if the variable is global
2. check string or real or what
3. check lhs or rhs

#### IfNode / WhileNode
Get the condition part and the statement part(s).

#### ForNode
Split it to var_decl, init, end, statements. Dump different instructions for different parts.

#### ReturnNode
Get the value and save it to a0 and pop.


## What is the hardest you think in this project

I think the hardest part is understand the riscv code. It's so hard for human reading. When I debugging, p language is easier to understand, but i need to check the riscv line by line, it's terrible.
Also, I don't know why the points will be different everytime i make test without changing anything.

## Feedback to T.A.s

Thanks TA, i hope i can pass this lecture.
