#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include "../Src/PathPlanning/tinyxml2.h"
#include "path.h"

struct Task {
    int width, height;
    int obstacles_number;
    std::vector<std::vector<bool>> grid;
    bool mightIntersect = true;
};

void ParseMap(const std::string& input_file, Task& task);
void GenerateTask(Task& task, const std::string& output_file);
void SetStartFinish(tinyxml2::XMLElement *map, Task& task);
void SetGrid(tinyxml2::XMLElement *grid, Task& task);
void SetDynamicObstacles(tinyxml2::XMLElement *obstacles, Task& task);

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Maps directory is not specified" << std::endl;
        return 0;
    }

    std::filesystem::path maps_directory(argv[1]);
    for (const auto& dir_entry : std::filesystem::directory_iterator{maps_directory}) {
        auto map_path = dir_entry.path();
        const std::string map_name = map_path.stem().string();
        std::cout << map_name << "\n";

        Task task;
        ParseMap(map_path, task);

        for (int arg = 3; arg < argc; ++arg) {
            task.obstacles_number = std::stoi(argv[arg]);

            for (int task_id = 0; task_id < 5; ++task_id) {
                std::string output_file = std::string(argv[2])
                                          + "/" + map_name
                                          + "_" + std::to_string(task.obstacles_number)
                                          + "_" + std::to_string(task_id)
                                          + ".xml";
                std::cout << output_file << std::endl;

                GenerateTask(task, output_file);
            }
        }
    }
}

void GenerateTask(Task& task, const std::string& filename) {
    tinyxml2::XMLDocument taskDoc;
    tinyxml2::XMLDeclaration *declaration = taskDoc.NewDeclaration();
    taskDoc.InsertFirstChild(declaration);

    tinyxml2::XMLElement *root = taskDoc.NewElement("root");
    taskDoc.InsertEndChild(root);

    tinyxml2::XMLElement *map = root->InsertNewChildElement("map");
    map->InsertNewChildElement("width")->SetText(task.width);
    map->InsertNewChildElement("height")->SetText(task.height);
    SetStartFinish(map, task);

    tinyxml2::XMLElement *grid = map->InsertNewChildElement("grid");
    SetGrid(grid, task);

    tinyxml2::XMLElement *obstacles = map->InsertNewChildElement("dynamicobstacles");
    SetDynamicObstacles(obstacles, task);
    
    // tinyxml2::XMLElement *options = root->InsertNewChildElement("options");
    // options->InsertNewChildElement("hweight")->SetText(1.0);

    taskDoc.SaveFile(filename.data());
}

void ParseMap(const std::string& input_file, Task& task) {
    std::ifstream input(input_file);

    std::string unused_line;
    std::getline(input, unused_line);
    input >> unused_line >> task.width >> unused_line >> task.height >> unused_line;
    std::getline(input, unused_line);

    task.grid.resize(task.height, std::vector<bool>(task.width));

    char map_element;
    for (int i = 0; i < task.height; ++i) {
        for (int j = 0; j < task.width; ++j) {
            input >> map_element;
            task.grid[i][j] = (map_element != '.');
        }
    }

    input.close();
}

void SetStartFinish(tinyxml2::XMLElement *map, Task& task) {
    std::cout << "setting start & finish..." << std::endl;
    int start_i, start_j, finish_i, finish_j;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> height_distrib(0, task.height - 1);
    std::uniform_int_distribution<> width_distrib(0, task.width - 1);

    do {
        start_i = height_distrib(gen);
        start_j = width_distrib(gen);
    } while (task.grid[start_i][start_j] == true);

    int distance;
    do {
        finish_i = height_distrib(gen);
        finish_j = width_distrib(gen);

        distance = abs(start_i - finish_i) + abs(start_j - finish_j);
    } while (distance < std::max(task.height, task.width)
             || task.grid[finish_i][finish_j] == true);

    map->InsertNewChildElement("startx")->SetText(start_j + 1);
    map->InsertNewChildElement("starty")->SetText(start_i + 1);
    map->InsertNewChildElement("finishx")->SetText(finish_j + 1);
    map->InsertNewChildElement("finishy")->SetText(finish_i + 1);
}


