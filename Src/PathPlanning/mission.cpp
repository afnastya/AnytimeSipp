#include "mission.h"
#include "tinyxml2.h"

Mission::Mission(const char* taskFile) : fileName(taskFile) {
}

Mission::~Mission() {
    delete search;
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
    search = new Sipp();

    searchResult = search->startSearch(map, options);
    return true;
}

void Mission::SaveResultToOutputDocument() {
    std::string outputFile(fileName);
    outputFile.insert(outputFile.size() - 4, "_log");


    tinyxml2::XMLElement *root = inputDoc.FirstChildElement("root");
    tinyxml2::XMLElement *log = root->InsertNewChildElement("log");
    tinyxml2::XMLElement *summary = log->InsertNewChildElement("summary");
    summary->SetAttribute("pathlength", searchResult.pathlength);
    summary->SetAttribute("numberofsteps", searchResult.numberofsteps);
    summary->SetAttribute("searchtime", searchResult.searchtime);

    SavePathToOutputDocument(log);

    inputDoc.SaveFile(&outputFile[0]);
}

void Mission::SavePathToOutputDocument(tinyxml2::XMLElement *log) {
    tinyxml2::XMLElement *path = log->InsertNewChildElement("path");
    for (auto& node : *searchResult.hppath) {
        auto *point = path->InsertNewChildElement("point");
        point->SetAttribute("x", node.j + 1);
        point->SetAttribute("y", node.i + 1);
        point->SetAttribute("time", node.g);
    }
}

void Mission::WriteResultToConsole() {
    std::cout << (searchResult.pathfound ? "Path is found" : "Path is not found!") << "\n";
    std::cout << "Path Length: " << searchResult.pathlength << "\n";
}