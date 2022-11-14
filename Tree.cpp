#include <cstdio>
#include <malloc.h>

#include "Tree.h"

//--------------------------------------------------------------
//FOR GRAPHVIZ
//--------------------------------------------------------------

static const int  MAX_PATH_LEN    = 30;
static const char DUMP_PATH[]     = "./DumpFiles/Dump%d.dmp";
static const char SVG_DUMP_PATH[] = "./DumpFiles/Dump%d.svg";
static const char START_GRAPH[]   = "digraph\n{\n\tordering = out\nnode[style = filled]\n\n";

static int Dump_counter = 1;

//--------------------------------------------------------------
//FOR LOG
//--------------------------------------------------------------

static const char LOG_FILENAME[] = "./Log/log.html";

static FILE *Logfile = nullptr;

//--------------------------------------------------------------

static treeNode *addNode          (treeNode *node, tree_elem_t value, bool toLeft);
static int  creatGraphvizTreeCode (const treeNode *const node, int nodeNum, FILE *dump_file);
static void get_dump_filenames    (char *dump_filename, char *svg_dump_filename);
static void PrintNodeData         (FILE *stream, tree_elem_t data, int space, PrintMode mode);
static void setSpace              (FILE *stream, int space);

//--------------------------------------------------------------

void initLog()
{
    Logfile = fopen(LOG_FILENAME, "w");
    if (!Logfile)
    {
        printf("Error opening logfile");
        return;
    }

    fprintf(Logfile, "<style>p {font-size: 25px;}</style>\n\n"
                     "<p>Program started!</p>\n");
}

void closeLog()
{
    fprintf(Logfile, "\n<p>Program Finished!</p>");
}

treeNode *treeCtor(tree_elem_t value, int depth)
{
    treeNode *node = (treeNode *)calloc(1, sizeof(treeNode));

    node->data   = value;
    node->left   = nullptr;
    node->right  = nullptr;
    node->parent = nullptr;
    node->depth  = depth;

    return node;
}

treeNode *addToLeft(treeNode *node, tree_elem_t value)
{
    return addNode(node, value, true);
}

treeNode *addToRight(treeNode *node, tree_elem_t value)
{
    return addNode(node, value, false);
}

void treeDtor(treeNode *node)
{
    if (node == nullptr) {return;}

    treeDtor(node->left );
    treeDtor(node->right);

    node->data   = 0;
    node->left   = nullptr;
    node->right  = nullptr;
    node->parent = nullptr;
    node->depth  = 0;
    free(node);
}

void treePrint(FILE *const stream, const treeNode *const node, PrintMode mode, int space)
{
    setSpace(stream, space);
    fprintf(stream, "{");
    space++;

    if (node != nullptr)
    {
        if (mode == PRE_ORDER)
        {
            PrintNodeData(stream, node->data, space, mode);
        }
        else
        {
            fprintf(stream, "\n");
        }

        treePrint(stream, node->left, mode, space);

        if (mode == IN_ORDER)
        {
            PrintNodeData(stream, node->data, space, mode);
        }

        treePrint(stream, node->right, mode, space);

        if (mode == POST_ORDER)
        {
            PrintNodeData(stream, node->data, space, mode);
        }
        
        space--;
        setSpace(stream, space);
    }

    fprintf(stream, "}\n");
}

void treePrint(const char *const filename, const treeNode *const node, PrintMode mode, int space)
{
    FILE *stream = fopen(filename, "w");
    if (stream == nullptr)
    {
        printf("Error opening file for print: \"%s\".\n", filename);
        return;
    }

    treePrint(stream, node, mode, space);
}

void treeGraphDump(const treeNode *const node)
{
    fprintf(Logfile, "<hr>\n");

    char     dump_filename[MAX_PATH_LEN] = "";
    char svg_dump_filename[MAX_PATH_LEN] = "";

    get_dump_filenames(dump_filename, svg_dump_filename);

    FILE *dump_file = fopen(dump_filename, "w");
    if (dump_file == nullptr)
    {
        fprintf(Logfile, "Error opening dump file: %s\n", dump_filename);
        return;
    }

    fprintf(dump_file, "%s", START_GRAPH);
    int nodeNum = 0;
    creatGraphvizTreeCode(node, nodeNum, dump_file);

    fprintf(dump_file, "}");
    if (fclose(dump_file) != 0)
    {
        fprintf(Logfile, "<p>Error closing dump_file</p>\n");
    }

    const int MAX_CMD_LEN = 100;
    char CMD[MAX_CMD_LEN] = "";
    sprintf(CMD, "dot %s -T svg -o %s", dump_filename, svg_dump_filename);
    
    system(CMD);

    fprintf(Logfile, "\n<img src = \".%s\"><hr>\n", svg_dump_filename);
    Dump_counter++;
}

//--------------------------------------------------------------

static treeNode *addNode(treeNode *node, tree_elem_t value, bool toLeft)
{
    treeNode *newNode = treeCtor(value);
    newNode->parent   = node;
    newNode->depth    = node->depth + 1;

    if (toLeft) {node->left  = newNode;}
    else        {node->right = newNode;}

    return newNode;
}

static int creatGraphvizTreeCode(const treeNode *const node, int nodeNum, FILE *dump_file)
{
    int number_of_nodes = 1;
    if (node == nullptr)
    {
        fprintf(dump_file, "node%d [shape = point]\n", nodeNum);
        return number_of_nodes;
    }
    else
    {
        const int color_name_len = 10;
        const char color[color_name_len] = "";
        fprintf(dump_file, "node%d [label = \"%s\"]\n", nodeNum, node->data);
    }

    fprintf(dump_file, "node%d -> node%d [color = \"red\"]\n\n", nodeNum, nodeNum + number_of_nodes);
    number_of_nodes += creatGraphvizTreeCode(node->left, nodeNum + number_of_nodes, dump_file);

    fprintf(dump_file, "node%d -> node%d [color = \"green\"]\n\n", nodeNum, nodeNum + number_of_nodes);
    number_of_nodes += creatGraphvizTreeCode(node->right, nodeNum + number_of_nodes, dump_file);

    return number_of_nodes;
}

static void get_dump_filenames(char *dump_filename, char *svg_dump_filename)
{
    sprintf(    dump_filename,     DUMP_PATH, Dump_counter);
    sprintf(svg_dump_filename, SVG_DUMP_PATH, Dump_counter);
}

static void PrintNodeData(FILE *stream, tree_elem_t data, int space, PrintMode mode)
{ 
    if (mode != PRE_ORDER) 
    {
        setSpace(stream, space);
    }

    fprintf(stream, "\"%s\"\n", data);
}

static void setSpace(FILE *stream, int space)
{
    for (int i = 0; i < space; ++i)
    {
        fprintf(stream, "\t");
    }
}

//--------------------------------------------------------------