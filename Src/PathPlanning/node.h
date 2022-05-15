#pragma once
#include <functional>
#include <tuple>

struct Node {
    int     i, j, interval, g, F;
    double  H;
    Node    *parent;
    bool opt;

    Node(int _i, int _j, int _interval, int _g, double _H, Node* _parent, bool _opt = true) {
        i = _i;
        j = _j;
        interval = _interval;
        g = _g;
        H = _H;
        parent = _parent;
        opt = _opt;
        updateF();
    }

    void updateF() {
        F = g + H;
    }

    double getWeightedF(double hweight) const {
        if (opt) {
            return hweight * (H + g);
        } else {
            return hweight * H + g;
        }
    }
};

std::function<bool(Node*, Node*)> CreateNodeCompare(double hweight);
std::function<bool(Node*, Node*)> CreateNodeCompare();