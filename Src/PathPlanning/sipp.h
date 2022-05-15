#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <boost/container_hash/hash.hpp>
#include "search.h"
#include "searchresult.h"
#include "node.h"
#include "map.h"
#include "environmentoptions.h"

class Sipp : public Search {
public:
    Sipp(int _logLevel = -1);
    ~Sipp();
    SearchResult startSearch(Map& map, const EnvironmentOptions& options) override;
    const SearchResult* getResults() override;

private:
    using NodeInfo = std::tuple<int, int, int>;
    SearchResult                                                  sresult;
    std::list<Node>                                               lppath, hppath;
    std::unordered_map<NodeInfo, Node*, boost::hash<NodeInfo>>    cells2nodes;
    std::set<Node*, std::function<bool(Node*, Node*)>>            OPEN;
    std::unordered_set<Node*>                                     CLOSE;
    int                                                           logLevel;
    size_t                                                        currentResultIndex;

    std::vector<std::pair<Node*, int>> getSuccessors(Node* node, Map& map, const EnvironmentOptions &options);
    bool checkDiagonalSuccessor(Node* node, int i, int j, const Map& map, const EnvironmentOptions &options);
    void generatePath(Node* goal, const Map& map);
    void setSearchResult(bool pathFound);
    double getHeuristic(int i, int j, const Map& map);
};