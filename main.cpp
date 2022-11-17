#include <cstdio>
#include <signal.h>

#include "Akinator.hpp"
#include "logs.hpp"
#include "MyGeneralFunctions.hpp"

int main(const int argc, const char *argv[])
{
    initLog();

    ProgMode mode = GetProgramMode(argc, argv);
    const char input_filename[] = "./data.aki";

    Node *data = Run_Akinator(input_filename, mode);

    treeGraphDump(data);
    DestructData(data);
    closeLog();
    return 0;
}