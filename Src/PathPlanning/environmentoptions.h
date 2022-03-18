#pragma once
#include "tinyxml2.h"

class EnvironmentOptions {
public:
    int         metrictype;
    double      hweight;
    bool        allowsqueeze;
    bool        allowdiagonal;
    bool        cutcorners;
    // int         searchtype;

    bool getOptions(tinyxml2::XMLElement *options);
};