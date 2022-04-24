#include "map.h"
#include "environmentoptions.h"
#include "search.h"
#include "searchresult.h"
#include <memory>

class Mission {
private:
    Map                     map;
    EnvironmentOptions      options;
    std::shared_ptr<Sipp>   search;
    SearchResult            searchResult;
    tinyxml2::XMLDocument   inputDoc;
    const char*             fileName;
    int                     logLevel;
public:
    Mission(const char *taskFile, int _logLevel = 0);
    ~Mission();
    bool ParseTask();
    bool RunTask();
    void WriteResultToConsole();
    void SaveResultToOutputDocument();
    void SavePathToOutputDocument(tinyxml2::XMLElement *log, const std::list<Node>& nodesPath);
    void WriteTestResult();
};