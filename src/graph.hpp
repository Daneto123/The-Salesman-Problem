#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace tsp {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

// Complete weighted graph stored as a dense n x n distance matrix.
// Directed in general: dist(a, b) may differ from dist(b, a) (ATSP).
class Graph {
public:
    Graph() = default;
    explicit Graph(int n);

    // Builds a graph from 2D points using rounded Euclidean distances
    // (TSPLIB EUC_2D convention).
    static Graph from_points(std::vector<Point> points);

    int size() const { return n_; }

    long long dist(int from, int to) const {
        return weights_[index(from, to)];
    }

    void set_dist(int from, int to, long long w) {
        weights_[index(from, to)] = w;
    }

    bool is_symmetric() const;

    bool has_coordinates() const { return !points_.empty(); }
    const std::vector<Point>& points() const { return points_; }

private:
    std::size_t index(int from, int to) const {
        return static_cast<std::size_t>(from) * static_cast<std::size_t>(n_) +
               static_cast<std::size_t>(to);
    }

    int n_ = 0;
    std::vector<long long> weights_;
    std::vector<Point> points_;
};

}  // namespace tsp
