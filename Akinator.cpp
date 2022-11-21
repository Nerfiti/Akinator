#include <assert.h>
#include <cstdio>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <sys/wait.h>

#include "Akinator.hpp"
#include "linesLib.hpp"
#include "logs.hpp"
#include "MyGeneralFunctions.hpp"

//----------------------------------------------------------------------
//CONSTANTS
//----------------------------------------------------------------------

#include "phrases.hpp"

//----------------------------------------------------------------------

static void SkipSymbolsBeforeNextToSymbol (char sym, int *position, char *InputData);

static ProgMode AskProgMode   (pid_t speak_pid);

static void  DefineCharacter  (Node  *tree, Node *node, bool last_def = true);
static Node *FindCharacter    (Node  *node, const char *character_name);
static bool  IsPrevAnsYes     (Node  *node);
static int   InitTree         (Node **node, int position, char *InputData, int depth = 0);
static void  CompleteGuessing ();
static void  FailGuessing     (Node *node);
static void  EqualizeDepth    (Node **first,      Node **second);
static Node *SearchCommonNode (Node  *first_node, Node  *second_node);

//----------------------------------------------------------------------

Node *Run_Akinator(const char *input_filename, ProgMode mode)
{
    Answers ans = YES;

    Node *data = InitData(input_filename);

    Greet();

    while (ans == YES)
    {
        switch (mode)
        {
            case Guessing:
            {
                GuessingCharacters(data);
                break;
            }        
            case Definitions:
            {
                pid_t PID = CMD_Speak(ASK_ABOUT_CHARACTER_FOR_DEFINITION);

                char character_name[MAX_NODE_NAME_LEN] = "";
                GetLine(character_name);

                kill(PID, SIGKILL);                                

                GetDefinition(data, character_name);
                break;
            }
            case Compare:
            {
                pid_t PID = CMD_Speak(ASK_ABOUT_CHARATERS_FOR_COMPARING);
                
                char  first_character_name[MAX_NODE_NAME_LEN] = "";
                GetLine(first_character_name);

                char second_character_name[MAX_NODE_NAME_LEN] = "";
                GetLine(second_character_name);

                kill(PID, SIGKILL);

                SimAndDiffsCharacters(data, first_character_name, second_character_name);
                break;
            }
            default:
            {
                pid_t PID = CMD_Speak(ASK_PROGRAM_MODE);
                mode = AskProgMode(PID);
                continue;
            }
        }
        pid_t PID = CMD_Speak(ONE_MORE_TIME);
        ans = ProcessingAnswer(PID);
        if (ans == YES) 
        {
            pid_t PID = CMD_Speak(ASK_PROGRAM_MODE);
            mode = AskProgMode(PID);
        }
    }

    OptionalPrint(input_filename, data, PRE_ORDER);
    Farewell();
    return data;
}

void Greet()
{
    for (int i = 0; i < NUMBER_OF_GREETINGS_PARTS; ++i)
    {
        CMD_SpeakAndWait(GREETINGS[i]);
    }
}

void Farewell()
{
    for (int i = 0; i < NUMBER_OF_FAREWELLS_PARTS; ++i)
    {
        CMD_SpeakAndWait(FAREWELLS[i]);
    }
}

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
    pid_t PID = CMD_Speak(DATABASE_CHANGES);

    Answers ans = ProcessingAnswer(PID);
    if (ans == YES)
    {
        treePrint(stream, node, mode, space);
    }
}

void OptionalPrint(const char *filename, Node *node, PrintMode mode, int space)
{
    pid_t PID = CMD_Speak(DATABASE_CHANGES);

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
    fclose(input_file);

    int position = 0;
    
    SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, Data.content);

    tree_elem_t value = Data.content + position;

    Node *Tree = nullptr;
    InitTree(&Tree, position, Data.content);

    FreeBuff(&Data);
    return Tree;
}

Answers ProcessingAnswer(pid_t speak_pid) 
{
    char ans = 0;
    scanf("%c", &ans);
    while(getchar() != '\n');
    
    kill(speak_pid, SIGKILL);

    switch (tolower(ans))
    {
        case 'y':
            return YES;
        case 'n':
            return NO;
        default:
            speak_pid = CMD_Speak(UNDEFINED_ANSWER, ans);
            return ProcessingAnswer(speak_pid);
    }    
}

void GuessingCharacters(Node *data)//Without recursive
{
    pid_t PID = CMD_Speak(ANSWERS_ABOUT_CHARACTER, data->data);
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
    }
}

