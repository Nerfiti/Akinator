#include "Tree.h"

const int MAX_NODE_NAME_LEN = 100;

enum Answers
{
    YES       =  1,
    NO        = -1,
    Undefined =  0
};

enum ProgMode
{
    Guessing,
    Definitions,
    Compare,
    WrongMode
};

ProgMode  GetProgramMode     (const int argc, const char *argv[]);
void      OptionalPrint      (FILE *stream,         treeNode *node, PrintMode mode, int space = 0);
void      OptionalPrint      (const char *filename, treeNode *node, PrintMode mode, int space = 0);
treeNode *InitData           (const char *input_filename);
Answers   ProcessingAnswer   ();
void      GuessingCharacters (treeNode *data);
void      GetDefinition      (treeNode *data, const char *character_name);
void      CompareCharacters  (treeNode *data, const char *first_name, const char * second_name);