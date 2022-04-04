cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME "SippSearch")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -O2 -Wall -Wextra -fsanitize=address")

add_subdirectory(Src/PathPlanning)
add_subdirectory(Tests)