void GetDefinition(Node *data, const char *character_name)
{    
    Node *character = FindCharacter(data, character_name);
 
    if (character == nullptr) 
    {
        CMD_SpeakAndWait(UNDEFINED_CHARACTER, character_name);//TODO: phrases
        return;
    }
    CMD_SpeakAndWait(TRY_GUESS_CHARACTER, character_name);//TODO: phrases

    DefineCharacter(data, character, true);
}

void SimAndDiffsCharacters(Node *data, const char *first_name, const char *second_name)
{
    Node *first_character  = FindCharacter(data, first_name );
    Node *second_character = FindCharacter(data, second_name);


    if (strcmp(first_name, second_name) == 0)
    {
        CMD_SpeakAndWait(SAME_CHARACTERS);
        return;
    }
    if (first_character == nullptr)
    {
        CMD_SpeakAndWait(UNDEFINED_CHARACTER, first_name);
        return;
    }
    if (second_character == nullptr)
    {
        CMD_SpeakAndWait(UNDEFINED_CHARACTER, second_name);
        return;
    }

    Node *FirstCommonNode = SearchCommonNode(first_character, second_character);

    if (FirstCommonNode == data)
    {
        CMD_SpeakAndWait(NO_SIMILAR);
    }
    else
    {
        CMD_SpeakAndWait(BOTH_CHARACTERS);
        DefineCharacter(data, FirstCommonNode);
    }

    CMD_SpeakAndWait(FIRST_CHARACTERISTICS, first_name);
    DefineCharacter(FirstCommonNode, first_character);

    CMD_SpeakAndWait(SECOND_CHARACTERISTICS, second_name);
    DefineCharacter(FirstCommonNode, second_character);
}

void DestructData(Node *data_tree)
{
    if (data_tree != nullptr)
    {
        if (data_tree->left  != nullptr) {DestructData(data_tree->left);}
        if (data_tree->right != nullptr) {DestructData(data_tree->right);}

        free(data_tree->data);
    }
    if (data_tree->parent == nullptr)
    {
        treeDtor(data_tree);
    }
}

//----------------------------------------------------------------------

static void SkipSymbolsBeforeNextToSymbol(char sym, int *position, char *InputData)
{
    while (InputData[*position - 1] != sym && InputData[*position] != '\0') 
    {
        (*position)++;
    }
}

static ProgMode AskProgMode(pid_t speak_pid)
{
    char ans = 0;
    scanf("%c", &ans);
    while(getchar() != '\n');
    
    kill(speak_pid, SIGKILL);

    switch (tolower(ans))
    {
        case 'g':
            return Guessing;
        case 'd':
            return Definitions;
        case 'c':
            return Compare;
        default:
            speak_pid = CMD_Speak(ASK_PROGRAM_MODE, ans);
            return AskProgMode(speak_pid);
    }    
}

static void DefineCharacter(Node *tree, Node *node, bool last_def)//TODO: without recursive
{
    if (node->parent != nullptr && node != tree)
    {  
        DefineCharacter(tree, node->parent, false);

        CMD_SpeakAndWait("%s%s", IsPrevAnsYes(node) ? "" : "not ", node->parent->data);

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

static int InitTree(Node **node, int position, char *InputData, int depth)
{
    assert (node      != nullptr);
    assert (InputData != nullptr);

    int start_pos = position;

    position++;
    SkipSymbolsBeforeNextToSymbol(IDENT_DATA_SYM, &position, InputData);
    InputData[position - 1] = '\0';

    char *temp_ptr = strdup(InputData + start_pos);//TODO: without strdup

    *node = treeCtor(temp_ptr, depth);

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

    SkipSymbolsBeforeNextToSymbol(CLOSE_NODE_SYM, &position, InputData); 
    return position;
}

static void CompleteGuessing()
{
    CMD_SpeakAndWait(COMPLETE_GUESSING);
}

static void FailGuessing(Node *node)
{
    pid_t PID = CMD_Speak(FAIL_GUESSING);
    
    char character[MAX_NODE_NAME_LEN + 1] = "";
    GetLine(character);

    kill(PID, SIGKILL);

    PID = CMD_Speak(ANSWER_ABOUT_DIFFERENCES, character, node->data, character);
    
    char difference[MAX_NODE_NAME_LEN + 1] = "";
    GetLine(difference);

    kill(PID, SIGKILL);

    addToLeft(node, node->data);

    char *temp = strdup(character);
    addToRight(node, temp);

    temp = strdup(difference);
    node->data = temp;
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