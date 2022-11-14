#include <assert.h>
#include <cstdio>
#include <string.h>

#include "Akinator.h"
#include "linesLib.h"

//----------------------------------------------------------------------
//CONSTANTS
//----------------------------------------------------------------------

static const int MAX_ANS_LEN = 1;

//----------------------------------------------------------------------

static void skipSymbolsBeforeNextToThis (char sym, int *position, char *InputData);

static void      DefineCharacter  (treeNode *tree, treeNode *node, bool last_def = true);
static treeNode *FindCharacter    (treeNode *node, const char *character_name);
static bool      isPrevAnsYes     (treeNode *node);
static int       initTree         (treeNode **node, int position, char *InputData, int depth = 0);

static void      CompleteGuessing ();
static void      FailGuessing     (treeNode *node);

static void      EqualizeDepth    (treeNode **first,      treeNode **second);
static treeNode *SearchCommonNode (treeNode  *first_node, treeNode  *second_node);

//----------------------------------------------------------------------

ProgMode GetProgramMode(const int argc, const char *argv[])
{
    if (argc != 2)                   {return WrongMode;  }

    if (strcmp(argv[1], "-MG") == 0) {return Guessing;   }
    if (strcmp(argv[1], "-MD") == 0) {return Definitions;}
    if (strcmp(argv[1], "-MC") == 0) {return Compare;    }

    return WrongMode;
}

void OptionalPrint(FILE *stream, treeNode *node, PrintMode mode, int space)
{
    printf("Do you want to save changes of database? (y/n)\n");

    Answers ans = ProcessingAnswer();
    if (ans == YES)
    {
        treePrint(stream, node, mode, space);
    }
    if (ans == Undefined)
    {
        printf("I don't understand. Can you please repeat your answer?\n");
        OptionalPrint(stream, node, mode, space);
    }
}

void OptionalPrint(const char *filename, treeNode *node, PrintMode mode, int space)
{
    printf("Do you want to save changes of database? (y/n)\n");

    Answers ans = ProcessingAnswer();
    if (ans == YES)
    {
        treePrint(filename, node, mode, space);
    }
    if (ans == Undefined)
    {
        printf("I don't understand. Can you please repeat your answer?\n");
        OptionalPrint(filename, node, mode, space);
    }
}

treeNode *InitData(const char *input_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == nullptr)
    {
        printf("Error opening input file: %s\n", input_filename);
        return nullptr;
    }
    
    Text Data = {};
    file_to_memory_with_fread(input_file, &Data);
    char *InputData = Data.content;
    fclose(input_file);

    int position = 0;
    
    skipSymbolsBeforeNextToThis('"', &position, InputData);

    tree_elem_t value = InputData + position;

    treeNode *Tree = nullptr;
    initTree(&Tree, position, InputData);
    return Tree;
}

Answers ProcessingAnswer() 
{
    char ans[MAX_ANS_LEN + 1] = "";
    gets(ans);

    switch (*ans)
    {
        case 'y':
            return YES;
            break;
        case 'n':
            return NO;
            break;
        default:
            return Undefined;
            break;
    }    
}

void GuessingCharacters(treeNode *data)
{
    printf("Is your character %s? (y/n)\n", data->data);
    int ans = ProcessingAnswer();

    switch (ans)
    {
        case YES:
        {
            if (data->right == nullptr) {CompleteGuessing();}
            else                        {GuessingCharacters(data->right);}
            break;
        }
        case NO:
        {
            if (data->left == nullptr) {FailGuessing(data);}
            else                       {GuessingCharacters(data->left);}
            break;
        }
        default:
        {
            printf("I don't understand. Can you repeat your answer? (%c)\n", ans);
            GuessingCharacters(data);
            break;
        }
    }
}

void GetDefinition(treeNode *data, const char *character_name)
{    
    treeNode *character = FindCharacter(data, character_name);
 
    if (character == nullptr) 
    {
        printf("\"%s\" not found.\n");
        return;
    }
    printf("%s is ", character_name);
    DefineCharacter(data, character, true);
}

