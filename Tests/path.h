#include "vector.h"
#include <vector>

struct PathComponent {
    int x, y;
    double time;

    friend std::ostream& operator<<(std::ostream& out, const PathComponent& p) {
        out << "(" << p.x << ", " << p.y << ", " << p.time << ")";
        return out;
    }
};

struct PathSegment {
    PathComponent start, end;

    friend std::ostream& operator<<(std::ostream& out, const PathSegment& p) {
        out << "{" << p.start << ", " << p.end << "}";
        return out;
    }
};

double GetTime(int x, int y, const PathComponent& p1) {        // no diagonal steps
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

        int dx = abs(p1.end.x - p1.start.x) + abs(p2.end.x - p2.start.x);
        Vector<int> p1_time1(dx == 0 ? p1.start.y : p1.start.x, p1.start.time);
        Vector<int> p1_time2(dx == 0 ? p1.end.y : p1.end.x, p1.end.time);
        Vector<int> p2_time1(dx == 0 ? p2.start.y : p2.start.x, p2.start.time);
        Vector<int> p2_time2(dx == 0 ? p2.end.y : p2.end.x, p2.end.time);

        I = GetIntersection(p1_time1, p1_time2, p2_time1, p2_time2);

        if (!std::get_if<Intersection>(&I) || std::get<Intersection>(I) != Intersection::NONE) {
            // std::cout << p1 << " " << p2 << std::endl;
            return true;
        }
    } else {
        Vector<int> P = std::get<Vector<double>>(I);
        double time1 = GetTime(P.x, P.y, p1.start);
        double time2 = GetTime(P.x, P.y, p2.start);

        if (time1 == time2) {
            // std::cout << p1 << " " << p2 << std::endl;
            return true;
        }
    }

    return false;
}

bool PathsIntersect(const PathSegment& segment, const std::vector<std::vector<PathComponent>>& paths) {
    for (const auto& path : paths) {
        for (uint32_t j = 1; j < path.size(); ++j) {
            PathSegment path_segment = {path[j - 1], path[j]};

            if (PathsIntersect(segment, path_segment)) {
                return true;
            }
        }
    }

    return false;
}