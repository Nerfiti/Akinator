typedef char* tree_elem_t;

enum PrintMode
{
    PRE_ORDER  = -1,
    IN_ORDER   =  0,
    POST_ORDER =  1
};

struct treeNode
{
    treeNode    *parent = nullptr;
    tree_elem_t  data   = 0;
    treeNode    *left   = nullptr;
    treeNode    *right  = nullptr;
    int          depth  = 0;
};

void initLog  ();
void closeLog ();

treeNode *treeCtor   (tree_elem_t value, int depth = 0);
treeNode *addToLeft  (treeNode *node, tree_elem_t value);
treeNode *addToRight (treeNode *node, tree_elem_t value);
void      treeDtor   (treeNode *node);

void treePrint       (FILE *const stream,         const treeNode *const node, PrintMode mode, int space = 0);
void treePrint       (const char *const filename, const treeNode *const node, PrintMode mode, int space = 0);
void treeGraphDump   (const treeNode *const node);