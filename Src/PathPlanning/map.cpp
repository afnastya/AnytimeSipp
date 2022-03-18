#include "map.h"
#include <sstream>

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

            dynamicObstacles.back().path.emplace_back(x - 1, y - 1, time);
        }
    }

    return true;
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

int vectorProduct(int x1, int y1, int x2, int y2) {
    return x1 * y2 - x2 * y1;
}

int scalarProduct(int x1, int y1, int x2, int y2) {
    return x1 * x2 + y1 * y2;
}

bool CellBelongsToPath(int i, int j, const PathComponent& p1, const PathComponent& p2) {
    return vectorProduct(j - p1.x, i - p1.y, p2.x - p1.x, p2.y - p1.y) == 0
           && scalarProduct(j - p1.x, i - p1.y, p2.x - p1.x, p2.y - p1.y) >= 0
           && scalarProduct(j - p2.x, i - p2.y, p1.x - p2.x, p1.y - p2.y) >= 0;
}

void Map::setIntervals(int i, int j) {
    if (!map[i][j].empty()) {
        return;
    }

    for (const auto& obstacle : dynamicObstacles) {
        auto& path = obstacle.path;
        for (int point_i = 1; point_i < path.size(); ++point_i) {
            if (!CellBelongsToPath(i, j, path[point_i - 1], path[point_i])) {
                continue;
            }

            map[i][j].emplace_back(
                false,
                path[point_i - 1].time + abs(j - path[point_i - 1].x) + abs(i - path[point_i - 1].y) - 0.5,
                path[point_i - 1].time + abs(j - path[point_i - 1].x) + abs(i - path[point_i - 1].y) + 0.5
            );
        }
    }

    std::sort(
        map[i][j].begin(),
        map[i][j].end(),
        [](const Interval& i1, const Interval& i2) { return i1.startTime < i2.startTime; }
    );

    if (map[i][j].empty()) {
        map[i][j].emplace_back(true, 0, INF);
        return;
    }

    // for (int interval = 0; interval < map[i][j].size(); ++interval) {
        // if (map[i][j][interval].isSafe == false) {
        //     double safeStart = map[i][j][interval].endTime;
        //     double safeEnd = INF;
        //     if (interval + 1 < map[i][j].size()) {
        //         safeEnd = map[i][j][interval + 1].startTime;
        //     }

        //     map[i][j].insert(map[i][j].begin() + interval + 1, {true, safeStart, safeEnd});
        // }
    // }

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

    // if (map[i][j][0].startTime > 0) {
    //     double safeEnd = INF;
    //     if (1 < map[i][j].size()) {
    //         safeEnd = map[i][j][0].startTime;
    //     }

    //     map[i][j].insert(map[i][j].begin(), {true, 0, safeEnd});
    // }
}

int Map::getSafeIntervalId(int i, int j, double time) const {
    for (int interval = 0; interval < map[i][j].size(); ++interval) {
        if (map[i][j][interval].endTime > time) {
            return interval;
        }
    }
}

double Map::getIntervalStart(int i, int j, int interval) const {
    return map[i][j][interval].startTime;
}

double Map::getIntervalEnd(int i, int j, int interval) const {
    return map[i][j][interval].endTime;
}

void Map::printIntervals(int i, int j) {
    std::cout << "i = " << i << ", j = " << j << "\nIntervals:";
    for (auto& interval : map[i][j]) {
        std::cout << "{" << interval.isSafe << ", " << interval.startTime << ", " << interval.endTime << "}\n";
    }
}

// int Map::getEarliestSafeIntervalId(int i, int j, double time) {
//     for (int interval = 0; interval < map[i][j].size(); ++i) {
//         if (map[i][j][interval].endTime > time) {
//             if (map[i][j][interval].startTime > time) {
//                 --interval;
//             }

//             return (interval + 1) * 2 - (map[i][j][0] == 0);
//         }
//     }

//     return map[i][j].size();
// }

// double Map::getSafeIntervalStart(int i, int j, int interval) {
//     return 0;
// }

// double Map::getSafeIntervalEnd(int i, int j, int interval) {
//     interval /= 2;
//     if (interval >= map[i][j].size()) {
//         return 0;   // invalid argument
//     }

//     if (map[i][j][0].startTime == 0) {
//         if (interval + 1 >= map[i][j].size()) {
//             return 0; // invalid argument
//         }

//         return map[i][j][interval + 1].startTime;
//     }

//     return map[i][j][interval].startTime;
// }