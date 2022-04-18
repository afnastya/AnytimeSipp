#pragma once
#include <vector>

struct PathComponent {
    int x;
    int y;
    int time;

    PathComponent() {}
    PathComponent(int _x, int _y, int _time) : x(_x), y(_y), time(_time) {}

    friend std::ostream& operator<<(std::ostream& out, const PathComponent& p) {
        out << "(" << p.x << ", " << p.y << ", " << p.time << ")";
        return out;
    }
};

struct Obstacle {
    std::vector<PathComponent>      path;
};