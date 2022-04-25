#include "search.h"
#include <cmath>

auto NodeCompare = [](const Node* lhs, const Node* rhs) {
    return std::tie(lhs->F, lhs->g, lhs->interval, lhs->i, lhs->j) < std::tie(rhs->F, rhs->g, rhs->interval, rhs->i, rhs->j);
};


Sipp::Sipp(int _logLevel) : OPEN(NodeCompare), logLevel(_logLevel) {
}

Sipp::~Sipp() {
    for (auto it = cells2nodes.begin(); it != cells2nodes.end(); ++it) {
        delete it->second;
    }
}

long long hash(int i, int j, int k) {
    return ((i * 10000) + j) * 10000 + k;
}

SearchResult Sipp::startSearch(Map& map, const EnvironmentOptions& options) {
    auto startTime = std::chrono::system_clock::now();

    std::pair<int, int> start = map.getStartCell(), goal = map.getGoalCell();
    map.setIntervals(start.first, start.second);
    // map.printIntervals(start.first, start.second);

    cells2nodes[hash(start.first, start.second, 0)] = new Node(
        start.first,
        start.second,
        0,
        0,
        getHeuristic(start.first, start.second, map),
        options.hweight,
        nullptr
    );

    if (map(start.first, start.second, 0).startTime == 0) {
        OPEN.insert(cells2nodes[hash(start.first, start.second, 0)]);
    }

    while (!OPEN.empty()) {
        ++sresult.numberofsteps;

        Node* now = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        CLOSE.insert(now);

        if (logLevel > 2) {
            std::cout << "i = " << now->i << " j = " << now->j << " interval = " << now->interval 
                      << " g = " << now->g << std::endl;
        }

        if (now->i == goal.first && now->j == goal.second) {
            generatePath(now, map);
            setSearchResult(true);
            auto endTime = std::chrono::system_clock::now();
            sresult.searchtime = std::chrono::duration<double>(endTime - startTime).count();
            return sresult;
        }

        for (auto [successor, new_g] : getSuccessors(now, map, options)) {
            if (CLOSE.contains(successor)) {
                continue;
            }
            
            if (successor->g > new_g) {
                OPEN.erase(successor);

                successor->g = new_g;
                // successor->updateTime();
                successor->updateF();
                successor->parent = now;

                OPEN.insert(successor);
            }
        }
    }

    setSearchResult(false);
    auto endTime = std::chrono::system_clock::now();
    sresult.searchtime = std::chrono::duration<double>(endTime - startTime).count();
    return sresult;
}


std::vector<std::pair<Node*, int>> Sipp::getSuccessors(Node* node, Map& map, const EnvironmentOptions& options) {
    std::vector<std::pair<Node*, int>> successors;

    for (int i = std::max(0, node->i - 1); i < std::min(node->i + 2, map.getHeight()); ++i) {
        for (int j = std::max(0, node->j - 1); j < std::min(node->j + 2, map.getWidth()); ++j) {
            if (map.CellIsTraversable(i, j)) {
                int cost = map.getCost();
                if ((i + j) % 2 == (node->i + node->j) % 2) {
                    if (!checkDiagonalSuccessor(node, i, j, map, options)) {
                        continue;
                    }
                    cost = sqrt(2);
                }

                int minTime = node->g + cost / 2;        // min time when (node->i, node->j) will be free
                int maxTime = map(node->i, node->j, node->interval).endTime; // max time when (node->i, node->j) will be free

                if (logLevel > 2) {
                    std::cout << "minTime: " << minTime << " maxTime: " << maxTime << std::endl;
                }

                if (minTime >= maxTime) {
                    continue;
                }

                map.setIntervals(i, j);

                if (logLevel > 2) {
                    map.printIntervals(i, j);
                }

                uint32_t interval = map.getSafeIntervalId(i, j, minTime);

                for (; interval < map(i, j).size() && map(i, j, interval).startTime < maxTime; ++interval) {
                    Node* successor = cells2nodes[hash(i, j, interval)];
                    int new_g = std::max(
                        node->g + cost,
                        map(i, j, interval).startTime + cost / 2
                    );

                    if (successor == nullptr) {
                        successor = new Node(
                            i,
                            j,
                            interval,
                            new_g,
                            getHeuristic(i, j, map),
                            options.hweight,
                            node
                        );
                        cells2nodes[hash(i, j, interval)] = successor;
                        OPEN.insert(successor);
                    }
                    successors.push_back({successor, new_g});
                }
            }
        }
    }
    return successors;
}

bool Sipp::checkDiagonalSuccessor(Node* node, int i, int j, const Map& map, const EnvironmentOptions& options) {
    if (!options.allowdiagonal || (node->i == i && node->j == j)) {
        return false;
    }

    bool nearCell1 = map.CellIsTraversable(node->i, j), nearCell2 = map.CellIsTraversable(i, node->j);
    return (nearCell1 && nearCell2) ||
           (options.cutcorners && (nearCell1 || nearCell2)) ||
           (options.allowsqueeze && !nearCell1 && !nearCell2);
}

uint32_t distance(Node* start, Node* finish) {
    return abs(finish->i - start->i) + abs(finish->j - start->j);
}

void Sipp::generatePath(Node* goal, const Map& map) {
    if (logLevel > 1) {
        std::cout << "Generating the path\n";
    }

    Node* now = goal;
    while (now != nullptr) {
        lppath.push_front(*now);
        lppath.front().g /= map.getCost();
        now = now->parent;
    }

    if (lppath.size() < 3) {
        hppath = lppath;
        return;
    }

    auto it1 = lppath.begin(), it3 = it1++, it2 = it1++;
    hppath.push_back(*it3);
    uint32_t startG = it3->g; 
    while (it1 != lppath.end()) {
        if ((it2->j - it3->j) * (it1->i - it3->i) - (it1->j - it3->j) * (it2->i - it3->i) != 0
                || distance(&(*it3), &(*it1)) < it1->g - startG) {
            bool shouldWait = distance(&(*it3), &(*it1)) < it1->g - startG;
            it2 = it1;
            it3 = --it1;
            ++it1;

            hppath.push_back(*it3);
            if (shouldWait) {
                hppath.push_back(*it3);
                hppath.back().g = it2->g - 1;   // 1 = cost
            }

            startG = hppath.back().g;
        }

        ++it1;
    }

    // int dist = distance(&hppath.back(), &lppath.back());
    // if (lppath.back().g > hppath.back().g + dist) {
    //     hppath.push_back(hppath.back());
    //     hppath.back().g = lppath.back().g - dist;
    // }
    hppath.push_back(lppath.back());
}

void Sipp::setSearchResult(bool pathFound) {
    sresult.pathfound = pathFound;
    sresult.pathlength = pathFound ? lppath.back().g : 0;
    sresult.lppath = &lppath;
    sresult.hppath = &hppath;
    sresult.nodescreated = OPEN.size() + CLOSE.size();
}

double Sipp::getHeuristic(int i, int j, const Map& map) {
    auto [goal_i, goal_j] = map.getGoalCell();
    return map.getDistance(i, j, goal_i, goal_j);
}