void CompareCharacters(treeNode *data, const char *first_name, const char *second_name)
{
    treeNode *first_character  = FindCharacter(data, first_name );
    treeNode *second_character = FindCharacter(data, second_name);


    if (stricmp(first_name, second_name) == 0)
    {
        printf("There are the same characters.\n");
        return;
    }
    if (first_character == nullptr)
    {
        printf("Character's not found: \"%s\"\n", first_name);
        return;
    }
    if (second_character == nullptr)
    {
        printf("Character's not found: \"%s\"\n", second_name);
        return;
    }

    treeNode *FirstCommonNode = SearchCommonNode(first_character, second_character);

    if (FirstCommonNode == data)
    {
        printf("These characters don't have similar characteristics.\n");
    }
    else
    {
        printf("Both of the characters are ");
        DefineCharacter(data, FirstCommonNode);
    }
    
    printf("But %s is ", first_name);
    DefineCharacter(FirstCommonNode, first_character);

    printf("And %s is ", second_name);
    DefineCharacter(FirstCommonNode, second_character);
}

//----------------------------------------------------------------------

static int initTree(treeNode **node, int position, char *InputData, int depth)
{
    int start_pos = position;

    position++;
    skipSymbolsBeforeNextToThis('"', &position, InputData);
    InputData[position - 1] = '\0';

    char *temp = strdup(InputData + start_pos);

    *node = treeCtor(temp, depth);

    skipSymbolsBeforeNextToThis('{', &position, InputData);
    if (InputData[position] != '}')
    {
        skipSymbolsBeforeNextToThis('"', &position, InputData);
        treeNode *left_tree = nullptr;

        position = initTree(&left_tree, position, InputData, depth + 1);
        left_tree->parent = *node;
        
        (**node).left = left_tree;
    }
    else
    {
        position++;
    }

    skipSymbolsBeforeNextToThis('{', &position, InputData);

    if (InputData[position] != '}')
    {
        skipSymbolsBeforeNextToThis('"', &position, InputData);

        treeNode *right_tree = nullptr;

        position = initTree(&right_tree, position, InputData, depth + 1);
        right_tree->parent = *node;

        (**node).right = right_tree;
    }
    else
    {
        position++;
    }

    while (InputData[position] != '}' && InputData[position] != '\0') 
    {
        position++;
    }
    return position;
}

static void skipSymbolsBeforeNextToThis(char sym, int *position, char *InputData)
{
    while (InputData[*position - 1] != sym) 
    {
        (*position)++;
    }
}

static treeNode *FindCharacter(treeNode *node, const char *character_name)
{
    if (stricmp(node->data, character_name) == 0) {return node;}

    treeNode *character = nullptr;

    if (node->left != nullptr)
    {
        if (character = FindCharacter(node->left, character_name)) {return character;}
    }

    if (node->right != nullptr)
    {
        if (character = FindCharacter(node->right, character_name)) {return character;}
    }

    return nullptr;
}

static bool isPrevAnsYes(treeNode *node)
{
    return (node->parent->right == node); 
} 

static void CompleteGuessing()
{
    printf("Of course yes, I told you, I'm smart\n");
}

static void FailGuessing(treeNode *node)
{
    printf("I know who it is, but I just forgot. Could you remind me who it is?\n");
    
    char character[MAX_NODE_NAME_LEN + 1] = "";
    gets(character);

    printf("And how does %s differ from %s?\n" "%s is: ", character, node->data, character);
    char difference[MAX_NODE_NAME_LEN + 1] = "";
    gets(difference);

    addToLeft(node, node->data);

    char *temp = strdup(character);
    addToRight(node, temp);

    temp = strdup(difference);
    node->data = temp;
}

static void DefineCharacter(treeNode *tree, treeNode *node, bool last_def)
{
    if (node->parent != nullptr && node != tree)
    {  
        DefineCharacter(tree, node->parent, false);

        if (last_def)
        {
            printf("%s%s.\n", isPrevAnsYes(node) ? "" : "not ", node->parent->data);
        }
        else
        {
            printf("%s%s, ", isPrevAnsYes(node) ? "" : "not ", node->parent->data);
        }
    }
}

static void EqualizeDepth(treeNode **first, treeNode **second)
{
    assert(first != nullptr && second != nullptr);
    assert((*first)->depth >= 0 && (*second)->depth >= 0);

    if ((*first)->depth == (*second)->depth) {return;}

    while ((*first )->depth > (*second)->depth) {*first  = (*first )->parent;}
    while ((*second)->depth > (*first )->depth) {*second = (*second)->parent;}   
}

static treeNode *SearchCommonNode(treeNode *first_node, treeNode *second_node)
{
    EqualizeDepth(&first_node, &second_node);

    while (first_node != second_node)
    {
        first_node = first_node->parent;
        second_node = second_node->parent;
    }

    return first_node;
}

//----------------------------------------------------------------------