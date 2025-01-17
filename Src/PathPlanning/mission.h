#include "map.h"
#include "anytime.h"
#include "environmentoptions.h"
#include "search.h"
#include "sipp.h"
#include "searchresult.h"
#include <memory>
#include <optional>
#include <vector>

class Mission {
private:
    Map                                 map;
    EnvironmentOptions                  options;
    std::shared_ptr<Search>             search;
    SearchResult                        searchResult;
    std::vector<const SearchResult*>    searchResults;
    tinyxml2::XMLDocument               inputDoc;
    const char*                         fileName;
    int                                 logLevel;
public:
    Mission(const char *taskFile, int _logLevel = 0);
    ~Mission();
    void ParseTask();
    void SetOptions(double hweight);
    void SetOptions(const std::optional<double>& hweight);
    void CreateSearch();
    bool RunTask();
    void WriteResultToConsole();
    void SaveResultToOutputDocument();
    void SavePathToOutputDocument(tinyxml2::XMLElement *log, const SearchResult& sresult);
    void WriteTestResult();
};