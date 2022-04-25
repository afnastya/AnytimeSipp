#include "mission.h"
#include "tinyxml2.h"

Mission::Mission(const char* taskFile, int _logLevel)
    : fileName(taskFile), logLevel(_logLevel) {
        if (logLevel >= 0 || logLevel == -2) {
            std::cout << fileName << (logLevel == -2 ? "," : "\n");
        }
}

Mission::~Mission() {
}

bool Mission::ParseTask() {
    if (inputDoc.LoadFile(fileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }

    tinyxml2::XMLElement *root = inputDoc.FirstChildElement("root");
    if (!root) {
        std::cout << "ERROR: no root tag in XML input file\n";
        return false;
    }

    tinyxml2::XMLElement *map_tag = root->FirstChildElement("map");
    if (!map.getMap(map_tag)) {
        std::cout << "ERROR: appeared while parsing map tag\n";
        return false;
    }

    tinyxml2::XMLElement *options_tag = root->FirstChildElement("options");
    if (!options.getOptions(options_tag)) {
        std::cout << "ERROR: appeared while parsing options tag\n";
        return false;
    }

    return true;
}

bool Mission::RunTask() {
    search = std::make_shared<Sipp>(logLevel);

    searchResult = search->startSearch(map, options);

    if (logLevel == -2) {
        WriteTestResult();
    }

    return true;
}

void Mission::SaveResultToOutputDocument() {
    std::string outputFile(fileName);
    outputFile.insert(outputFile.rfind('/'), "/../logs");
    outputFile.insert(outputFile.size() - 4, "_log");

    tinyxml2::XMLElement *root = inputDoc.FirstChildElement("root");
    tinyxml2::XMLElement *log = root->InsertNewChildElement("log");
    tinyxml2::XMLElement *summary = log->InsertNewChildElement("summary");
    summary->SetAttribute("pathlength", searchResult.pathlength);
    summary->SetAttribute("numberofsteps", searchResult.numberofsteps);
    summary->SetAttribute("searchtime", searchResult.searchtime);

    if (logLevel >= 3) {
        SavePathToOutputDocument(log, *searchResult.lppath);
    }
    SavePathToOutputDocument(log, *searchResult.hppath);

    inputDoc.SaveFile(&outputFile[0]);
    if (logLevel > 0) {
        std::cout << "Results are saved to " << outputFile << std::endl;
    }
}

void Mission::SavePathToOutputDocument(tinyxml2::XMLElement *log, const std::list<Node>& nodesPath) {
    tinyxml2::XMLElement *path = log->InsertNewChildElement("path");
    // path->SetAttribute("hweight", options.hweight);
    for (auto& node : nodesPath) {
        auto *point = path->InsertNewChildElement("point");
        point->SetAttribute("x", node.j + 1);
        point->SetAttribute("y", node.i + 1);
        point->SetAttribute("time", node.g);
    }
}

void Mission::WriteResultToConsole() {
    std::cout << "Path is " << (searchResult.pathfound ? "" : "not ") << "found!\n";
    if (searchResult.pathfound) {
        std::cout << "Path Length: " << searchResult.pathlength << "\n";
    }
    std::cout << "Nodes created: " << searchResult.nodescreated << "\n";
    std::cout << "Number of steps: " << searchResult.numberofsteps << "\n";
    std::cout << "Search time: " << searchResult.searchtime << "\n";
}

void Mission::WriteTestResult() {
    std::cout << map.getWidth() << "," << map.getHeight() << ","
              << map.dynamicObstacles.size() << "," 
              << options.hweight << ","
              << searchResult.pathfound << ","
              << searchResult.pathlength << ","
              << searchResult.nodescreated << ","
              << searchResult.numberofsteps << ","
              << searchResult.searchtime << "\n";
}