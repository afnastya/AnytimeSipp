#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include "tinyxml2.h"
#include "dynamicobstacle.h"

struct Interval {
    bool isSafe;
    int startTime;
    int endTime;

    Interval() {}
    Interval(bool _isSafe, int _startTime, int _endTime) : isSafe(_isSafe), startTime(_startTime), endTime(_endTime) {}
    bool operator==(const Interval& other) {
        return startTime == other.startTime && endTime == other.endTime;
    }

    friend std::ostream& operator<<(std::ostream& out, const Interval& interval) {
        out << "{" << interval.isSafe << ", " << interval.startTime << ", " << interval.endTime << "}";
        return out;
    }
};

class Map {
private:
    int width, height;
    int start_i, start_j;
    int goal_i, goal_j;
    static const int cost = 10;
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
    int getCost() const;
    int getDistance(int i1, int j1, int i2, int j2) const;

    bool CellIsTraversable(int i, int j) const;
    const Interval& operator()(int i, int j, int interval) const;
    const std::vector<Interval>& operator()(int i, int j) const;

    void setIntervals(int i, int j);
    int getSafeIntervalId(int i, int j, int time) const;
    int getIntervalStart(int i, int j, int interval) const;
    int getIntervalEnd(int i, int j, int interval) const;

    void printIntervals(int i, int j) const;
};