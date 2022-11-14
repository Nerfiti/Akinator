#include <cstdio>

#include "Akinator.h"

int main(const int argc, const char *argv[])
{
    initLog();

    ProgMode mode = GetProgramMode(argc, argv);
    Answers ans = YES;

    const char input_filename[] = "./data.aki";
    treeNode *data = InitData(input_filename);

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
                printf("For which character do you want to get a definition?\n");

                char character_name[MAX_NODE_NAME_LEN] = "";
                gets(character_name);

                GetDefinition(data, character_name);
                break;
            }
            case Compare:
            {
                printf("Which characters do you want to compare?\n");

                char  first_character_name[MAX_NODE_NAME_LEN] = "";
                char second_character_name[MAX_NODE_NAME_LEN] = "";
                gets(first_character_name);
                gets(second_character_name);

                CompareCharacters(data, first_character_name, second_character_name);
                break;
            }
            default:
            {
                break;
            }
        }
        printf("Do you want to make sure that I am the smartest one more time?\n");
        ans = ProcessingAnswer();
    }
    OptionalPrint(input_filename, data, PRE_ORDER);
    treeGraphDump(data);

    closeLog();
    return 0;
}