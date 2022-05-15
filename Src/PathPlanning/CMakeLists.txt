set(HEADERS         anytime.h
                    dynamicobstacle.h
                    environmentoptions.h
                    map.h
                    mission.h
                    node.h
                    search.h
                    searchresult.h
                    sipp.h
                    tinyxml2.h)

set(SOURCES         anytime.cpp
                    environmentoptions.cpp
                    map.cpp
                    mission.cpp
                    node.cpp
                    sipp.cpp
                    tinyxml2.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(tmp test.cpp)
target_link_libraries(tmp ${Boost_LIBRARIES})