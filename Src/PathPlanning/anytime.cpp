#include "anytime.h"
#include <cmath>

Anytime::Anytime(int _logLevel) : hweightlessOPEN(CreateNodeCompare()) {
    logLevel = _logLevel;
    goalNode = nullptr;
    currentResultIndex = 0;
}

Anytime::~Anytime() {
    for (auto it = cells2nodes.begin(); it != cells2nodes.end(); ++it) {
        delete it->second;
    }
}

// long long hash(int i, int j, int k, bool l = true) {
//     return (((i * 10000) + j) * 10000 + k) * 10 + l;
// }

SearchResult Anytime::startSearch(Map& map, const EnvironmentOptions& options) {
    startTime = std::chrono::system_clock::now();

    std::pair<int, int> start = map.getStartCell();
    map.setIntervals(start.first, start.second);

    cells2nodes[{start.first, start.second, 0, 1}] = new Node(
        start.first, start.second,
        0, 0,
        getHeuristic(start.first, start.second, map),
        nullptr
    );

    if (map(start.first, start.second, 0).startTime != 0) {
        setSearchResult(false, map);
        return sresults.back();
    }

    hweight = options.hweight;
    OPEN = std::set<Node*, std::function<bool(Node*, Node*)>>(CreateNodeCompare(hweight));
    INCONS = std::set<Node*, std::function<bool(Node*, Node*)>>(CreateNodeCompare());

    OPEN.insert(cells2nodes[{start.first, start.second, 0, 1}]);
    hweightlessOPEN.insert(cells2nodes[{start.first, start.second, 0, 1}]);
    sresults.emplace_back();
    bool pathFound = improvePath(map);
    setSearchResult(pathFound, map);

    while (pathFound && suboptimalityBound > 1) {
        hweight = std::max(1.0, hweight * 0.8);
        regenerateOPEN();
        CLOSE.clear();
        sresults.emplace_back();

        pathFound = improvePath(map);
        setSearchResult(pathFound, map);
    }

    SearchResult sresult = sresults.back();
    sresult.numberofsearches = sresults.size();
    sresult.numberofsteps = 0;
    for (const auto& result : sresults) {
        sresult.numberofsteps += result.numberofsteps;
    }
    return sresult;
}

const SearchResult* Anytime::getResults() {
    if (currentResultIndex < sresults.size()) {
        return &sresults[currentResultIndex++];
    }

    return nullptr;
}

void Anytime::regenerateOPEN() {
    std::set<Node*, std::function<bool(Node*, Node*)>> tmp(OPEN.key_comp());
    OPEN.swap(tmp);
    OPEN = std::set<Node*, std::function<bool(Node*, Node*)>>(CreateNodeCompare(hweight));

    for (auto node : tmp) {
        OPEN.insert(node);
    }

    for (auto node : INCONS) {
        OPEN.insert(node);
        hweightlessOPEN.insert(node);
    }
}

void Anytime::updateSuboptimalityBound() {
    if (logLevel > 2) {
        std::cout << "Update Suboptimality: ";
    }

    auto minF = (*hweightlessOPEN.begin())->F;
    if (!INCONS.empty()) {
        minF = std::min(minF, (*INCONS.begin())->F);
    }

    suboptimalityBound = std::min(hweight, (double) goalNode->g / minF);

    if (logLevel > 2) {
        std::cout << suboptimalityBound << std::endl;
    }
}

