# hw1 report

|Field|Value|
|-:|:-|
|Name|田松翰|
|ID|110550071|

## How much time did you spend on this project

I spent 2 hours on the lecture notes and the spec and spent about 3 hours on coding.

## Project overview

#### Definitions
- Define some regular expression like digit, letter, ...etc, which is convenient for implementing the rules below.
- An interesting tool `%x COMMENT`signifies exclusive start conditions, which can be easier to implement the comment section.

#### Rules
- List all the tokens mentioned in the spec. (delimeters, operators, reserved words...)
- Comments and Pseudocomments needed to do some special handling. For example, begin the `COMMENT` condition , change the value of `opt_src`.
- String token needs to erase the first and last character(""). And if it appeared two times of " consecutive, erase one of it. ("aa""bb" -> aa"bb)

## What is the hardest you think in this project
- The spec is so long and so difficult to understand for a beginner. For the first time to learn this kind of low-level languages like lex, it takes much time to get familiar and proficient.
- For the definition of floating points and scientific notations, it needs to try and error to know how to fix the regular expression. I think it's hard for me to get correct in the first time.

## Feedback to T.A.s

Thanks TA for providing such detailed and thorough explantions for the assignment.
