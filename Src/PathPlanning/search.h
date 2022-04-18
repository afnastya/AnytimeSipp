#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "searchresult.h"
#include "node.h"
#include "map.h"
#include "environmentoptions.h"

// class Search {
// private:
// public:
//     Search();
//     virtual SearchResult startSearch(const Map& map, const EnvironmentOptions& options) = 0;
//     virtual ~Search();
// };


class Sipp {
public:
    Sipp(int _logLevel = -1);
    ~Sipp();
    SearchResult startSearch(Map& map, const EnvironmentOptions& options);

private:
    SearchResult                                            sresult;
    std::list<Node>                                         lppath, hppath;
    std::unordered_map<long long, Node*>                    cells2nodes;
    std::set<Node*, std::function<bool(Node*, Node*)>>      OPEN;
    std::unordered_set<Node*>                               CLOSE;
    int                                                     logLevel;

    std::vector<std::pair<Node*, int>> getSuccessors(Node* node, Map& map, const EnvironmentOptions &options);
    bool checkDiagonalSuccessor(Node* node, int i, int j, const Map& map, const EnvironmentOptions &options);
    void generatePath(Node* goal, const Map& map);
    void setSearchResult(bool pathFound);
    double getHeuristic(int i, int j, const Map& map);
};