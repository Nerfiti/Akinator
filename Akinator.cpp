#include <assert.h>
#include <cstdio>
#include <string.h>
#include <sys/wait.h>

#include "Akinator.hpp"
#include "linesLib.hpp"
#include "logs.hpp"
#include "MyGeneralFunctions.hpp"

//----------------------------------------------------------------------
//CONSTANTS
//----------------------------------------------------------------------

static const int MAX_ANS_LEN = 1;

//----------------------------------------------------------------------

static void SkipSymbolsBeforeNextToSymbol (char sym, int *position, char *InputData);

static void  DefineCharacter  (Node  *tree, Node *node, bool last_def = true);
static Node *FindCharacter    (Node  *node, const char *character_name);
static bool  IsPrevAnsYes     (Node  *node);
static int   InitTree         (Node **node, int position, char *InputData, int depth = 0);

static void  CompleteGuessing ();
static void  FailGuessing     (Node *node);

static void  EqualizeDepth    (Node **first,      Node **second);
static Node *SearchCommonNode (Node  *first_node, Node  *second_node);

//----------------------------------------------------------------------

ProgMode GetProgramMode(const int argc, const char *argv[])
{
    if (argc != 2)                   {return WrongMode;  }

    if (strcmp(argv[1], "-MG") == 0) {return Guessing;   }
    if (strcmp(argv[1], "-MD") == 0) {return Definitions;}
    if (strcmp(argv[1], "-MC") == 0) {return Compare;    }

    return WrongMode;
}

void OptionalPrint(FILE *stream, Node *node, PrintMode mode, int space)
{
    pid_t PID = CMD_Speak("Do you want to save changes of database? (y/n)\n");//TODO: phrases

    Answers ans = ProcessingAnswer(PID);
    if (ans == YES)
    {
        treePrint(stream, node, mode, space);
    }
}

void OptionalPrint(const char *filename, Node *node, PrintMode mode, int space)
{
    pid_t PID = CMD_Speak("Do you want to save changes of database? (y/n)\n");//TODO: phrases

    Answers ans = ProcessingAnswer(PID);
    if (ans == YES)
    {
        treePrint(filename, node, mode, space);
    }
}

Node *InitData(const char *input_filename)
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
    
    SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, InputData);

    tree_elem_t value = InputData + position;

    Node *Tree = nullptr;
    InitTree(&Tree, position, InputData);
    return Tree;
}

Answers ProcessingAnswer(pid_t speak_pid) 
{
    char ans = 0;
    scanf("%c", &ans);
    while(getchar() != '\n');
    
    kill(speak_pid, SIGKILL);

    switch (ans)
    {
        case 'y':
            return YES;
        case 'n':
            return NO;
        default:
            speak_pid = CMD_Speak("I don't understand (you say \"%c\"). Can you, please, repeat your answer?\n", ans);//TODO: phrases
            return ProcessingAnswer(speak_pid);
    }    
}

void GuessingCharacters(Node *data)
{
    pid_t PID = CMD_Speak("Is your character %s? (y/n)\n", data->data);//TODO: phrases
    int ans = ProcessingAnswer(PID);

    switch (ans)
    {
        case YES:
        {
            if (data->right == nullptr) 
            {
                CompleteGuessing();
            }
            else 
            {
                GuessingCharacters(data->right);
            }
            break;
        }
        case NO:
        {
            if (data->left == nullptr) 
            {
                FailGuessing(data);
            }
            else
            {
                GuessingCharacters(data->left);
            }
            break;
        }
        default:
        {
            CMD_Speak("I don't understand. Can you repeat your answer? (%c)\n", ans);//TODO: phrases
            GuessingCharacters(data);
            break;
        }
    }
}

void GetDefinition(Node *data, const char *character_name)
{    
    Node *character = FindCharacter(data, character_name);
 
    if (character == nullptr) 
    {
        CMD_SpeakWithoutAns("\"%s\" not found.\n", character_name);//TODO: phrases
        return;
    }
    CMD_SpeakWithoutAns("%s is ", character_name);//TODO: phrases

    DefineCharacter(data, character, true);
}

