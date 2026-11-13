#include "algorithms.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace tsp {

long long tour_length(const Graph& g, const Tour& tour) {
    if (tour.size() < 2) return 0;
    long long total = 0;
    for (std::size_t i = 0; i + 1 < tour.size(); ++i) {
        total += g.dist(tour[i], tour[i + 1]);
    }
    total += g.dist(tour.back(), tour.front());  // back home
    return total;
}

bool is_valid_tour(const Graph& g, const Tour& tour) {
    if (static_cast<int>(tour.size()) != g.size()) return false;
    std::vector<char> seen(tour.size(), 0);
    for (int city : tour) {
        if (city < 0 || city >= g.size() || seen[city]) return false;
        seen[city] = 1;
    }
    return true;
}

Tour rotate_to_start(Tour tour, int start) {
    const auto it = std::find(tour.begin(), tour.end(), start);
    if (it != tour.end()) std::rotate(tour.begin(), it, tour.end());
    return tour;
}

Tour nearest_neighbour(const Graph& g, int start) {
    const int n = g.size();
    if (start < 0 || start >= n) throw std::out_of_range("start city out of range");

    std::vector<char> visited(static_cast<std::size_t>(n), 0);
    Tour tour;
    tour.reserve(static_cast<std::size_t>(n));

    int current = start;
    visited[current] = 1;
    tour.push_back(current);

    for (int step = 1; step < n; ++step) {
        int best = -1;
        long long best_dist = std::numeric_limits<long long>::max();
        for (int next = 0; next < n; ++next) {
            if (!visited[next] && g.dist(current, next) < best_dist) {
                best_dist = g.dist(current, next);
                best = next;
            }
        }
        visited[best] = 1;
        tour.push_back(best);
        current = best;
    }
    return tour;
}

Tour repeated_nearest_neighbour(const Graph& g, int start) {
    Tour best;
    long long best_len = std::numeric_limits<long long>::max();
    for (int s = 0; s < g.size(); ++s) {
        Tour candidate = nearest_neighbour(g, s);
        const long long len = tour_length(g, candidate);
        if (len < best_len) {
            best_len = len;
            best = std::move(candidate);
        }
    }
    return rotate_to_start(std::move(best), start);
}

Tour two_opt(const Graph& g, Tour t) {
    const int n = static_cast<int>(t.size());
    if (n < 4) return t;
    const bool symmetric = g.is_symmetric();

    // Move (i, j): reverse t[i..j]. Edges a->b and c->d are replaced by a->c
    // and b->d. In an asymmetric graph the edges inside the segment change
    // direction too, so we keep the cost of the segment walked forwards and
    // backwards and update both in O(1) as j grows.
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 1; i < n - 1; ++i) {
            long long forward = 0;
            long long backward = 0;
            for (int j = i + 1; j < n; ++j) {
                const int a = t[i - 1];
                const int b = t[i];
                const int c = t[j];
                const int d = t[(j + 1) % n];

                if (!symmetric) {
                    forward += g.dist(t[j - 1], t[j]);
                    backward += g.dist(t[j], t[j - 1]);
                }

                long long delta = g.dist(a, c) + g.dist(b, d) - g.dist(a, b) - g.dist(c, d);
                if (!symmetric) delta += backward - forward;

                if (delta < 0) {
                    std::reverse(t.begin() + i, t.begin() + j + 1);
                    improved = true;
                    // The segment starting at i changed: rescan j from i + 1.
                    forward = 0;
                    backward = 0;
                    j = i;
                }
            }
        }
    }
    return t;
}

std::optional<Tour> held_karp(const Graph& g, int start) {
    const int n = g.size();
    if (start < 0 || start >= n) throw std::out_of_range("start city out of range");
    if (n > kHeldKarpMaxCities) return std::nullopt;
    if (n <= 2) {
        Tour t;
        for (int i = 0; i < n; ++i) t.push_back(i);
        return rotate_to_start(t, start);
    }

    // Cities other than `start` get indices 0..m-1.
    const int m = n - 1;
    std::vector<int> city(static_cast<std::size_t>(m));
    for (int i = 0, k = 0; i < n; ++i) {
        if (i != start) city[k++] = i;
    }

    const std::size_t subsets = std::size_t{1} << m;
    constexpr long long kInf = std::numeric_limits<long long>::max() / 4;
    // cost[mask * m + j]: shortest path from start through `mask`, ending at j.
    std::vector<long long> cost(subsets * m, kInf);
    std::vector<std::int8_t> parent(subsets * m, -1);

    for (int j = 0; j < m; ++j) {
        cost[(std::size_t{1} << j) * m + j] = g.dist(start, city[j]);
    }

    for (std::size_t mask = 1; mask < subsets; ++mask) {
        for (int j = 0; j < m; ++j) {
            if (!(mask & (std::size_t{1} << j))) continue;
            const long long here = cost[mask * m + j];
            if (here >= kInf) continue;
            for (int k = 0; k < m; ++k) {
                if (mask & (std::size_t{1} << k)) continue;
                const std::size_t next = mask | (std::size_t{1} << k);
                const long long candidate = here + g.dist(city[j], city[k]);
                if (candidate < cost[next * m + k]) {
                    cost[next * m + k] = candidate;
                    parent[next * m + k] = static_cast<std::int8_t>(j);
                }
            }
        }
    }

    const std::size_t full = subsets - 1;
    int last = 0;
    long long best = kInf;
    for (int j = 0; j < m; ++j) {
        const long long total = cost[full * m + j] + g.dist(city[j], start);
        if (total < best) {
            best = total;
            last = j;
        }
    }

    Tour reversed;
    std::size_t mask = full;
    for (int j = last; j != -1;) {
        reversed.push_back(city[j]);
        const int prev = parent[mask * m + j];
        mask &= ~(std::size_t{1} << j);
        j = prev;
    }

    Tour tour{start};
    tour.insert(tour.end(), reversed.rbegin(), reversed.rend());
    return tour;
}

}  // namespace tsp
