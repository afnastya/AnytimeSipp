#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include "../Src/PathPlanning/tinyxml2.h"
#include "path.h"

struct Data {
    int width, height;
    int start_i, start_j;
    int goal_i, goal_j;
    std::vector<std::vector<int>> map;
    std::vector<std::vector<PathComponent>> obstacles;
    std::vector<PathComponent> path;
};

void ParseData(const char* filename, Data& data);

bool Check(const char* filename) {
    Data data;
    ParseData(filename, data);

    for (uint32_t i = 1; i < data.path.size(); ++i) {
        auto& path = data.path;
        int distance = abs(path[i].x - path[i - 1].x) + abs(path[i].y - path[i - 1].y);
        if (distance != 0 && distance != path[i].time - path[i - 1].time) {
            std::cout << "Incorrect representation of path " << PathSegment{path[i - 1], path[i]} << std::endl;
            return false;
        }

        static auto sign = [] (const int x) { return (x > 0) - (0 > x); };
        int dx = sign(path[i].x - path[i - 1].x), dy = sign(path[i].y - path[i - 1].y);

        for (int x = path[i - 1].x, y = path[i - 1].y; x != path[i].x || y != path[i].y; x += dx, y += dy) {
            if (data.map[y][x]) {
                std::cout << "Path intersects static obstacle " << x << " " << y << " "
                          << PathSegment{path[i - 1], path[i]} << std::endl;
                return false;
            }
        }
    }

    for (uint32_t i = 1; i < data.path.size(); ++i) {
        PathSegment path_segment = {data.path[i - 1], data.path[i]};

        if (PathsIntersect(path_segment, data.obstacles)) {
            std::cout << "Path intersects obstacle path" << std::endl;
            return false;
        }
    }

    return true;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Logs' directory is not specified" << std::endl;
        return 1;
    }

    std::filesystem::path logs_directory(argv[1]);
    for (const auto& dir_entry : std::filesystem::directory_iterator{logs_directory}) {
        std::string filename = dir_entry.path().string();
        try {
            std::cout << filename << "\t" << (Check(filename.c_str()) ? "correct" : "wrong") << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}



void ParseData(const char* filename, Data& data) {
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename) != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::invalid_argument("Error occured while openning file\n");
    }

    tinyxml2::XMLElement *element = doc.FirstChildElement("root")->FirstChildElement("map")->FirstChildElement();
    std::vector<int*> data_ptrs = {
        &data.width, &data.height,
        &data.start_j, &data.start_i,
        &data.goal_j, &data.goal_i
    };
    for (auto data_ptr : data_ptrs) {
        element->QueryIntText(data_ptr);
        element = element->NextSiblingElement();
    }

    for (uint32_t i = 2; i < data_ptrs.size(); ++i) {
        *data_ptrs[i] -= 1;
    }

    data.map.resize(data.height, std::vector<int>(data.width));
    tinyxml2::XMLElement *row = element->FirstChildElement();
    for (int i = 0; i < data.height; ++i) {
        std::stringstream stream(row->FirstChild()->Value());
        for (int j = 0; j < data.width; ++j) {
            stream >> data.map[i][j];
        }
        row = row->NextSiblingElement();
    }

    element = element->NextSiblingElement();
    for (auto obstacle = element->FirstChildElement(); obstacle != nullptr; obstacle = obstacle->NextSiblingElement()) {
        data.obstacles.emplace_back();
        for (auto point = obstacle->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
            int x, y;
            double time;

            point->QueryIntAttribute("x", &x);
            point->QueryIntAttribute("y", &y);
            point->QueryDoubleAttribute("time", &time);

            data.obstacles.back().push_back({x - 1, y - 1, time});
        }
    }

    element = doc.FirstChildElement("root")->FirstChildElement("log")->FirstChildElement("path");
    for (auto point = element->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
        int x, y;
        double time;

        point->QueryIntAttribute("x", &x);
        point->QueryIntAttribute("y", &y);
        point->QueryDoubleAttribute("time", &time);

        data.path.push_back({x - 1, y - 1, time});
    }
}