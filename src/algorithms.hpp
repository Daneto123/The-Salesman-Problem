#pragma once

#include <optional>
#include <vector>

#include "graph.hpp"

namespace tsp {

// A tour visits every city exactly once; the edge from the last city back to
// the first is implied. tour[0] is the starting city.
using Tour = std::vector<int>;

// Largest graph the exact Held-Karp solver accepts (memory grows as n * 2^n).
constexpr int kHeldKarpMaxCities = 18;

long long tour_length(const Graph& g, const Tour& tour);
bool is_valid_tour(const Graph& g, const Tour& tour);

// Rotates the tour so that it begins at `start` (direction is kept).
Tour rotate_to_start(Tour tour, int start);

// Greedy: always go to the closest unvisited city. O(n^2).
Tour nearest_neighbour(const Graph& g, int start);

// Runs nearest neighbour from every city and keeps the shortest tour,
// rotated to begin at `start`. O(n^3).
Tour repeated_nearest_neighbour(const Graph& g, int start);

// Improves a tour with 2-opt moves (reverse a segment) until no move helps.
// Works for both symmetric and asymmetric graphs; each move is evaluated in
// O(1), so one pass over all moves is O(n^2). Keeps tour[0] in place.
Tour two_opt(const Graph& g, Tour tour);

// Improves a tour with Or-opt moves: take a chain of 1-3 consecutive cities
// and move it (as is or reversed) to the best other place in the tour, until
// no move helps. Often better than 2-opt on asymmetric graphs because the
// moved chain keeps its direction. Keeps tour[0] in place.
Tour or_opt(const Graph& g, Tour tour);

// Alternates 2-opt and Or-opt until neither can improve the tour.
Tour two_opt_or_opt(const Graph& g, Tour tour);

// Exact optimum by dynamic programming (Held-Karp), O(n^2 * 2^n).
// Returns std::nullopt if the graph has more than kHeldKarpMaxCities cities.
std::optional<Tour> held_karp(const Graph& g, int start);

}  // namespace tsp
