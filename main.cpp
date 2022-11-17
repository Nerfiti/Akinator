#include <cstdio>
#include <signal.h>

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
    Answers  ans = YES;

    const char input_filename[] = "./data.aki";
    Node *data = InitData(input_filename);
    
    treeGraphDump(data);
    
    while (ans == YES)//TODO: function
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
                pid_t PID = CMD_Speak("For which character do you want to get a definition?\n");//TODO: phrases

                char character_name[MAX_NODE_NAME_LEN] = "";
                scanf("%[^\n]", character_name);
                getchar();

                kill(PID, SIGKILL);                                

                GetDefinition(data, character_name);
                break;
            }
            case Compare:
            {
                pid_t PID = CMD_Speak("Which characters do you want to compare?\n");//TODO: phrases

                char  first_character_name[MAX_NODE_NAME_LEN] = "";
                char second_character_name[MAX_NODE_NAME_LEN] = "";
                scanf("%[^\n]",  first_character_name);
                getchar();
                scanf("%[^\n]", second_character_name);
                getchar();

                kill(PID, SIGKILL);

                SimAndDiffsCharacters(data, first_character_name, second_character_name);
                break;
            }
            default:
            {
                CMD_Speak("Wrong program mode.\n");//TODO: phrases
                END_PROGRAM(0);
            }
        }
        pid_t PID = CMD_Speak("Do you want to make sure that I am the smartest one more time?\n");//TODO: phrases
        ans = ProcessingAnswer(PID);
    }
    
    OptionalPrint(input_filename, data, PRE_ORDER);
    treeGraphDump(data);

    END_PROGRAM(0);
}