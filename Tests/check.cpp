#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include "../Src/PathPlanning/tinyxml2.h"
#include "path.h"

bool check(const char *filename) {
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename) != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::invalid_argument("Error occured while openning file\n");
    }

    tinyxml2::XMLElement *element = doc.FirstChildElement("root")->FirstChildElement("map")->FirstChildElement();
    int width, height, start_i, start_j, goal_i, goal_j;
    std::vector<int*> data_ptrs = {&width, &height, &start_j, &start_i, &goal_j, &goal_i};
    for (auto data_ptr : data_ptrs) {
        element->QueryIntText(data_ptr);
        element = element->NextSiblingElement();
    }

    for (int i = 2; i < data_ptrs.size(); i++) {
        *data_ptrs[i] -= 1;
    }

    std::vector<std::vector<int>> map(height, std::vector<int>(width));
    tinyxml2::XMLElement *row = element->FirstChildElement();
    for (int i = 0; i < height; ++i) {
        std::stringstream stream(row->FirstChild()->Value());
        for (int j = 0; j < width; ++j) {
            stream >> map[i][j];
        }
        row = row->NextSiblingElement();
    }

    std::vector<std::vector<PathComponent>> dynamicObstacles;
    element = element->NextSiblingElement();
    for (auto obstacle = element->FirstChildElement(); obstacle != nullptr; obstacle = obstacle->NextSiblingElement()) {
        dynamicObstacles.emplace_back();
        for (auto point = obstacle->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
            int x, y;
            double time;

            point->QueryIntAttribute("x", &x);
            point->QueryIntAttribute("y", &y);
            point->QueryDoubleAttribute("time", &time);

            dynamicObstacles.back().push_back({x - 1, y - 1, time});
        }
    }

    std::vector<PathComponent> path;
    element = doc.FirstChildElement("root")->FirstChildElement("log")->FirstChildElement("path");
    for (auto point = element->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
        int x, y;
        double time;

        point->QueryIntAttribute("x", &x);
        point->QueryIntAttribute("y", &y);
        point->QueryDoubleAttribute("time", &time);

        path.push_back({x - 1, y - 1, time});
    }


    for (int i = 1; i < path.size(); ++i) {
        PathSegment path_segment = {path[i - 1], path[i]};

        if (PathsIntersect(path_segment, dynamicObstacles)) {
            return false;
        }
    }

    return true;
}


int main(int argc, char **argv) {
    std::filesystem::path logs_directory(argv[1]);
    for (const auto& dir_entry : std::filesystem::directory_iterator{logs_directory}) {
        std::string filename = dir_entry.path().string();
        try {
            std::cout << filename << "\t" << (check(filename.c_str()) ? "correct" : "wrong") << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
