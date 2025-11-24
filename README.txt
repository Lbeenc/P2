P2 Parser and Parse Tree Builder
===============================

This project implements the CS4280/5280 Project 2 requirements:
- Uses your P1 scanner (scanner.cpp, scanner.h, token.h)
- Implements an LL(1) recursive-descent parser
- Constructs a parse tree
- Outputs the parse tree in preorder via testTree()

Project Structure
-----------------

main.cpp
    - Handles command-line input
    - Initializes scanner
    - Calls parser() to build the parse tree
    - Calls testTree() to print the tree
    - Cleans up memory

parser.cpp / parser.h
    - Implements one function per nonterminal
    - Uses FIRST-set logic to choose productions
    - Reports syntax errors with line information
    - Returns a Node* root representing the full parse tree

node.cpp / node.h
    - Defines the Node struct used for parse tree representation
    - Handles tree creation and cleanup

testTree.cpp / testTree.h
    - Preorder traversal printer
    - Indentation represents tree depth
    - Prints node label and all tokens stored in that node

scanner.cpp / scanner.h / token.h  (from P1)
    - Lexical analyzer already completed in Project 1
    - Integrated here unchanged

Makefile
    - Compiles all components
    - Output executable is named:   parser

Invocation
----------

Correct usage:
    ./parser
    ./parser inputfile

If no file is given, the parser reads from standard input.

Grammar (Summary)
-----------------

program → "program" vars block "exit"
vars    → "var" IDENT ":" NUMBER ";" vars | ε
block   → "start" stats "trats"
stats   → stat mStat
mStat   → stat mStat | ε
stat    → in | out | block | if | loop | assign
in      → "read" IDENT ";"
out     → "print" expr ";"
if      → "if" "[" expr RO expr "]" "then" stat
loop    → "while" "[" expr RO expr "]" stat
assign  → "set" IDENT ":" expr ";"
expr    → M expr'
expr'   → ("+"|"-") M expr' | ε
M       → T M'
M'      → ("*"|"%") T M' | ε
T       → IDENT | NUMBER | "(" expr ")"
RO      → "<" | "<=" | ">" | ">=" | "eq" | "neq"

Output Format
--------------

testTree() prints one node per line in preorder:

    <nonterminal> (TokenType,'lexeme') (TokenType,'lexeme')
        <child>
        <child>
        ...

Tokens for identifiers, numbers, and operators/delimiters are included.

Building
--------

Run:
    make

Cleaning:
    make clean

Notes
-----

• Ensure that all source files remain in the same directory.  
• Only the files included in this directory should be submitted.  
• Make sure the executable name is exactly: parser  
• Whitespace-separated tokens are assumed by the project spec.

