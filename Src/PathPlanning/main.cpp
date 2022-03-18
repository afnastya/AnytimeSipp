#include <iostream>
#include "mission.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "ERROR: Input file is not specified\n";
    }

    Mission mission(argv[1]);

    std::cout << "Parsing input file\n";

    if (!mission.ParseTask()) {
        return 0;
    }

    std::cout << "Parsing is completed\n";

    mission.RunTask();
    mission.WriteResultToConsole();
    mission.SaveResultToOutputDocument();
}