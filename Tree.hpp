#include <cstdio>

//----------------------------------------------------------------------
//CONSTANTS
//----------------------------------------------------------------------

static const char  OPEN_NODE_SYM = '{';
static const char CLOSE_NODE_SYM = '}';
static const char IDENT_DATA_SYM = '"';

//----------------------------------------------------------------------

typedef char* tree_elem_t;

enum PrintMode
{
    PRE_ORDER,
    IN_ORDER,
    POST_ORDER
};

struct Node
{
    tree_elem_t data = 0;
    Node *parent = nullptr;
    Node *left   = nullptr;
    Node *right  = nullptr;
    int  depth   = 0;

    bool must_been_released = false; 
};

//----------------------------------------------------------------------

Node *treeCtor   (tree_elem_t value, bool must_been_released, int depth = 0);
Node *addToLeft  (Node *node, tree_elem_t value, bool must_been_released);
Node *addToRight (Node *node, tree_elem_t value, bool must_been_released);
void  treeDtor   (Node *node);

void treePrint     (FILE *stream,         const Node *node, PrintMode mode, int space = 0);
void treePrint     (const char *filename, const Node *node, PrintMode mode, int space = 0);
void treeGraphDump (const Node *node);