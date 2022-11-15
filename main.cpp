#include <cstdio>

#include "Akinator.hpp"
#include "logs.hpp"
#include "MyGeneralFunctions.hpp"

#define END_PROGRAM(code)   \
    closeLog();             \
    return code                

int main(const int argc, const char *argv[])
{
    initLog();

    ProgMode mode = GetProgramMode(argc, argv);
    Answers ans = YES;

    const char input_filename[] = "./data.aki";
    Node *data = InitData(input_filename);
    
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
                CMD_Speak("For which character do you want to get a definition?\n");

                char character_name[MAX_NODE_NAME_LEN] = "";
                gets(character_name);

                GetDefinition(data, character_name);
                break;
            }
            case Compare:
            {
                CMD_Speak("Which characters do you want to compare?\n");

                char  first_character_name[MAX_NODE_NAME_LEN] = "";
                char second_character_name[MAX_NODE_NAME_LEN] = "";
                gets(first_character_name);
                gets(second_character_name);

                SimAndDiffsCharacters(data, first_character_name, second_character_name);
                break;
            }
            default:
            {
                CMD_Speak("Wrong program mode.\n");
                END_PROGRAM(0);
            }
        }
        CMD_Speak("Do you want to make sure that I am the smartest one more time?\n");
        ans = ProcessingAnswer();
    }
    
    OptionalPrint(input_filename, data, PRE_ORDER);
    treeGraphDump(data);

    END_PROGRAM(0);
}