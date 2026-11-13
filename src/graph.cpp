#include "graph.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace tsp {

Graph::Graph(int n) : n_(n) {
    if (n < 0) {
        throw std::invalid_argument("graph size must be non-negative");
    }
    weights_.assign(static_cast<std::size_t>(n) * static_cast<std::size_t>(n), 0);
}

Graph Graph::from_points(std::vector<Point> points) {
    Graph g(static_cast<int>(points.size()));
    for (int i = 0; i < g.n_; ++i) {
        for (int j = 0; j < g.n_; ++j) {
            if (i == j) continue;
            const double dx = points[i].x - points[j].x;
            const double dy = points[i].y - points[j].y;
            g.set_dist(i, j, std::llround(std::sqrt(dx * dx + dy * dy)));
        }
    }
    g.points_ = std::move(points);
    return g;
}

bool Graph::is_symmetric() const {
    for (int i = 0; i < n_; ++i) {
        for (int j = i + 1; j < n_; ++j) {
            if (dist(i, j) != dist(j, i)) return false;
        }
    }
    return true;
}

}  // namespace tsp
