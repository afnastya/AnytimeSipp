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
    int                     logLevel;
public:
    Mission(const char *taskFile, int _logLevel = 0);
    ~Mission();
    bool ParseTask();
    void SetHweight(double hweight);
    bool RunTask();
    void WriteResultToConsole();
    void SaveResultToOutputDocument();
    void SavePathToOutputDocument(tinyxml2::XMLElement *log);
    void WriteTestResult();
};