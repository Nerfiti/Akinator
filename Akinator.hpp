#ifndef AKINATOR_HPP
#define AKINATOR_HPP

#include "Tree.hpp"

const int MAX_NODE_NAME_LEN = 100;

enum Answers
{
    YES,
    NO
};

enum ProgMode
{
    Guessing,
    Definitions,
    Compare,
    WrongMode
};

ProgMode GetProgramMode        (const int argc, const char *argv[]);
void     OptionalPrint         (FILE       *stream,   Node *node, PrintMode mode, int space = 0);
void     OptionalPrint         (const char *filename, Node *node, PrintMode mode, int space = 0);
Node    *InitData              (const char *input_filename);
Answers  ProcessingAnswer      (pid_t speak_pid);
void     GuessingCharacters    (Node *data);
void     GetDefinition         (Node *data, const char *character_name);
void     SimAndDiffsCharacters (Node *data, const char *first_name, const char * second_name);

#endif //AKINATOR_HPP