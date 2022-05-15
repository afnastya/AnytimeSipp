#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include "../Src/PathPlanning/tinyxml2.h"
#include "path.h"

struct Path {
    std::vector<PathComponent> path;
    double bound;
    int pathlength;
};

struct Data {
    int width, height;
    int start_i, start_j;
    int goal_i, goal_j;
    int pathlength;
    std::vector<std::vector<int>> map;
    std::vector<std::vector<PathComponent>> obstacles;
    std::vector<Path> paths;
};

void ParseData(const char* filename, Data& data);

bool Check(const char* filename, std::stringstream& error) {
    Data data;
    ParseData(filename, data);

    if (data.paths.empty()) {
        if (data.pathlength != 0) {
            error << "no paths in log, even though pathlength value is not 0 in summary";
            return false;
        }
        throw std::runtime_error("path not found");
    }

    for (auto& path_structure : data.paths) {
        if (data.pathlength == 0) {
            throw std::runtime_error("path not found");
        }

        auto& path = path_structure.path;

        // check suboptimality bound correctness
        if (path_structure.pathlength > ceill(path_structure.bound * data.pathlength)) {
            error << "suboptimality bound is not correct: " << path_structure.pathlength << " > "
                  << path_structure.bound << " * " << data.pathlength;
            return false;
        }

        if (path_structure.pathlength != path_structure.path.back().time) {
            error << "value of pathlength differs from goal point time";
            return false;
        }

        // check path representation & intersection of path and static obstacles
        for (uint32_t i = 1; i < path.size(); ++i) {
            int distance = abs(path[i].x - path[i - 1].x) + abs(path[i].y - path[i - 1].y);
            if (distance != 0 && distance != path[i].time - path[i - 1].time) {
                error << "incorrect representation of path " << PathSegment{path[i - 1], path[i]};
                return false;
            }

            static auto sign = [] (const int x) { return (x > 0) - (0 > x); };
            int dx = sign(path[i].x - path[i - 1].x), dy = sign(path[i].y - path[i - 1].y);

            for (int x = path[i - 1].x, y = path[i - 1].y; x != path[i].x || y != path[i].y; x += dx, y += dy) {
                if (data.map[y][x]) {
                    error << "path intersects static obstacle " << x << " " << y << " "
                          << PathSegment{path[i - 1], path[i]};
                    return false;
                }
            }
        }

        // check intersection of path and dynamic obstacles
        for (uint32_t i = 1; i < path.size(); ++i) {
            PathSegment path_segment = {path[i - 1], path[i]};

            if (PathsIntersect(path_segment, data.obstacles, error)) {
                error << "path intersects obstacle path";
                return false;
            }
        }
    }

    return true;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Logs' directory is not specified" << std::endl;
        return 1;
    }

    int wrong_cnt = 0, all_cnt = 0;

    std::filesystem::path logs_directory(argv[1]);
    for (const auto& dir_entry : std::filesystem::directory_iterator{logs_directory}) {
        ++all_cnt;
        std::string filename = dir_entry.path().string();
        try {
            std::stringstream error("");
            if (!Check(filename.c_str(), error)) {
                ++wrong_cnt;
                std::cout << filename << "\tWrong:\t" << error.str() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << filename << "\t" << e.what() << std::endl;
        }
    }

    std::cout << wrong_cnt << " tests are wrong out of " << all_cnt << std::endl;
    return (wrong_cnt > 0 ? 1 : 0);
}


void ParseData(const char* filename, Data& data) {
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename) != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::invalid_argument("Error occured while openning file");
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
            int time;

            point->QueryIntAttribute("x", &x);
            point->QueryIntAttribute("y", &y);
            point->QueryIntAttribute("time", &time);

            data.obstacles.back().push_back({x - 1, y - 1, time});
        }
    }

    element = doc.FirstChildElement("root")->FirstChildElement("log");
    element->FirstChildElement("summary")->QueryIntAttribute("pathlength", &data.pathlength);
    
    element = element->FirstChildElement("path");
    for (; element != nullptr; element = element->NextSiblingElement()) {
        data.paths.emplace_back();
        element->QueryDoubleAttribute("suboptimalitybound", &data.paths.back().bound);
        element->QueryIntAttribute("pathlength", &data.paths.back().pathlength);
        for (auto point = element->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
            int x, y;
            int time;

            point->QueryIntAttribute("x", &x);
            point->QueryIntAttribute("y", &y);
            point->QueryIntAttribute("time", &time);

            data.paths.back().path.push_back({x - 1, y - 1, time});
        }
    }
}