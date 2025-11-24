#include "testTree.h"
#include "token.h"
#include <iostream>

static void printIndent(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
}

static void printTreeRec(Node *n, int level) {
    if (!n) return;

    printIndent(level);
    std::cout << n->label;

    // print all tokens in this node
    for (const Token &t : n->tokens) {
        int idx = static_cast<int>(t.id);
        const char *name = tokenName[idx];  // from token.h
        std::cout << " (" << name << ",'" << t.instance << "')";
    }
    std::cout << "\n";

    for (Node *child : n->children) {
        printTreeRec(child, level + 1);
    }
}

void testTree(Node *root) {
    printTreeRec(root, 0);
}
