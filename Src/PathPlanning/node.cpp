#include "node.h"

std::function<bool(Node*, Node*)> CreateNodeCompare(double hweight) {
    return [hweight] (const Node* lhs, const Node* rhs) {
        double lhs_F = lhs->getWeightedF(hweight);
        double rhs_F = rhs->getWeightedF(hweight);
        return std::tie(lhs_F, rhs->g, lhs->interval, lhs->i, lhs->j) < std::tie(rhs_F, lhs->g, rhs->interval, rhs->i, rhs->j);
    };
}

std::function<bool(Node*, Node*)> CreateNodeCompare() {
    return [](const Node* lhs, const Node* rhs) {
        return std::tie(lhs->F, rhs->g, lhs->interval, lhs->i, lhs->j) < std::tie(rhs->F, lhs->g, rhs->interval, rhs->i, rhs->j);
    };
}