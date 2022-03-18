#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include "tinyxml2.h"
#include "dynamicobstacle.h"

struct Interval {
    bool isSafe;
    double startTime;
    double endTime;

    Interval() {}
    Interval(bool _isSafe, double _startTime, double _endTime) : isSafe(_isSafe), startTime(_startTime), endTime(_endTime) {}
    bool operator==(const Interval& other) {
        return startTime == other.startTime && endTime == other.endTime;
    }
};

class Map {
private:
    int width, height;
    int start_i, start_j;
    int goal_i, goal_j;
public:
    std::vector<std::vector<std::vector<Interval>>> map;
    std::vector<Obstacle> dynamicObstacles;
public:
    Map();
    // ~Map();

    bool getMap(tinyxml2::XMLElement* map);

    int getWidth() const;
    int getHeight() const;
    std::pair<int, int> getStartCell() const;
    std::pair<int, int> getGoalCell() const;

    bool CellIsTraversable(int i, int j) const;
    const Interval& operator()(int i, int j, int interval) const;
    const std::vector<Interval>& operator()(int i, int j) const;

    void setIntervals(int i, int j);
    int getSafeIntervalId(int i, int j, double time) const;
    double getIntervalStart(int i, int j, int interval) const;
    double getIntervalEnd(int i, int j, int interval) const;

    void printIntervals(int i, int j);
};