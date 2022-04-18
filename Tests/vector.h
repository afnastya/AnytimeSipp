#include <iostream>
#include <variant>
#include <cmath>

enum class Intersection {
    POINT,
    NONE,
    INFINITE
};

template <typename T>
class Vector {
public:
    T x;
    T y;

    Vector() {}
    Vector(T _x, T _y) : x(_x), y(_y) {}
    
    template <typename T1>
    Vector(const Vector<T1>& other) : x(other.x), y(other.y) {}

    template <typename T1>
    Vector(Vector<T1>&& other) : x(other.x), y(other.y) {}

    template <typename T1>
    Vector& operator=(Vector<T1>& other) {
        x = other.x;
        y = other.y;
    }

    template <typename T1>
    Vector& operator=(Vector<T1>&& other) {
        x = other.x;
        y = other.y;
    }

    template <typename T1>
    Vector operator+(Vector<T1>& other) {
        return Vector(x + other.x, y + other.y);
    }

    template <typename T1>
    Vector operator-(Vector<T1>& other) {
        return Vector(x - other.x, y - other.y);
    }

    template <typename S>
    Vector<S> operator*(S scalar) {
        return Vector<S>(scalar * x, scalar * y);
    }

    template <typename T1>
    bool operator==(const Vector<T1>& other) const {
        return x == other.x && y == other.y;
    }

    friend std::istream& operator>>(std::istream& in, Vector& A) {
        in >> A.x >> A.y;
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const Vector A) {
        out << A.x << " " << A.y << "\n";
        return out;
    }

    friend T VectorProduct(Vector A, Vector B) {
        return A.x * B.y - B.x * A.y;
    }

    friend T ScalarProduct(Vector A, Vector B) {
        return A.x * B.x + A.y * B.y;
    }

    friend bool PointOnSegment(Vector P, Vector A, Vector B) {
        return VectorProduct(P - A, B - P) == 0
               && ScalarProduct(P - A, B - P) >= 0;
    }

    friend bool SegmentIntersectsLine(Vector A, Vector B, Vector C, Vector D) {
        return (VectorProduct(A - C, D - C) >= 0 && 
                VectorProduct(B - C, D - C) <= 0) || 
               (VectorProduct(A - C, D - C) <= 0 && 
                VectorProduct(B - C, D - C) >= 0);
    }

    friend bool SegmentsIntersect(Vector A, Vector B, Vector C, Vector D) {
        return SegmentIntersectsLine(A, B, C, D)
               && SegmentIntersectsLine(C, D, A, B)
               && std::min(A.x, B.x) <= std::max(C.x, D.x)
               && std::min(C.x, D.x) <= std::max(A.x, B.x)
               && std::min(A.y, B.y) <= std::max(C.y, D.y)
               && std::min(C.y, D.y) <= std::max(A.y, B.y);
    }

    friend std::variant<Intersection, Vector<double>> GetIntersection(Vector A, Vector B, Vector C, Vector D) {
        if ((A == B && PointOnSegment(A, C, D)) || (C == D && PointOnSegment(C, A, B))) {
            return Intersection::POINT;
        }

        if (!SegmentsIntersect(A, B, C, D)) {
            return Intersection::NONE;
        }

        if (VectorProduct(B - A, D - C) == 0) {
            if (VectorProduct(C - A, B - A) != 0) {
                return Intersection::NONE;
            } else if (std::min(A.x, B.x) > std::max(C.x, D.x) || std::min(C.x, D.x) > std::max(A.x, B.x)) {
                return Intersection::NONE;
            } else {
                return Intersection::INFINITE;
            }
        }

        double k = (double(VectorProduct(C - A, C - B)) / (VectorProduct(C - A, C - D) + VectorProduct(C - D, C - B)));
        Vector<double> E = Vector(D - C) * k + C;
        return E;
    }
};