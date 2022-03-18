#pragma once
#include <vector>

struct PathComponent {
    int x;
    int y;
    double time;

    PathComponent() {}
    PathComponent(int _x, int _y, double _time) : x(_x), y(_y), time(_time) {}
};

struct Obstacle {
    std::vector<PathComponent>      path;
};