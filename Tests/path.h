#include "vector.h"

struct PathComponent {
    int x, y;
    double time;
};

struct PathSegment {
    PathComponent start, end;
};

double GetTime(int x, int y, const PathComponent& p1, const PathComponent& p2) {        // no diagonal steps
    return p1.time + abs(x - p1.x) + abs(y - p1.y);
}

bool PathsIntersect(const PathSegment& p1, const PathSegment& p2) {
    Vector<int> p1_xy1(p1.start.x, p1.start.y), p1_xy2(p1.end.x, p1.end.y);
    Vector<int> p2_xy1(p2.start.x, p2.start.y), p2_xy2(p2.end.x, p2.end.y);

    auto I = GetIntersection(p1_xy1, p1_xy2, p2_xy1, p2_xy2);

    if (std::get_if<Intersection>(&I)) {
        if (std::get<Intersection>(I) == Intersection::NONE) {
            return false;
        }

        int dx1 = p1.end.x - p1.start.x;
        Vector<int> p1_time1(dx1 == 0 ? p1.start.y : p1.start.x, p1.start.time);
        Vector<int> p1_time2(dx1 == 0 ? p1.end.y : p1.end.x, p1.end.time);
        Vector<int> p2_time1(dx1 == 0 ? p2.start.y : p2.start.x, p2.start.time);
        Vector<int> p2_time2(dx1 == 0 ? p2.end.y : p2.end.x, p2.end.time);

        I = GetIntersection(p1_time1, p1_time2, p2_time1, p2_time2);

        if (!std::get_if<Intersection>(&I) || std::get<Intersection>(I) != Intersection::NONE) {
            return true;
        }
    } else {
        Vector<int> P = std::get<Vector<double>>(I);
        double time1 = GetTime(P.x, P.y, p1.start, p1.end);
        double time2 = GetTime(P.x, P.y, p2.start, p2.end);

        if (time1 == time2) {
            return true;
        }
    }

    return false;
}

bool PathsIntersect(const PathSegment& segment, const std::vector<std::vector<PathComponent>>& paths) {
    for (const auto& path : paths) {
        for (int j = 1; j < path.size(); ++j) {
            PathSegment path_segment = {path[j - 1], path[j]};

            if (PathsIntersect(segment, path_segment)) {
                return true;
            }
        }
    }

    return false;
}