#include "map.h"
#include <algorithm>
#include <sstream>
#include "vector.h"

#define INF 1'000'000'000

Map::Map() {
}

bool Map::getMap(tinyxml2::XMLElement* map_tag) {
    tinyxml2::XMLElement *element = map_tag->FirstChildElement();

    if (!element) {
        std::cout << "ERROR: nothing in map tag\n";
        return false;
    }

    std::vector<int*> data_ptrs = {&width, &height, &start_j, &start_i, &goal_j, &goal_i};
    for (auto data_ptr : data_ptrs) {
        if (element->QueryIntText(data_ptr) != tinyxml2::XMLError::XML_SUCCESS || *data_ptr < 0) {
            return false;
        }

        element = element->NextSiblingElement();
        if (element == nullptr) {
            std::cout << "ERROR: less children of 'map' tag than expected, some map info is missing\n";
            return false;
        }
    }

    for (int i = 2; i < data_ptrs.size(); i++) {
        *data_ptrs[i] -= 1;
    }

    map.resize(height, std::vector<std::vector<Interval>>(width));

    // Parsing "grid" tag
    tinyxml2::XMLElement *row = element->FirstChildElement();
    for (int i = 0; i < height; ++i) {
        if (row == nullptr) {
            std::cout << "ERROR: less rows in grid than expected\n";
            return false;
        }

        std::stringstream stream(row->FirstChild()->Value());
        for (int j = 0; j < width; ++j) {
            bool isObstacle;
            if (!(stream >> isObstacle)) {
                std::cout << "ERROR: less columns in row #" << i + 1 << " than expected\n";
                return false;
            }

            if (isObstacle) {
                map[i][j].emplace_back(false, 0, INF);
            }
        }
        row = row->NextSiblingElement();
    }

    // Parsing "dynamicobstacles" tag
    element = element->NextSiblingElement();
    for (auto obstacle = element->FirstChildElement(); obstacle != nullptr; obstacle = obstacle->NextSiblingElement()) {
        dynamicObstacles.emplace_back();
        for (auto point = obstacle->FirstChildElement(); point != nullptr; point = point->NextSiblingElement()) {
            int x, y;
            double time;

            point->QueryIntAttribute("x", &x);
            point->QueryIntAttribute("y", &y);
            point->QueryDoubleAttribute("time", &time);

            dynamicObstacles.back().path.emplace_back(x - 1, y - 1, time * cost);
        }
    }

    return true;
}

int Map::getCost() const {
    return cost;
}

bool Map::CellIsTraversable(int i, int j) const {
    return map[i][j].empty() || map[i][j][0].isSafe != false;
}

int Map::getWidth() const {
    return width;
}

int Map::getHeight() const {
    return height;
}

std::pair<int, int> Map::getStartCell() const {
    return std::pair<int, int>(start_i, start_j);
}

std::pair<int, int> Map::getGoalCell() const {
    return std::pair<int, int>(goal_i, goal_j);
}

const Interval& Map::operator()(int i, int j, int interval) const {
    return map[i][j][interval];
}

const std::vector<Interval>& Map::operator()(int i, int j) const {
    return map[i][j];
}

void Map::setIntervals(int i, int j) {
    if (!map[i][j].empty()) {
        return;
    }

    std::vector<Interval> collision_intervals;

    for (const auto& obstacle : dynamicObstacles) {
        auto& path = obstacle.path;
        for (int point_i = 1; point_i < path.size(); ++point_i) {
            Vector<int> Cell(j, i);
            Vector<int> P1(path[point_i - 1].x, path[point_i - 1].y);
            Vector<int> P2(path[point_i].x, path[point_i].y);
            if (!PointOnSegment(Cell, P1, P2)) {
                continue;
            }

            collision_intervals.emplace_back(
                false,
                path[point_i - 1].time + getDistance(i, j, path[point_i - 1].y, path[point_i - 1].x) - cost / 2,
                path[point_i - 1].time + getDistance(i, j, path[point_i - 1].y, path[point_i - 1].x) + cost / 2
            );
        }
    }

    std::sort(
        collision_intervals.begin(),
        collision_intervals.end(),
        [](const Interval& i1, const Interval& i2) { return i1.startTime < i2.startTime; }
    );

    if (collision_intervals.empty()) {
        map[i][j].emplace_back(true, 0, INF);
        return;
    }

    for (const Interval& interval : collision_intervals) {
        if (map[i][j].empty() || interval.startTime > map[i][j].back().endTime) {
            map[i][j].emplace_back(interval);
        }

        map[i][j].back().endTime = std::max(map[i][j].back().endTime, interval.endTime);
    }

    if (map[i][j][0].startTime > 0) {
        map[i][j].emplace_back(true, map[i][j].back().endTime, INF);
        for (int interval = map[i][j].size() - 2; interval >= 0; --interval) {
            map[i][j][interval].isSafe = true;
            map[i][j][interval].endTime = map[i][j][interval].startTime;
            map[i][j][interval].startTime = interval == 0 ? 0 : map[i][j][interval - 1].endTime;
        }
    } else {
        for (int interval = 0; interval < map[i][j].size(); ++interval) {
            map[i][j][interval].isSafe = true;
            map[i][j][interval].startTime = map[i][j][interval].endTime;

            if (interval + 1 == map[i][j].size()) {
                map[i][j][interval].endTime = INF;
                continue;
            }

            map[i][j][interval].endTime = map[i][j][interval + 1].startTime;
        }
    }
}

int Map::getDistance(int i1, int j1, int i2, int j2) const {
    return (abs(i2 - i1) + abs(j2 - j1)) * cost;
}

int Map::getSafeIntervalId(int i, int j, int time) const {
    static auto Compare = [] (const int time, const Interval& interval) {
        return time < interval.endTime;
    };

    return std::upper_bound(map[i][j].begin(), map[i][j].end(), time, Compare) - map[i][j].begin();
}

int Map::getIntervalStart(int i, int j, int interval) const {
    return map[i][j][interval].startTime;
}

int Map::getIntervalEnd(int i, int j, int interval) const {
    return map[i][j][interval].endTime;
}

void Map::printIntervals(int i, int j) const {
    std::cout << "i = " << i << ", j = " << j << "\nIntervals:";
    for (auto& interval : map[i][j]) {
        std::cout << "{" << interval.isSafe << ", " << interval.startTime << ", " << interval.endTime << "} ";
    }
    std::cout << "\n";
}
