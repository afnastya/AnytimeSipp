#include <iostream>
#include <filesystem>
#include <optional>
#include <string>
#include "mission.h"

void ProcessTask(const char *filename, int logLevel, double hweight = 1);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "ERROR: Input file or directory is not specified\n";
        return 1;
    }

    std::optional<int> logLevel;
    double hweight = 1;
    for (int arg = 2; arg < argc; ++arg) {
        if (std::string(argv[arg]) == "-l") {
            try {
                logLevel = std::stoi(argv[arg + 1]);
                ++arg;
            } catch (...) {
            }
        } else if (std::string(argv[arg]) == "-w") {
            try {
                hweight = std::stod(argv[arg + 1]);
                ++arg;
            } catch (...) {
            }
        }
    }

    std::filesystem::path tasks_path(argv[1]);
    if (!std::filesystem::is_directory(tasks_path)) {
        ProcessTask(tasks_path.c_str(), logLevel ? logLevel.value() : 2, hweight);
        return 0;
    }

    for (const auto& dir_entry : std::filesystem::directory_iterator{tasks_path}) {
        const char *filename = dir_entry.path().c_str();

        ProcessTask(filename, logLevel ? logLevel.value() : 0, hweight);
    }
}


void ProcessTask(const char *filename, int logLevel, double hweight) {
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

    mission.SetHweight(hweight);

    mission.RunTask();

    if (logLevel > 0) {
        mission.WriteResultToConsole();
    }

    mission.SaveResultToOutputDocument();
}