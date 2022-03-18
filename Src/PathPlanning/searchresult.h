#pragma once
#include <list>
#include "node.h"

struct SearchResult {
    bool                    pathfound;
    const std::list<Node>*  lppath;
    const std::list<Node>*  hppath;
    float                   pathlength;
    unsigned int            nodescreated;
    unsigned int            numberofsteps;
    double                  searchtime;

    SearchResult() {
        pathfound = false;
        lppath = nullptr;
        hppath = nullptr;
        pathlength = 0;
        nodescreated = 0;
        numberofsteps = 0;
        searchtime = 0;
    }
};