void SetGrid(tinyxml2::XMLElement *grid, Task& task) {
    for (const auto& row : task.grid) {
        std::stringstream stream("");
        for (uint32_t j = 0; j < row.size(); ++j) {
            stream << row[j] << (j + 1 == row.size() ? "" : " ");
        }

        grid->InsertNewChildElement("row")->SetText(stream.str().data());
    }
}

bool GeneratePath(std::vector<std::vector<PathComponent>>& obstacles, Task& task) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> height_distrib(0, task.height - 1);
    std::uniform_int_distribution<> width_distrib(0, task.width - 1);

    std::vector<PathComponent> obstacle;

    PathComponent next;

    next.time = 0;
    do {
        next.x = width_distrib(gen);
        next.y = height_distrib(gen);
    } while (task.grid[next.y][next.x] || (!task.mightIntersect && PathsIntersect({next, next}, obstacles)));

    obstacle.push_back(next);

    while (next.time < task.width + task.height) {
        PathComponent& current = obstacle.back();

        int min_dy = 0, max_dy = 0, min_dx = 0, max_dx = 0;

        while (current.x + min_dx - 1 >= 0 && !task.grid[current.y][current.x + min_dx - 1]) {
            --min_dx;
        }
        while (current.x + max_dx + 1 < task.width && !task.grid[current.y][current.x + max_dx + 1]) {
            ++max_dx;
        }
        while (current.y + min_dy - 1 >= 0 && !task.grid[current.y + min_dy - 1][current.x]) {
            --min_dy;
        }
        while (current.y + max_dy + 1 < task.height && !task.grid[current.y + max_dy + 1][current.x]) {
            ++max_dy;
        }


        auto startTime = std::chrono::system_clock::now();
        double time = 0;

        do {
            if (height_distrib(gen) % 2 == 1) {
                next.x = width_distrib(gen) % (max_dx - min_dx + 1) + min_dx + current.x;
                next.y = current.y;
            } else {
                next.x = current.x;
                next.y = height_distrib(gen) % (max_dy - min_dy + 1) + min_dy + current.y;
            }

            if (next.x == current.x && next.y == current.y) {
                next.time = current.time + width_distrib(gen) % 10;
            } else {
                next.time = current.time + abs(next.x - current.x) + abs(next.y - current.y);
            }

            auto endTime = std::chrono::system_clock::now();
            time = std::chrono::duration<double>(endTime - startTime).count();
        } while (time < 3 && (task.grid[next.y][next.x]
                              || (!task.mightIntersect && PathsIntersect({current, next}, obstacles))));

        if (time >= 3) {
            return false;
        }

        obstacle.push_back(next);
    }

    obstacles.push_back(obstacle);
    return true;
}

void SetDynamicObstacles(tinyxml2::XMLElement *obstacles, Task& task) {
    std::cout << "setting dynamic obstacles... " << std::endl;
    std::vector<std::vector<PathComponent>> obstaclePaths;

    for (int i = 0; i < task.obstacles_number; ++i) {
        tinyxml2::XMLElement *obstacle = obstacles->InsertNewChildElement("obstacle");
        obstacle->SetAttribute("id", i);

        if (i % 10 == 0) {
            std::cout << i << " ";
            std::cout.flush();
        } 

        while (!GeneratePath(obstaclePaths, task)) {
        }

        for (const auto& [x, y, time] : obstaclePaths[i]) {
            tinyxml2::XMLElement *point = obstacle->InsertNewChildElement("point");

            point->SetAttribute("x", x);
            point->SetAttribute("y", y);
            point->SetAttribute("time", time);
        }
    }
    std::cout << std::endl;
}