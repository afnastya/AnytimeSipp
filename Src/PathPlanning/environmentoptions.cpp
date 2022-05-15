#include "environmentoptions.h"
#include <iostream>

EnvironmentOptions::EnvironmentOptions() {
    anytime = false;
    hweight = 1.0;
    allowdiagonal = false;
    cutcorners = false;
    allowsqueeze = false;
}

bool EnvironmentOptions::getOptions(tinyxml2::XMLElement *options) {
    if (!options) {
        return true;
    }

    auto hweight_tag = options->FirstChildElement("hweight");
    if (hweight_tag) {
        hweight_tag->QueryDoubleText(&hweight);
    }
    // options->QueryBoolAttribute("allowdiagonal", &allowdiagonal);
    // options->QueryBoolAttribute("cutcorners", &cutcorners);
    // options->QueryBoolAttribute("allowsqueeze", &allowsqueeze);

    return true;
}