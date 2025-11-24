#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include "token.h"

// Simple parse tree node
struct Node {
    std::string label;              // nonterminal name, e.g. "program", "expr"
    std::vector<Token> tokens;      // tokens stored at this node
    std::vector<Node*> children;    // children, left-to-right
};

// Helper to create a node with a label
Node* makeNode(const std::string &label);

// Free the entire tree recursively
void freeTree(Node *root);

#endif // NODE_H
