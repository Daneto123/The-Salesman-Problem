#pragma once

#include <string>
#include <vector>

#include "graph.hpp"

namespace tsp {

enum class InputType { Auto, Matrix, Points };

struct Instance {
    Graph graph;
    InputType type = InputType::Auto;  // what was actually loaded
    std::vector<std::string> warnings;
};

// Loads an instance from a file. Two formats are supported:
//
// 1. The simple format used by the files in data/:
//      n
//      followed by either an n x n distance matrix   (InputType::Matrix)
//      or n lines "id x y"                           (InputType::Points)
//    A trailing "EOF" marker is ignored. If the header disagrees with the
//    amount of data (e.g. ftv64 is really 65 x 65) the data wins and a
//    warning is added.
//
// 2. TSPLIB files with EDGE_WEIGHT_TYPE EUC_2D (NODE_COORD_SECTION) or
//    EXPLICIT / FULL_MATRIX (EDGE_WEIGHT_SECTION).
//
// Throws std::runtime_error on errors.
Instance load_instance(const std::string& path, InputType type = InputType::Auto);

// Parses an instance from text (used by load_instance and by the tests).
Instance parse_instance(const std::string& text, InputType type = InputType::Auto);

const char* to_string(InputType type);

}  // namespace tsp
