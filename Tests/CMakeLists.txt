set(HEADERS         ../Src/PathPlanning/tinyxml2.h
                    vector.h
                    path.h)

set(SOURCES         ../Src/PathPlanning/tinyxml2.cpp)

add_executable(Generate ${HEADERS} ${SOURCES} generator.cpp)
add_executable(Check ${HEADERS} ${SOURCES} check.cpp)