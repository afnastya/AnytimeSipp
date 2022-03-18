#include "map.h"
#include "environmentoptions.h"
#include "search.h"
#include "searchresult.h"

class Mission {
private:
    Map                     map;
    EnvironmentOptions      options;
    Sipp*                   search;
    SearchResult            searchResult;
    tinyxml2::XMLDocument   inputDoc;
    const char*             fileName;
public:
    Mission(const char *taskFile);
    ~Mission();
    bool ParseTask();
    bool RunTask();
    void WriteResultToConsole();
    void SaveResultToOutputDocument();
    void SavePathToOutputDocument(tinyxml2::XMLElement *log);
};