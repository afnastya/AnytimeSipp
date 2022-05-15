#pragma once
#include <vector>
#include <deque>
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

class Anytime : public Search {
public:
    Anytime(int _logLevel = -1);
    SearchResult startSearch(Map& map, const EnvironmentOptions& options) override;
    const SearchResult* getResults() override;
    ~Anytime();

private:
    using NodeInfo = std::tuple<int, int, int, bool>;
    std::deque<SearchResult>                                        sresults;
    std::deque<std::list<Node>>                                     lppaths, hppaths;
    std::unordered_map<NodeInfo, Node*, boost::hash<NodeInfo>>      cells2nodes;
    std::set<Node*, std::function<bool(Node*, Node*)>>              OPEN, INCONS, hweightlessOPEN;
    std::unordered_set<Node*>                                       CLOSE;
    int                                                             logLevel;
    double                                                          hweight, suboptimalityBound;
    Node*                                                           goalNode;
    std::chrono::time_point<std::chrono::system_clock>              startTime;
    size_t                                                          currentResultIndex;

    bool improvePath(Map& map);
    std::vector<std::pair<Node*, int>> getSuccessors(Node* node, Map& map);
    void regenerateOPEN();
    void updateSuboptimalityBound();
    void generatePath(Node* goal, const Map& map);
    void setSearchResult(bool pathFound, const Map& map);
    double getHeuristic(int i, int j, const Map& map);
};