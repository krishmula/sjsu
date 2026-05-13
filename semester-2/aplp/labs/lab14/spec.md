## In this lab, you will develop evaluation rules and typing rules for a small language.

Our language is an extended version of the one discussed in class:

e ::= true
| false
| i (Integers)
| s (Strings)
| succ e
| pred e
| iszero e
| if e then e else e
| concat e e
| isemptystr e
| strlen e

Our types must therefore include:

T ::= Bool
| Int
| String

First, write the evaluation order rules (using small-step semantics) for the expressions in the language.

Next, define the typing rules for these expressions. Be sure that your typing rules guarantee both progress and preservation. (For ways of formally guaranteeing these properties, see Chapter 8 of Ben Pierce's "Types and Programming Languages").