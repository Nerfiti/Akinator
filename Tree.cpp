#include <malloc.h>
#include <cstdlib>

#include "logs.hpp"
#include "MyGeneralFunctions.hpp"
#include "Tree.hpp"

#define DEBUG

//--------------------------------------------------------------
//FOR GRAPH DUMP
//--------------------------------------------------------------

static int Dump_counter = 1;

static const int  MAX_PATH_LEN    = 30;
static const char DUMP_PATH[]     = "./DumpFiles/Dump%d.dot";
static const char SVG_DUMP_PATH[] = "./DumpFiles/Dump%d.svg";

static const char ADD_DUMP_TO_HTML_CODE[] = "<details>\n"
                                                "\t<summary>Dump%d</summary>\n"
                                                "\t<img src = \".%s\">\n"
                                            "</details>\n\n";

static const char NODE_COLOR[]      = "cornflowerblue";
static const char LEAF_COLOR[]      = "springGreen";
static const char JUST_ADDED_NODE[] = "pink";

static const char  LEFT_EDGE_COLOR[] = "red";
static const char RIGHT_EDGE_COLOR[] = "green";

static const char START_GRAPH[] =   "digraph {\n"
                                        "\tbgcolor=\"invis\"\n"
                                        "\tordering = out\n\n"
                                    "node[style = filled]\n\n";

//--------------------------------------------------------------

static Node *addNode              (Node *node, tree_elem_t value, bool toLeft, bool must_been_released);
static int  creatGraphvizTreeCode (const Node *node, int nodeNum, FILE *dump_file);
static void get_dump_filenames    (char *dump_filename, char *svg_dump_filename);
static void printNodeData         (FILE *stream, tree_elem_t data, int space, PrintMode mode);
static void setSpace              (FILE *stream, int space);
static bool IsLeaf                (const Node *node);

//--------------------------------------------------------------

Node *treeCtor(tree_elem_t value, bool must_been_released, int depth)
{
    Node *node = (Node *)calloc(1, sizeof(Node));

    node->data   = value;
    node->left   = nullptr;
    node->right  = nullptr;
    node->parent = nullptr;
    node->depth  = depth;
    
    node->must_been_released = must_been_released;

    return node;
}

Node *addToLeft(Node *node, tree_elem_t value, bool must_been_released)
{
    return addNode(node, value, true, must_been_released);
}

Node *addToRight(Node *node, tree_elem_t value, bool must_been_released)
{
    return addNode(node, value, false, must_been_released);
}

void treeDtor(Node *node)
{
    if (node == nullptr) {return;}

    treeDtor(node->left );
    treeDtor(node->right);

    #ifdef DEBUG
        node->data   = 0;
        node->left   = nullptr;
        node->right  = nullptr;
        node->parent = nullptr;
        node->depth  = 0;
    #endif //DEBUG

    if (node != JUST_FREE_PTR) 
    {
        free(node);
        node = (Node *)JUST_FREE_PTR;
    }

}

void treePrint(FILE *stream, const Node *node, PrintMode mode, int space)
{
    setSpace(stream, space);
    space++;

    fprintf(stream, "%c", OPEN_NODE_SYM);

    if (node != nullptr)
    {
        if (mode == PRE_ORDER)
        {
            printNodeData(stream, node->data, space, mode);
        }
        else
        {
            fprintf(stream, "\n");
        }

        treePrint(stream, node->left, mode, space);

        if (mode == IN_ORDER)
        {
            printNodeData(stream, node->data, space, mode);
        }

        treePrint(stream, node->right, mode, space);

        if (mode == POST_ORDER)
        {
            printNodeData(stream, node->data, space, mode);
        }
        
        space--;
        setSpace(stream, space);
    }

    fprintf(stream, "%c\n", CLOSE_NODE_SYM);
}

