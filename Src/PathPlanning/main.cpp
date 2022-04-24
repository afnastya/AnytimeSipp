#include <iostream>
#include <filesystem>
#include <optional>
#include <string>
#include "mission.h"

void ProcessTask(const char *filename, int logLevel = -1);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "ERROR: Input file or directory is not specified\n";
        return 1;
    }

    std::optional<int> logLevel;
    try {
        if (argc > 2) {
            logLevel = std::stoi(argv[2]);
        }
    } catch (...) {
    }

    std::filesystem::path tasks_path(argv[1]);
    if (!std::filesystem::is_directory(tasks_path)) {
        ProcessTask(tasks_path.c_str(), logLevel ? logLevel.value() : 2);
        return 0;
    }

    for (const auto& dir_entry : std::filesystem::directory_iterator{tasks_path}) {
        const char *filename = dir_entry.path().c_str();

        ProcessTask(filename, logLevel ? logLevel.value() : 0);
    }
}


void ProcessTask(const char *filename, int logLevel) {
    Mission mission(filename, logLevel);

    if (logLevel > 1) {
        std::cout << "Parsing input file\n";
    }

    if (!mission.ParseTask()) {
        return;
    }

    if (logLevel > 1) {
        std::cout << "Parsing is completed\n";
    }

    mission.RunTask();

    if (logLevel > 0) {
        mission.WriteResultToConsole();
    }

    mission.SaveResultToOutputDocument();
}