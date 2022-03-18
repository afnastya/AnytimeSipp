#include "search.h"
#include <cmath>

auto NodeCompare = [](const Node* lhs, const Node* rhs) {
    return std::tie(lhs->F, lhs->g, lhs->interval, lhs->i, lhs->j) < std::tie(rhs->F, rhs->g, rhs->interval, rhs->i, rhs->j);
};


Sipp::Sipp() : OPEN(NodeCompare) {
}

Sipp::~Sipp() {
    for (auto it = cells2nodes.begin(); it != cells2nodes.end(); ++it) {
        delete it->second;
    }
}

long long hash(int i, int j, int k) {
    return ((i * 1000) + j) * 1000 + k;
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
        getHeuristic(start.first, start.second, map, options),
        options.hweight,
        nullptr
    );

    OPEN.insert(cells2nodes[hash(start.first, start.second, 0)]);

    while (!OPEN.empty()) {
        ++sresult.numberofsteps;

        Node* now = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        CLOSE.insert(now);

        // std::cout << now->i << " " << now->j << " " << now->interval <<"\n";

        if (now->i == goal.first && now->j == goal.second) {
            generatePath(now);
            setSearchResult(true);
            auto endTime = std::chrono::system_clock::now();
            sresult.searchtime = std::chrono::duration<double>(endTime - startTime).count();
            return sresult;
        }

        for (auto [successor, cost] : getSuccessors(now, map, options)) {
            if (CLOSE.contains(successor)) {
                continue;
            }

            double new_g = std::max(
                now->g + cost,
                map(successor->i, successor->j, successor->interval).startTime + cost / 2
            );
            
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


std::vector<std::pair<Node*, double>> Sipp::getSuccessors(Node* node, Map& map, const EnvironmentOptions& options) {
    std::vector<std::pair<Node*, double>> successors;

    for (int i = std::max(0, node->i - 1); i < std::min(node->i + 2, map.getHeight()); ++i) {
        for (int j = std::max(0, node->j - 1); j < std::min(node->j + 2, map.getWidth()); ++j) {
            if (map.CellIsTraversable(i, j)) {
                double cost = 1.0;
                if ((i + j) % 2 == (node->i + node->j) % 2) {
                    if (!checkDiagonalSuccessor(node, i, j, map, options)) {
                        continue;
                    }
                    cost = sqrt(2);
                }

                double minTime = node->g + cost;
                double maxTime = map(node->i, node->j, node->interval).endTime;
                if (minTime >= maxTime) {
                    continue;
                }


                map.setIntervals(i, j);

                int interval = map.getSafeIntervalId(i, j, minTime);
                if (interval >= map(i, j).size() || map(i, j, interval).startTime >= maxTime) {
                    continue;
                }

                for (; interval < map(i, j).size() && map(i, j, interval).startTime < maxTime; ++interval) {
                    Node* successor = cells2nodes[hash(i, j, interval)];
                    if (successor == nullptr) {
                        successor = new Node(
                            i,
                            j,
                            interval,
                            std::max(node->g + cost, map(i, j, interval).startTime + cost / 2),
                            getHeuristic(i, j, map, options),
                            options.hweight,
                            node
                        );
                        cells2nodes[hash(i, j, interval)] = successor;
                        OPEN.insert(successor);
                    }
                    successors.push_back({successor, cost});
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

double distance(Node* start, Node* finish) {
    return sqrt(pow(finish->i - start->i, 2) + pow(finish->j - start->j, 2));
}

void Sipp::generatePath(Node* goal) {
    Node* now = goal;
    while (now != nullptr) {
        lppath.push_front(*now);
        now = now->parent;
    }

    if (lppath.size() < 3) {
        hppath = lppath;
        return;
    }

    auto it1 = lppath.begin(), it3 = it1++, it2 = it1++;
    hppath.push_back(*it3);
    double prevDistance = distance(&(*it3), &(*it2));
    while (it1 != lppath.end()) {
        if ((it2->j - it3->j) * (it1->i - it3->i) - (it1->j - it3->j) * (it2->i - it3->i) != 0
                || distance(&(*it3), &(*it1)) < prevDistance) {
            it2 = it1;
            it3 = --it1;
            ++it1;

            if (it3->g > hppath.back().g + 1) { // 1 = cost
                hppath.push_back(hppath.back());
            }

            hppath.push_back(*it3);
        }

        prevDistance = distance(&(*it3), &(*it1));
        ++it1;
    }

    hppath.push_back(lppath.back());
}

void Sipp::setSearchResult(bool pathFound) {
    sresult.pathfound = pathFound;
    sresult.pathlength = pathFound ? lppath.back().g : 0;
    sresult.lppath = &lppath;
    sresult.hppath = &hppath;
    sresult.nodescreated = OPEN.size() + CLOSE.size();
}

double Sipp::getHeuristic(int i, int j, const Map& map, const EnvironmentOptions& options) {
    auto [goal_i, goal_j] = map.getGoalCell();
    int di = abs(i - goal_i), dj = abs(j - goal_j);
    return di + dj; // manhattan

    // switch (options.metrictype) {
    //     case 0:
    //         return sqrt(2) * std::min(di, dj) + abs(di - dj);
    //     case 1:
    //         return di + dj;
    //     case 2:
    //         return sqrt(pow(di, 2) + pow(dj, 2));
    //     case 3:
    //         return std::max(di, dj);
    //     default:
    //         return 0;
    // }
}