#include "parser.h"
#include "scanner.h"

#include <iostream>
#include <cstdlib>

// current lookahead token
static Token tk;

// forward declarations of nonterminals
static Node* programNT();
static Node* varsNT();
static Node* blockNT();
static Node* statsNT();
static Node* mStatNT();
static Node* statNT();
static Node* inNT();
static Node* outNT();
static Node* ifNT();
static Node* loopNT();
static Node* assignNT();
static Node* exprNT();
static Node* exprPrimeNT();
static Node* MNT();
static Node* MPrimeNT();
static Node* TNT();
static Node* RONT();

// ------------ basic helpers ------------

static void advance() {
    tk = scanner();
}

static [[noreturn]] void parseError(const std::string &msg) {
    std::cerr << "SYNTAX ERROR at line " << tk.line
              << ": " << msg << " (got token '" << tk.instance << "')\n";
    std::exit(EXIT_FAILURE);
}

static bool isIdent(const Token &t) {
    return t.id == TokenID::IDENT_tk;
}

static bool isNumber(const Token &t) {
    return t.id == TokenID::NUM_tk;
}

static bool isKeyword(const std::string &kw) {
    return tk.id == TokenID::KW_tk && tk.instance == kw;
}

static bool isOpLex(const std::string &lex) {
    return tk.id == TokenID::OP_tk && tk.instance == lex;
}

// expect a specific keyword
static Token expectKW(const std::string &kw) {
    if (!isKeyword(kw)) {
        parseError("expected keyword '" + kw + "'");
    }
    Token out = tk;
    advance();
    return out;
}

// expect identifier
static Token expectIdent() {
    if (!isIdent(tk)) {
        parseError("expected identifier");
    }
    Token out = tk;
    advance();
    return out;
}

// expect number
static Token expectNumber() {
    if (!isNumber(tk)) {
        parseError("expected number");
    }
    Token out = tk;
    advance();
    return out;
}

// expect a specific operator/delimiter lexeme
static Token expectOp(const std::string &lex) {
    if (!isOpLex(lex)) {
        parseError("expected symbol '" + lex + "'");
    }
    Token out = tk;
    advance();
    return out;
}

// public entry point
Node* parser() {
    advance();              // prime lookahead
    Node *root = programNT();

    if (tk.id != TokenID::EOFTk) {
        parseError("extra tokens after end of program");
    }
    return root;
}

// ------------ nonterminals ------------
// Grammar (adjust to your official BNF if needed):
// <program> -> "program" <vars> <block> "exit"
// <vars>    -> "var" IDENT ":" NUMBER ";" <vars> | epsilon
// <block>   -> "start" <stats> "trats"
// <stats>   -> <stat> <mStat>
// <mStat>   -> <stat> <mStat> | epsilon
// <stat>    -> <in> | <out> | <block> | <if> | <loop> | <assign>
// <in>      -> "read" IDENT ";"
// <out>     -> "print" <expr> ";"
// <if>      -> "if" "[" <expr> <RO> <expr> "]" "then" <stat>
// <loop>    -> "while" "[" <expr> <RO> <expr> "]" <stat>
// <assign>  -> "set" IDENT ":" <expr> ";"
// <expr>    -> <M> <expr'>
// <expr'>   -> ("+"|"-") <M> <expr'> | epsilon
// <M>       -> <T> <M'>
// <M'>      -> ("*"|"%") <T> <M'> | epsilon
// <T>       -> IDENT | NUMBER | "(" <expr> ")"
// <RO>      -> "<" | "<=" | ">" | ">=" | "eq" | "neq"

// <program> -> "program" <vars> <block> "exit"
static Node* programNT() {
    Node *n = makeNode("program");

    n->tokens.push_back(expectKW("program"));
    // If your BNF has a program name, insert expectIdent() here.

    n->children.push_back(varsNT());
    n->children.push_back(blockNT());

    n->tokens.push_back(expectKW("exit"));

    return n;
}

// <vars> -> "var" IDENT ":" NUMBER ";" <vars> | epsilon
static Node* varsNT() {
    Node *n = makeNode("vars");

    if (isKeyword("var")) {
        n->tokens.push_back(expectKW("var"));
        n->tokens.push_back(expectIdent());
        n->tokens.push_back(expectOp(":"));
        n->tokens.push_back(expectNumber());
        n->tokens.push_back(expectOp(";"));
        n->children.push_back(varsNT());
    }
    // epsilon production: otherwise do nothing
    return n;
}

// <block> -> "start" <stats> "trats"
static Node* blockNT() {
    Node *n = makeNode("block");
    n->tokens.push_back(expectKW("start"));
    n->children.push_back(statsNT());
    n->tokens.push_back(expectKW("trats"));
    return n;
}

// <stats> -> <stat> <mStat>
static Node* statsNT() {
    Node *n = makeNode("stats");
    n->children.push_back(statNT());
    n->children.push_back(mStatNT());
    return n;
}

