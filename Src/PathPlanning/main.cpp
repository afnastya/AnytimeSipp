#include <iostream>
#include <filesystem>
#include "mission.h"

void ProcessTask(const char *filename, bool console_log = false);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "ERROR: Input file or directory is not specified\n";
    }

    std::filesystem::path tasks_path(argv[1]);
    if (!std::filesystem::is_directory(tasks_path)) {
        ProcessTask(tasks_path.c_str(), true);
        return 0;
    }

    for (const auto& dir_entry : std::filesystem::directory_iterator{tasks_path}) {
        const char *filename = dir_entry.path().c_str();

        ProcessTask(filename);
    }
}


void ProcessTask(const char *filename, bool console_log) {
    Mission mission(filename);

    if (console_log) {
        std::cout << "Parsing input file\n";
    }

    if (!mission.ParseTask()) {
        return;
    }

    if (console_log) {
        std::cout << "Parsing is completed\n";
    }

    mission.RunTask();

    if (console_log) {
        mission.WriteResultToConsole();
    }

    mission.SaveResultToOutputDocument();
}