void SimAndDiffsCharacters(Node *data, const char *first_name, const char *second_name)
{
    Node *first_character  = FindCharacter(data, first_name );
    Node *second_character = FindCharacter(data, second_name);


    if (strcmp(first_name, second_name) == 0)
    {
        CMD_SpeakWithoutAns("There are the same characters.\n");//TODO: phrases
        return;
    }
    if (first_character == nullptr)
    {
        CMD_SpeakWithoutAns("Character's not found: \"%s\"\n", first_name);//TODO: phrases
        return;
    }
    if (second_character == nullptr)
    {
        CMD_SpeakWithoutAns("Character's not found: \"%s\"\n", second_name);//TODO: phrases
        return;
    }

    Node *FirstCommonNode = SearchCommonNode(first_character, second_character);

    if (FirstCommonNode == data)
    {
        CMD_SpeakWithoutAns("These characters don't have similar characteristics.\n");//TODO: phrases
    }
    else
    {
        CMD_SpeakWithoutAns("Both of the characters are ");//TODO: phrases
        DefineCharacter(data, FirstCommonNode);
    }

    CMD_SpeakWithoutAns("But %s is ", first_name);//TODO: phrases
    DefineCharacter(FirstCommonNode, first_character);

    CMD_SpeakWithoutAns("And %s is ", second_name);//TODO: phrases
    DefineCharacter(FirstCommonNode, second_character);
}

//----------------------------------------------------------------------

static int InitTree(Node **node, int position, char *InputData, int depth)
{
    assert (node      != nullptr);
    assert (InputData != nullptr);

    int start_pos = position;

    position++;
    SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, InputData);
    InputData[position - 1] = '\0';

    char *temp = strdup(InputData + start_pos);

    *node = treeCtor(temp, depth);

    SkipSymbolsBeforeNextToSymbol(OPEN_NODE_SYM, &position, InputData);
    if (InputData[position] != CLOSE_NODE_SYM)
    {
        SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, InputData);
        Node *left_tree = nullptr;

        position = InitTree(&left_tree, position, InputData, depth + 1);
        left_tree->parent = *node;
        
        (**node).left = left_tree;
    }
    else
    {
        position++;
    }

    SkipSymbolsBeforeNextToSymbol(OPEN_NODE_SYM, &position, InputData);

    if (InputData[position] != CLOSE_NODE_SYM)
    {
        SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, InputData);

        Node *right_tree = nullptr;

        position = InitTree(&right_tree, position, InputData, depth + 1);
        right_tree->parent = *node;

        (**node).right = right_tree;
    }
    else
    {
        position++;
    }

    while (InputData[position] != CLOSE_NODE_SYM && InputData[position] != '\0') 
    {
        position++;
    }
    return position;
}

static void SkipSymbolsBeforeNextToSymbol(char sym, int *position, char *InputData)
{
    while (InputData[*position - 1] != sym) 
    {
        (*position)++;
    }
}

static Node *FindCharacter(Node *node, const char *character_name)
{
    if (strcmp(node->data, character_name) == 0) {return node;}

    Node *character = nullptr;

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

static bool IsPrevAnsYes(Node *node)
{
    assert (node != nullptr && node->parent != nullptr);
    return (node->parent->right == node); 
} 

static void CompleteGuessing()
{
    CMD_SpeakWithoutAns("Of course yes, I told you, I'm smart\n");//TODO: phrases
}

static void FailGuessing(Node *node)
{
    pid_t PID = CMD_Speak("I know who it is, but I just forgot. Could you remind me who it is?\n");//TODO: phrases
    
    char character[MAX_NODE_NAME_LEN + 1] = "";
    scanf("%[^\n]", character);
    getchar();

    kill(PID, SIGKILL);

    PID = CMD_Speak("And how does %s differ from %s?\n" "%s is: ", character, node->data, character);//TODO: phrases
    
    char difference[MAX_NODE_NAME_LEN + 1] = "";
    scanf("%[^\n]", difference);
    getchar();

    kill(PID, SIGKILL);

    addToLeft(node, node->data);

    char *temp = strdup(character);
    addToRight(node, temp);

    temp = strdup(difference);
    node->data = temp;
}

static void DefineCharacter(Node *tree, Node *node, bool last_def)
{
    if (node->parent != nullptr && node != tree)
    {  
        DefineCharacter(tree, node->parent, false);

        CMD_SpeakWithoutAns("%s%s", IsPrevAnsYes(node) ? "" : "not ", node->parent->data);

        if (last_def)
        {
            printf(".\n");
        }
        else
        {
            printf(", ");
        }
    }
}

static void EqualizeDepth(Node **first, Node **second)
{
    assert(first != nullptr && second != nullptr);
    assert((*first)->depth >= 0 && (*second)->depth >= 0);

    if ((*first)->depth == (*second)->depth) {return;}

    while ((*first )->depth > (*second)->depth) {*first  = (*first )->parent;}
    while ((*second)->depth > (*first )->depth) {*second = (*second)->parent;}   
}

static Node *SearchCommonNode(Node *first_node, Node *second_node)
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