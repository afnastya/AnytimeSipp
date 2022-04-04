#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include "../Src/PathPlanning/tinyxml2.h"
#include "path.h"

void SetStartFinish(tinyxml2::XMLElement *map, std::vector<std::vector<bool>>& grid);
void SetGrid(tinyxml2::XMLElement *grid, std::vector<std::vector<bool>>& grid_matrix);
void SetDynamicObstacles(tinyxml2::XMLElement *obstacles, std::vector<std::vector<bool>>& grid);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Maps directory is not specified" << std::endl;
        return 0;
    }

    int test_id = 0;
    std::filesystem::path maps_directory(argv[1]);
    for (const auto& dir_entry : std::filesystem::directory_iterator{maps_directory}) {
        ++test_id;
        std::cout << dir_entry.path().string() << " ";
        std::ifstream input(dir_entry.path());

        int width, height;
        std::string unused_line;
        std::getline(input, unused_line);
        input >> unused_line >> width >> unused_line >> height >> unused_line;
        std::getline(input, unused_line);

        std::vector<std::vector<bool>> grid_matrix(height, std::vector<bool>(width));

        char map_element;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                input >> map_element;
                grid_matrix[i][j] = (map_element != '.');
            }
        }

        tinyxml2::XMLDocument taskDoc;
        tinyxml2::XMLDeclaration *declaration = taskDoc.NewDeclaration();
        taskDoc.InsertFirstChild(declaration);

        tinyxml2::XMLElement *root = taskDoc.NewElement("root");
        taskDoc.InsertEndChild(root);

        tinyxml2::XMLElement *map = root->InsertNewChildElement("map");
        map->InsertNewChildElement("width")->SetText(width);
        map->InsertNewChildElement("height")->SetText(height);
        SetStartFinish(map, grid_matrix);

        tinyxml2::XMLElement *grid = map->InsertNewChildElement("grid");
        SetGrid(grid, grid_matrix);

        tinyxml2::XMLElement *obstacles = map->InsertNewChildElement("dynamicobstacles");
        SetDynamicObstacles(obstacles, grid_matrix);
        
        tinyxml2::XMLElement *options = root->InsertNewChildElement("options");
        options->InsertNewChildElement("hweight")->SetText(1.0);

        // taskDoc.Print();
        std::string filename = std::string(argv[2]) + "/" + std::to_string(test_id) + ".xml";
        std::cout << filename << std::endl;
        taskDoc.SaveFile(filename.data());
        input.close();
    }
}


void SetStartFinish(tinyxml2::XMLElement *map, std::vector<std::vector<bool>>& grid) {
    int start_i, start_j, finish_i, finish_j;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> height_distrib(0, grid.size() - 1);
    std::uniform_int_distribution<> width_distrib(0, grid[0].size() - 1);

    do {
        start_i = height_distrib(gen);
        start_j = width_distrib(gen);
    } while (grid[start_i][start_j] == true);

    do {
        finish_i = height_distrib(gen);
        finish_j = width_distrib(gen);
    } while (grid[finish_i][finish_j] == true || finish_i == start_i && finish_j == start_j);

    map->InsertNewChildElement("startx")->SetText(start_j + 1);
    map->InsertNewChildElement("starty")->SetText(start_i + 1);
    map->InsertNewChildElement("finishx")->SetText(finish_j + 1);
    map->InsertNewChildElement("finishy")->SetText(finish_i + 1);
}


void SetGrid(tinyxml2::XMLElement *grid, std::vector<std::vector<bool>>& grid_matrix) {
    for (const auto& row : grid_matrix) {
        std::stringstream stream("");
        for (int j = 0; j < row.size(); ++j) {
            stream << row[j] << (j + 1 == row.size() ? "" : " ");
        }

        grid->InsertNewChildElement("row")->SetText(stream.str().data());
    }
}

void GeneratePath(std::vector<std::vector<PathComponent>>& obstacles, std::vector<std::vector<bool>>& grid) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> height_distrib(0, grid.size() - 1);
    std::uniform_int_distribution<> width_distrib(0, grid[0].size() - 1);

    std::vector<PathComponent> obstacle;

    PathComponent next;

    do {
        next.x = width_distrib(gen);
        next.y = height_distrib(gen);
    } while (grid[next.y][next.x] == true);

    next.time = 0;
    obstacle.push_back(next);

    int components_number = height_distrib(gen) % 20 + 5;
    for (int i = 0; i < components_number; ++i) {
        PathComponent& current = obstacle.back();

        do {
            if (height_distrib(gen) % 2 == 1) {
                next.x = width_distrib(gen);
                next.y = current.y;
            } else {
                next.x = current.x;
                next.y = height_distrib(gen);
            }

            next.time = current.time + abs(next.x - current.x) + abs(next.y - current.y);
        } while (grid[next.y][next.x] == true || PathsIntersect({current, next}, obstacles) == true);

        obstacle.push_back(next);
    }

    obstacles.push_back(obstacle);
}

void SetDynamicObstacles(tinyxml2::XMLElement *obstacles, std::vector<std::vector<bool>>& grid) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> obstacles_distrib(10, 20);

    int obstacles_number = obstacles_distrib(gen);

    std::vector<std::vector<PathComponent>> obstaclePaths;

    for (int i = 0; i < obstacles_number; ++i) {
        tinyxml2::XMLElement *obstacle = obstacles->InsertNewChildElement("obstacle");

        GeneratePath(obstaclePaths, grid);

        for (const auto& [x, y, time] : obstaclePaths[i]) {
            tinyxml2::XMLElement *point = obstacle->InsertNewChildElement("point");

            point->SetAttribute("x", x);
            point->SetAttribute("y", y);
            point->SetAttribute("time", time);
        }
    }
}