void treePrint(const char *filename, const Node *node, PrintMode mode, int space)
{
    FILE *stream = fopen(filename, "w");
    if (stream == nullptr)
    {
        printf("Error opening file for print: \"%s\".\n", filename);
        return;
    }

    treePrint(stream, node, mode, space);
}

void treeGraphDump(const Node *node)
{
    char     dump_filename[MAX_PATH_LEN] = "";
    char svg_dump_filename[MAX_PATH_LEN] = "";

    get_dump_filenames(dump_filename, svg_dump_filename);

    FILE *dump_file = fopen(dump_filename, "w");
    if (dump_file == nullptr)
    {
        log("Error opening dump file: %s\n", dump_filename);
        return;
    }

    fprintf(dump_file, "%s", START_GRAPH);
    int nodeNum = 0;
    creatGraphvizTreeCode(node, nodeNum, dump_file);

    fprintf(dump_file, "}");
    if (fclose(dump_file) != 0)
    {
        log("<p>Error closing dump_file</p>\n");
    }

    const int MAX_CMD_LEN = 100;
    char CMD[MAX_CMD_LEN] = "";
    sprintf(CMD, "dot %s -T svg -o %s", dump_filename, svg_dump_filename);
    
    system(CMD);

    log(ADD_DUMP_TO_HTML_CODE, Dump_counter, svg_dump_filename);
    Dump_counter++;
}

//--------------------------------------------------------------

static Node *addNode(Node *node, tree_elem_t value, bool toLeft, bool must_been_released)
{
    Node *newNode   = treeCtor(value, must_been_released);
    newNode->parent = node;
    newNode->depth  = node->depth + 1;

    if (toLeft) {node->left  = newNode;}
    else        {node->right = newNode;}

    return newNode;
}

static int creatGraphvizTreeCode(const Node *node, int nodeNum, FILE *dump_file)
{
    int number_of_nodes = 0;
    if (node == nullptr) {return number_of_nodes;}

    number_of_nodes ++;

    bool nullLeft  = true;
    bool nullRight = true;

    if (node->left != nullptr)
    {
        fprintf(dump_file, "node%d -> node%d [color = \"%s\"]\n\n", nodeNum, nodeNum + number_of_nodes, LEFT_EDGE_COLOR);        
        number_of_nodes += creatGraphvizTreeCode(node->left, nodeNum + number_of_nodes, dump_file);

        number_of_nodes++;
    }

    if (node->right != nullptr)
    {
        fprintf(dump_file, "node%d -> node%d [color = \"%s\"]\n\n", nodeNum, nodeNum + number_of_nodes, RIGHT_EDGE_COLOR);        
        number_of_nodes += creatGraphvizTreeCode(node->right, nodeNum + number_of_nodes, dump_file);

        number_of_nodes++;
    }

    const char *color = IsLeaf(node) ? LEAF_COLOR : NODE_COLOR;
    if (node->must_been_released)
    {
        color = JUST_ADDED_NODE;
    }

    fprintf(dump_file, "node%d [label = \"%s\", fillcolor = %s]\n", nodeNum, node->data, color);

    return number_of_nodes;
}

static void get_dump_filenames(char *dump_filename, char *svg_dump_filename)
{
    sprintf(    dump_filename,     DUMP_PATH, Dump_counter);
    sprintf(svg_dump_filename, SVG_DUMP_PATH, Dump_counter);
}

static void printNodeData(FILE *stream, tree_elem_t data, int space, PrintMode mode)
{ 
    if (mode != PRE_ORDER) 
    {
        setSpace(stream, space);
    }

    fprintf(stream, "\"%s\"\n", data);
}

#ifdef DEBUG
    static void setSpace(FILE *stream, int space)
    {
        for (int i = 0; i < space; ++i)
        {
            fprintf(stream, "\t");
        }
    }
#else
    static void setSpace(FILE *stream, int space) {}
#endif //DEBUG

static bool IsLeaf(const Node *node)
{
    return (node->left == nullptr && node->right == nullptr);
}

//--------------------------------------------------------------