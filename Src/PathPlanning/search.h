#pragma once
#include "map.h"
#include "environmentoptions.h"
#include "searchresult.h"
#include <vector>

class Search {
private:
public:
    virtual SearchResult startSearch(Map& map, const EnvironmentOptions& options) = 0;
    virtual const SearchResult* getResults() = 0;
    virtual ~Search() = default;
};
