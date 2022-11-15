#include <cstdio>

typedef char* tree_elem_t;

enum PrintMode
{
    PRE_ORDER  = -1,
    IN_ORDER   =  0,
    POST_ORDER =  1
};

struct Node
{
    Node    *parent = nullptr;
    tree_elem_t  data   = 0;
    Node    *left   = nullptr;
    Node    *right  = nullptr;
    int          depth  = 0;
};

Node *treeCtor   (tree_elem_t value, int depth = 0);
Node *addToLeft  (Node *node, tree_elem_t value);
Node *addToRight (Node *node, tree_elem_t value);
void  treeDtor   (Node *node);

void treePrint       (FILE *stream,         const Node *node, PrintMode mode, int space = 0);
void treePrint       (const char *filename, const Node *node, PrintMode mode, int space = 0);
void treeGraphDump   (const Node *node);