// <mStat> -> <stat> <mStat> | epsilon
static Node* mStatNT() {
    Node *n = makeNode("mStat");

    // FIRST(stat) set
    if (isKeyword("read") || isKeyword("print") ||
        isKeyword("start") || isKeyword("if") ||
        isKeyword("while") || isKeyword("set")) {

        n->children.push_back(statNT());
        n->children.push_back(mStatNT());
    }
    // else epsilon
    return n;
}

// <stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
static Node* statNT() {
    Node *n = makeNode("stat");

    if (isKeyword("read")) {
        n->children.push_back(inNT());
    } else if (isKeyword("print")) {
        n->children.push_back(outNT());
    } else if (isKeyword("start")) {
        n->children.push_back(blockNT());
    } else if (isKeyword("if")) {
        n->children.push_back(ifNT());
    } else if (isKeyword("while")) {
        n->children.push_back(loopNT());
    } else if (isKeyword("set")) {
        n->children.push_back(assignNT());
    } else {
        parseError("expected a statement (read/print/start/if/while/set)");
    }

    return n;
}

// <in> -> "read" IDENT ";"
static Node* inNT() {
    Node *n = makeNode("in");
    n->tokens.push_back(expectKW("read"));
    n->tokens.push_back(expectIdent());
    n->tokens.push_back(expectOp(";"));
    return n;
}

// <out> -> "print" <expr> ";"
static Node* outNT() {
    Node *n = makeNode("out");
    n->tokens.push_back(expectKW("print"));
    n->children.push_back(exprNT());
    n->tokens.push_back(expectOp(";"));
    return n;
}

// <if> -> "if" "[" <expr> <RO> <expr> "]" "then" <stat>
static Node* ifNT() {
    Node *n = makeNode("if");
    n->tokens.push_back(expectKW("if"));
    n->tokens.push_back(expectOp("["));
    n->children.push_back(exprNT());
    n->children.push_back(RONT());
    n->children.push_back(exprNT());
    n->tokens.push_back(expectOp("]"));
    n->tokens.push_back(expectKW("then"));
    n->children.push_back(statNT());
    return n;
}

// <loop> -> "while" "[" <expr> <RO> <expr> "]" <stat>
static Node* loopNT() {
    Node *n = makeNode("loop");
    n->tokens.push_back(expectKW("while"));
    n->tokens.push_back(expectOp("["));
    n->children.push_back(exprNT());
    n->children.push_back(RONT());
    n->children.push_back(exprNT());
    n->tokens.push_back(expectOp("]"));
    n->children.push_back(statNT());
    return n;
}

// <assign> -> "set" IDENT ":" <expr> ";" 
static Node* assignNT() {
    Node *n = makeNode("assign");
    n->tokens.push_back(expectKW("set"));
    n->tokens.push_back(expectIdent());
    n->tokens.push_back(expectOp(":"));
    n->children.push_back(exprNT());
    n->tokens.push_back(expectOp(";"));
    return n;
}

// <expr> -> <M> <expr'>
static Node* exprNT() {
    Node *n = makeNode("expr");
    n->children.push_back(MNT());
    n->children.push_back(exprPrimeNT());
    return n;
}

// <expr'> -> ("+"|"-") <M> <expr'> | epsilon
static Node* exprPrimeNT() {
    Node *n = makeNode("expr'");
    if (isOpLex("+") || isOpLex("-")) {
        n->tokens.push_back(tk);
        advance();
        n->children.push_back(MNT());
        n->children.push_back(exprPrimeNT());
    }
    return n;
}

// <M> -> <T> <M'>
static Node* MNT() {
    Node *n = makeNode("M");
    n->children.push_back(TNT());
    n->children.push_back(MPrimeNT());
    return n;
}

// <M'> -> ("*"|"%") <T> <M'> | epsilon
static Node* MPrimeNT() {
    Node *n = makeNode("M'");
    if (isOpLex("*") || isOpLex("%")) {
        n->tokens.push_back(tk);
        advance();
        n->children.push_back(TNT());
        n->children.push_back(MPrimeNT());
    }
    return n;
}

// <T> -> IDENT | NUMBER | "(" <expr> ")"
static Node* TNT() {
    Node *n = makeNode("T");

    if (isIdent(tk)) {
        n->tokens.push_back(expectIdent());
    } else if (isNumber(tk)) {
        n->tokens.push_back(expectNumber());
    } else if (isOpLex("(")) {
        n->tokens.push_back(expectOp("("));
        n->children.push_back(exprNT());
        n->tokens.push_back(expectOp(")"));
    } else {
        parseError("expected identifier, number, or '('");
    }
    return n;
}

// <RO> -> "<" | "<=" | ">" | ">=" | "eq" | "neq"
static Node* RONT() {
    Node *n = makeNode("RO");

    if (isOpLex("<") || isOpLex("<=") ||
        isOpLex(">") || isOpLex(">=")) {
        n->tokens.push_back(tk);
        advance();
    } else if (tk.id == TokenID::OP_tk &&
               (tk.instance == "eq" || tk.instance == "neq")) {
        n->tokens.push_back(tk);
        advance();
    } else {
        parseError("expected relational operator (<, <=, >, >=, eq, neq)");
    }

    return n;
}