bool Anytime::improvePath(Map& map) {
    std::pair<int, int> goal = map.getGoalCell();

    while ((!goalNode || goalNode->getWeightedF(hweight) > (*OPEN.begin())->getWeightedF(hweight)) && !OPEN.empty()) {
        ++sresults.back().numberofsteps;

        if (logLevel > 2) {
            std::cout << "step: " << sresults.back().numberofsteps << std::endl;
            std::cout << "OPEN: " << OPEN.size() << " hweightlessOPEN: " << hweightlessOPEN.size()
                      << " CLOSE: " << CLOSE.size() << " INCONS: " << INCONS.size() << std::endl;
        }

        Node* now = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        hweightlessOPEN.erase(now);
        CLOSE.insert(now);

        if (logLevel > 2) {
            std::cout << "now: i = " << now->i << " j = " << now->j << " interval = " << now->interval 
                      << " g = " << now->g << std::endl;
        }

        for (auto [successor, new_g] : getSuccessors(now, map)) {
            if (successor->i == goal.first && successor->j == goal.second
                && (!goalNode || std::tie(goalNode->g, successor->opt) > std::tie(new_g, goalNode->opt))) {
                    goalNode = successor;
            }

            if (successor->g > new_g) {
                if (CLOSE.contains(successor)) {
                    successor->g = new_g;
                    successor->updateF();
                    successor->parent = now;
                    INCONS.insert(successor);
                    continue;
                }

                OPEN.erase(successor);
                hweightlessOPEN.erase(successor);

                successor->g = new_g;
                successor->updateF();
                successor->parent = now;

                OPEN.insert(successor);
                hweightlessOPEN.insert(successor);
            }
        }
    }

    if (logLevel > 2) {
        std::cout << "END IMPROVE PATH" << std::endl;
    }

    return !OPEN.empty();
}

std::vector<std::pair<Node*, int>> Anytime::getSuccessors(Node* node, Map& map) {
    std::vector<std::pair<Node*, int>> successors;

    for (int i = std::max(0, node->i - 1); i < std::min(node->i + 2, map.getHeight()); ++i) {
        for (int j = std::max(0, node->j - 1); j < std::min(node->j + 2, map.getWidth()); ++j) {
            if (map.CellIsTraversable(i, j)) {
                if ((i + j) % 2 == (node->i + node->j) % 2) {
                    continue;
                }

                int cost = map.getCost();

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
                    for (int opt = 0; opt <= node->opt; ++opt) {
                        Node* successor = cells2nodes[std::tie(i, j, interval, opt)];
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
                                node,
                                opt
                            );
                            cells2nodes[std::tie(i, j, interval, opt)] = successor;
                            OPEN.insert(successor);
                            hweightlessOPEN.insert(successor);
                        }
                        successors.push_back({successor, new_g});
                    }
                }
            }
        }
    }
    return successors;
}

static uint32_t distance(Node* start, Node* finish) {
    return abs(finish->i - start->i) + abs(finish->j - start->j);
}

void Anytime::generatePath(Node* goal, const Map& map) {
    lppaths.emplace_back();
    hppaths.emplace_back();
    auto& lppath = lppaths.back();
    auto& hppath = hppaths.back();

    if (goal == nullptr) {
        return;
    }

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
    if (it2->g - it3->g > 1) { // 1 = cost
        hppath.push_back(*it3);
        hppath.back().g = it2->g - 1;
    }
    uint32_t startG = hppath.back().g; 
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

    hppath.push_back(lppath.back());
}

void Anytime::setSearchResult(bool pathFound, const Map& map) {
    auto& sresult = sresults.back();

    sresult.pathfound = pathFound;
    generatePath(goalNode, map);
    sresult.pathlength = pathFound ? lppaths.back().back().g : 0;
    sresult.lppath = &lppaths.back();
    sresult.hppath = &hppaths.back();

    if (pathFound) {
        updateSuboptimalityBound();
        sresult.suboptimalityBound = suboptimalityBound;
    }

    sresult.nodescreated = cells2nodes.size();
    auto endTime = std::chrono::system_clock::now();
    sresult.searchtime = std::chrono::duration<double>(endTime - startTime).count();
}

double Anytime::getHeuristic(int i, int j, const Map& map) {
    auto [goal_i, goal_j] = map.getGoalCell();
    return map.getDistance(i, j, goal_i, goal_j);
}