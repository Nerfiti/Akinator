#ifndef PHRASES_HPP
#define PHRASES

//----------------------------------------------------------------------------------------------------------------

static const int NUMBER_OF_GREETINGS_PARTS = 5;
static const int NUMBER_OF_FAREWELLS_PARTS = 6;

//----------------------------------------------------------------------------------------------------------------

static const char *DATABASE_CHANGES = "Do you want to save changes of database? [Y/N]\n";
static const char *UNDEFINED_ANSWER = "I don't understand (you say \"%c\"). Can you, please, repeat your answer?\n";

static const char *ONE_MORE_TIME = "Do you want to make sure that I am the smartest one more time? [Y/N]\n";

static const char *ASK_PROGRAM_MODE = "I don't know, which program mode you want to use. Can you say me? Guessing characters, definitions or comparing characters? [G/D/C]";

//----------------------------------------------------------------------------------------------------------------
//GREETING
//----------------------------------------------------------------------------------------------------------------

static const char *GREETINGS[NUMBER_OF_GREETINGS_PARTS] = 
{
    "Hello. I am a super akinator. ",
    "Even a little better than the usual akinator. ",
    "I am the smartest akinator on earth, ",
    "despite the fact that I don't know much. ",
    "Now you will be convinced of this.\n"
};

//----------------------------------------------------------------------------------------------------------------
//FAREWELL
//----------------------------------------------------------------------------------------------------------------

static const char *FAREWELLS[NUMBER_OF_FAREWELLS_PARTS]= 
{
    "What, are you leaving already? ",
    "It's a pity, ",
    "I didn't show you my whole mind. ",
    "But never mind, ",
    "if you study, ",
    "you will also someday become as smart as me.\n"
};

//----------------------------------------------------------------------------------------------------------------
//GUESSING
//----------------------------------------------------------------------------------------------------------------

static const char *ANSWERS_ABOUT_CHARACTER = "Is your character %s? [Y/N]\n";
static const char *UNDEFINED_CHARACTER = "\"%s\" not found.\n";
static const char *TRY_GUESS_CHARACTER = "%s is ";

static const char *COMPLETE_GUESSING = "Of course yes, I told you, I'm smart\n";
static const char *FAIL_GUESSING = "I know who it is, but I just forgot. Could you remind me who it is?\n";

//----------------------------------------------------------------------------------------------------------------
//COMPARING
//----------------------------------------------------------------------------------------------------------------

static const char *ASK_ABOUT_CHARATERS_FOR_COMPARING = "Which characters do you want to compare?\n";

static const char *SAME_CHARACTERS = "There are the same characters.\n";
static const char *NO_SIMILAR = "These characters don't have similar characteristics.\n";
static const char *BOTH_CHARACTERS = "Both of the characters are ";
static const char *FIRST_CHARACTERISTICS = "But %s is ";
static const char *SECOND_CHARACTERISTICS = "And %s is ";

static const char *ANSWER_ABOUT_DIFFERENCES = "And how does %s differ from %s?\n" "%s is: ";

//----------------------------------------------------------------------------------------------------------------
//DEFINITION
//----------------------------------------------------------------------------------------------------------------

static const char *ASK_ABOUT_CHARACTER_FOR_DEFINITION = "For which character do you want to get a definition?\n";

//----------------------------------------------------------------------------------------------------------------

#endif //PHRASES_HPP