#pragma once

struct Node {
    int     i, j, interval;
    double  F, g, H, w;
    Node    *parent;

    Node(int _i, int _j, int _interval, double _g, double _H, double _w, Node* _parent) {
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
        F = g + w * H;
    }
};