include_directories(../Src/PathPlanning)

set(HEADERS         ../Src/PathPlanning/tinyxml2.h
                    vector.h
                    path.h)

set(SOURCES         ../Src/PathPlanning/tinyxml2.cpp)

add_executable(Generate ${HEADERS} ${SOURCES} generator.cpp)
add_executable(Check ${HEADERS} ${SOURCES} check.cpp)

add_test(NAME "examples"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../Examples/tasks/ &&
         ./Check ../../Examples/logs")

add_test(NAME "examples/anytime"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../Examples/tasks/ -w 10 &&
         ./Check ../../Examples/logs_anytime")

add_test(NAME "preparation/run"
         COMMAND bash -c "echo \"taskfile,width,height,obstaclesN,found,length,nodescreated,numberofsteps,searchtime\" >../../Tests/results.csv")

add_test(NAME "256/small_tasks/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_256/small_tasks/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_256/small_tasks/logs")

add_test(NAME "256/tasks_500/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_256/tasks_500/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_256/tasks_500/logs")

add_test(NAME "256/tasks_1000/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_256/tasks_1000/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_256/tasks_1000/logs")

add_test(NAME "512/small_tasks/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_512/small_tasks/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_512/small_tasks/logs")

add_test(NAME "512/tasks_500/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_512/tasks_500/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_512/tasks_500/logs")

add_test(NAME "512/tasks_1000/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_512/tasks_1000/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_512/tasks_1000/logs")

add_test(NAME "1024/small_tasks/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_1024/small_tasks/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_1024/small_tasks/logs")

add_test(NAME "1024/tasks_500/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_1024/tasks_500/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_1024/tasks_500/logs")

add_test(NAME "1024/tasks_1000/run"
         COMMAND bash -c "./../Src/PathPlanning/SippSearch ../../TestsData/City/tests_1024/tasks_1000/tasks/ -l -2 >>../../Tests/results.csv &&
         ./Check ../../TestsData/City/tests_1024/tasks_1000/logs")

add_test(NAME "256/small_tasks/check"
         COMMAND ./Check ../../TestsData/City/tests_256/small_tasks/logs)

add_test(NAME "256/tasks_500/check"
         COMMAND ./Check ../../TestsData/City/tests_256/tasks_500/logs)

add_test(NAME "256/tasks_1000/check"
         COMMAND ./Check ../../TestsData/City/tests_256/tasks_1000/logs)

add_test(NAME "512/small_tasks/check"
         COMMAND ./Check ../../TestsData/City/tests_512/small_tasks/logs)

add_test(NAME "512/tasks_500/check"
         COMMAND ./Check ../../TestsData/City/tests_512/tasks_500/logs)

add_test(NAME "512/tasks_1000/check"
         COMMAND ./Check ../../TestsData/City/tests_512/tasks_1000/logs)

add_test(NAME "1024/small_tasks/check"
         COMMAND ./Check ../../TestsData/City/tests_1024/small_tasks/logs)

add_test(NAME "1024/tasks_500/check"
         COMMAND ./Check ../../TestsData/City/tests_1024/tasks_500/logs)

add_test(NAME "1024/tasks_1000/check"
         COMMAND ./Check ../../TestsData/City/tests_1024/tasks_1000/logs)