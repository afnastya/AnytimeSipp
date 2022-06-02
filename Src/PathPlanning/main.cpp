#include <iostream>
#include <filesystem>
#include <optional>
#include <string>
#include <boost/program_options.hpp>
#include "mission.h"

namespace po = boost::program_options;

void ProcessTask(const char *filename, int logLevel, std::optional<double> hweight);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "ERROR: Input file or directory is not specified\n";
        return 1;
    }

    po::options_description desc("Allowed options");
    desc.add_options()
        ("logLevel,l", po::value<int>(), "log level")
        ("hweight,w", po::value<double>(), "initial hweight for anytime sipp")
    ;

    po::variables_map vm;
    po::parsed_options parser = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    po::store(parser, vm);
    po::notify(vm);

    std::filesystem::path tasks_path(argv[1]);
    if (!std::filesystem::is_directory(tasks_path)) {
        try {
            ProcessTask(
                tasks_path.c_str(),
                vm.count("logLevel") ? vm["logLevel"].as<int>() : 2,
                vm.count("hweight") ? std::optional(vm["hweight"].as<double>()) : std::nullopt
            );
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }

        return 0;
    }

    for (const auto& dir_entry : std::filesystem::directory_iterator{tasks_path}) {
        const char *filename = dir_entry.path().c_str();

        try {
            ProcessTask(
                filename,
                vm.count("logLevel") ? vm["logLevel"].as<int>() : 0,
                vm.count("hweight") ? std::optional(vm["hweight"].as<double>()) : std::nullopt
            );
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}


void ProcessTask(const char *filename, int logLevel, std::optional<double> hweight) {
    Mission mission(filename, logLevel);

    if (logLevel > 1) {
        std::cout << "Parsing input file\n";
    }

    mission.ParseTask();

    if (logLevel > 1) {
        std::cout << "Parsing is completed\n";
    }

    mission.SetOptions(hweight);
    mission.RunTask();

    if (logLevel > 0) {
        mission.WriteResultToConsole();
    }

    mission.SaveResultToOutputDocument();
}