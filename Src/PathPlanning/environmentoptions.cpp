#include "environmentoptions.h"
#include <iostream>

bool EnvironmentOptions::getOptions(tinyxml2::XMLElement *options) {
    if (!options) {
        return false;
    }

    options->FirstChildElement("hweight")->QueryDoubleText(&hweight);
    // options->QueryBoolAttribute("allowdiagonal", &allowdiagonal);
    // options->QueryBoolAttribute("cutcorners", &cutcorners);
    // options->QueryBoolAttribute("allowsqueeze", &allowsqueeze);

    allowdiagonal = false;
    cutcorners = false;
    allowsqueeze = false;

    return true;
}