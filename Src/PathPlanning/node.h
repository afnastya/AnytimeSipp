#pragma once

struct Node {
    int     i, j, interval, g;
    double  F, H, w;
    Node    *parent;

    Node(int _i, int _j, int _interval, int _g, double _H, double _w, Node* _parent) {
        i = _i;
        j = _j;
        interval = _interval;
        g = _g;
        H = _H;
        w = _w;
        parent = _parent;
        updateF();
    }

    void updateF() {
        F = w * H + g;
    }
};