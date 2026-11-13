#pragma once

#include <string>
#include <vector>

#include "algorithms.hpp"
#include "graph.hpp"

namespace tsp {

struct NamedTour {
    std::string name;
    Tour tour;
    long long length = 0;
};

// Draws each tour side by side in one SVG file (open it in any browser).
// Needs city coordinates, so it only works for "points" instances.
// Throws std::runtime_error if the graph has no coordinates or the file
// cannot be written.
void write_svg(const std::string& path, const Graph& g, const std::vector<NamedTour>& tours,
               const std::string& title);

}  // namespace tsp
