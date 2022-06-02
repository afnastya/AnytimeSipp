#include "environmentoptions.h"
#include <iostream>

EnvironmentOptions::EnvironmentOptions() {
    anytime = false;
    hweight = 1.0;
    allowdiagonal = false;
    cutcorners = false;
    allowsqueeze = false;
}

void EnvironmentOptions::getOptions(tinyxml2::XMLElement *options) {
    if (!options) {
        return;
    }

    auto hweight_tag = options->FirstChildElement("hweight");
    if (hweight_tag) {
        hweight_tag->QueryDoubleText(&hweight);
    }

    if (hweight < 1.0) {
        throw std::invalid_argument("ERROR: invalid value of initial heuristic weight, should be > 1");
    }
    // options->QueryBoolAttribute("allowdiagonal", &allowdiagonal);
    // options->QueryBoolAttribute("cutcorners", &cutcorners);
    // options->QueryBoolAttribute("allowsqueeze", &allowsqueeze);
}