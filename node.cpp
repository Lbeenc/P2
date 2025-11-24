#include "node.h"

Node* makeNode(const std::string &label) {
    Node *n = new Node;
    n->label = label;
    return n;
}

static void freeTreeRec(Node *n) {
    if (!n) return;
    for (Node *child : n->children) {
        freeTreeRec(child);
    }
    delete n;
}

void freeTree(Node *root) {
    freeTreeRec(root);
}
