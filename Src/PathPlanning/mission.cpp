#include "mission.h"
#include "tinyxml2.h"
#include <sstream>

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

void Mission::SetOptions(double hweight, bool anytime) {
    if (hweight < 1.0) {
        throw std::invalid_argument("Invalid value of heuristic weight");
    }

    options.anytime = anytime;
    options.hweight = hweight;
}

void Mission::CreateSearch() {
    if (options.anytime) {
        search = std::make_shared<Anytime>(logLevel);
    } else {
        search = std::make_shared<Sipp>(logLevel);
    }
}

bool Mission::RunTask() {
    if (!search) {
        CreateSearch();
    }

    searchResult = search->startSearch(map, options);

    for (auto result = search->getResults(); result; result = search->getResults()) {
        searchResults.push_back(result);
    }

    if (logLevel == -2) {
        WriteTestResult();
    }

    return true;
}

void Mission::SaveResultToOutputDocument() {
    std::string tmp(fileName);
    tmp.insert(tmp.rfind('/'), std::string("/../logs") + (options.anytime ? "_anytime" : ""));
    tmp.erase(tmp.rfind('.'));
    std::stringstream outputFile("");
    outputFile << tmp;
    if (options.anytime) {
        outputFile << "_anytime_" << options.hweight;
    }
    outputFile << "_log.xml" ;

    tinyxml2::XMLElement *root = inputDoc.FirstChildElement("root");
    tinyxml2::XMLElement *log = root->InsertNewChildElement("log");
    tinyxml2::XMLElement *summary = log->InsertNewChildElement("summary");

    if (searchResult.pathlength > 0) {
        for (auto result : searchResults) {
            SavePathToOutputDocument(log, *result);
        }
    }

    summary->SetAttribute("pathlength", searchResult.pathlength);
    summary->SetAttribute("numberofsteps", searchResult.numberofsteps);
    summary->SetAttribute("nodescreated", searchResult.nodescreated);
    if (searchResult.numberofsearches != 0) {
        summary->SetAttribute("numberofsearches", searchResult.numberofsearches);
    }
    summary->SetAttribute("searchtime", searchResult.searchtime);

    inputDoc.SaveFile(outputFile.str().c_str());
    if (logLevel > 0) {
        std::cout << "Results are saved to " << outputFile.str() << std::endl;
    }
}

void Mission::SavePathToOutputDocument(tinyxml2::XMLElement *log, const SearchResult& sresult) {
    tinyxml2::XMLElement *path = log->InsertNewChildElement("path");
    path->SetAttribute("suboptimalitybound", sresult.suboptimalityBound);
    path->SetAttribute("pathlength", sresult.pathlength);
    path->SetAttribute("nodescreated", sresult.nodescreated);
    path->SetAttribute("numberofsteps", sresult.numberofsteps);
    path->SetAttribute("searchtime", sresult.searchtime);

    for (auto& node : *sresult.hppath) {
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
              << map.dynamicObstacles.size() << ",";
    if (options.anytime) {
        std::cout << options.hweight << ","
                  << searchResult.suboptimalityBound << ",";
    }

    // searchResult.numberofsteps = 0;
    // for (auto result : searchResults) {
    //     searchResult.numberofsteps += result->numberofsteps;
    // }

    std::cout << searchResult.pathfound << ","
              << searchResult.pathlength << ","
              << searchResult.nodescreated << ","
              << searchResult.numberofsteps << ","
              << searchResult.searchtime;

    if (options.anytime) {
        std::cout << "," << searchResult.numberofsearches;
        if (!searchResults.empty()) {
            std::cout << "," << searchResults[0]->searchtime;
        }
    }
    std::cout << "\n";
}