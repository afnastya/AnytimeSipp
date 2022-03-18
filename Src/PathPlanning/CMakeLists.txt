set(HEADERS         dynamicobstacle.h
                    environmentoptions.h
                    map.h
                    mission.h
                    node.h
                    search.h
                    searchresult.h
                    tinyxml2.h)

set(SOURCES         environmentoptions.cpp
                    map.cpp
                    mission.cpp
                    search.cpp
                    tinyxml2.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)