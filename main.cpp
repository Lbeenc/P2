#include <iostream>
#include <cstdio>

#include "scanner.h"
#include "parser.h"
#include "testTree.h"
#include "node.h"

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);

    FILE *fp = nullptr;

    if (argc == 1) {
        // read from stdin
        fp = stdin;
    } else if (argc == 2) {
        fp = std::fopen(argv[1], "r");
        if (!fp) {
            std::perror("Error opening input file");
            return 1;
        }
    } else {
        std::cerr << "Usage: parser [file]\n";
        return 1;
    }

    initScanner(fp);

    Node *root = parser();
    testTree(root);
    freeTree(root);

    if (fp != stdin) {
        std::fclose(fp);
    }
    return 0;
}
