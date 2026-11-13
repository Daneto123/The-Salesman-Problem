#include "io.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace tsp {

namespace {

bool parse_number(const std::string& token, double& out) {
    if (token.empty()) return false;
    char* end = nullptr;
    out = std::strtod(token.c_str(), &end);
    return end == token.c_str() + token.size();
}

std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

std::string upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}

Graph graph_from_matrix(int n, const std::vector<double>& values) {
    Graph g(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const auto w = std::llround(values[static_cast<std::size_t>(i) * n + j]);
            g.set_dist(i, j, i == j ? 0 : w);
        }
    }
    return g;
}

Graph graph_from_triplets(int n, const std::vector<double>& values) {
    std::vector<Point> points(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        // values[3i] is the city id, which we do not need.
        points[i].x = values[3 * static_cast<std::size_t>(i) + 1];
        points[i].y = values[3 * static_cast<std::size_t>(i) + 2];
    }
    return Graph::from_points(std::move(points));
}

long long perfect_square_root(std::size_t value) {
    const auto r = static_cast<long long>(std::llround(std::sqrt(static_cast<double>(value))));
    return static_cast<std::size_t>(r * r) == value ? r : -1;
}

Instance parse_simple(std::istringstream& in, InputType type) {
    Instance result;

    std::string token;
    in >> token;
    double header = 0;
    if (!parse_number(token, header) || header < 1) {
        throw std::runtime_error("expected the number of cities on the first line");
    }
    const auto n = static_cast<long long>(header);

    std::vector<double> values;
    double v = 0;
    while (in >> token) {
        if (!parse_number(token, v)) {
            if (upper(token) == "EOF") break;
            throw std::runtime_error("unexpected token '" + token + "'");
        }
        values.push_back(v);
    }
    const std::size_t count = values.size();

    const bool fits_matrix = count == static_cast<std::size_t>(n * n);
    const bool fits_points = count == static_cast<std::size_t>(3 * n);

    if (type == InputType::Auto) {
        if (fits_matrix) {
            type = InputType::Matrix;  // also wins the n == 3 tie
        } else if (fits_points) {
            type = InputType::Points;
        } else if (perfect_square_root(count) > 0) {
            type = InputType::Matrix;
        } else if (count % 3 == 0) {
            type = InputType::Points;
        } else {
            throw std::runtime_error("cannot tell whether the file holds a matrix or points (" +
                                     std::to_string(count) + " numbers after the header)");
        }
    }

    long long real_n = n;
    if (type == InputType::Matrix && !fits_matrix) {
        real_n = perfect_square_root(count);
        if (real_n <= 0) {
            throw std::runtime_error("matrix data is not square (" + std::to_string(count) +
                                     " numbers after the header)");
        }
    } else if (type == InputType::Points && !fits_points) {
        if (count % 3 != 0) {
            throw std::runtime_error("points data is not a multiple of 3 numbers");
        }
        real_n = static_cast<long long>(count / 3);
    }
    if (real_n != n) {
        result.warnings.push_back("header says " + std::to_string(n) + " cities, but the data has " +
                                  std::to_string(real_n) + "; using " + std::to_string(real_n));
    }

    const int size = static_cast<int>(real_n);
    result.type = type;
    result.graph = type == InputType::Matrix ? graph_from_matrix(size, values)
                                             : graph_from_triplets(size, values);
    return result;
}

Instance parse_tsplib(std::istringstream& in) {
    Instance result;
    long long dimension = -1;
    std::string weight_type;
    std::string weight_format = "FULL_MATRIX";
    std::vector<double> values;
    bool have_coords = false;
    bool have_matrix = false;

    auto read_numbers = [&](std::size_t how_many) {
        values.clear();
        std::string token;
        double v = 0;
        while (values.size() < how_many && in >> token) {
            if (!parse_number(token, v)) {
                throw std::runtime_error("unexpected token '" + token + "' in data section");
            }
            values.push_back(v);
        }
        if (values.size() != how_many) {
            throw std::runtime_error("data section ended early");
        }
    };

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        const std::string key_upper = upper(line);

        if (key_upper == "EOF") break;

        if (key_upper == "NODE_COORD_SECTION") {
            if (dimension <= 0) throw std::runtime_error("DIMENSION must come before NODE_COORD_SECTION");
            if (weight_type != "EUC_2D") {
                throw std::runtime_error("only EDGE_WEIGHT_TYPE EUC_2D is supported for coordinates (got '" +
                                         weight_type + "')");
            }
            read_numbers(static_cast<std::size_t>(3 * dimension));
            have_coords = true;
            continue;
        }
        if (key_upper == "EDGE_WEIGHT_SECTION") {
            if (dimension <= 0) throw std::runtime_error("DIMENSION must come before EDGE_WEIGHT_SECTION");
            if (weight_format != "FULL_MATRIX") {
                throw std::runtime_error("only EDGE_WEIGHT_FORMAT FULL_MATRIX is supported (got '" +
                                         weight_format + "')");
            }
            read_numbers(static_cast<std::size_t>(dimension * dimension));
            have_matrix = true;
            continue;
        }

        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;  // unknown section, skip
        const std::string key = upper(trim(line.substr(0, colon)));
        const std::string value = trim(line.substr(colon + 1));

        if (key == "DIMENSION") {
            double d = 0;
            if (!parse_number(value, d) || d < 1) throw std::runtime_error("bad DIMENSION");
            dimension = static_cast<long long>(d);
        } else if (key == "EDGE_WEIGHT_TYPE") {
            weight_type = upper(value);
        } else if (key == "EDGE_WEIGHT_FORMAT") {
            weight_format = upper(value);
        }
    }

    const int n = static_cast<int>(dimension);
    if (have_coords) {
        result.type = InputType::Points;
        result.graph = graph_from_triplets(n, values);
    } else if (have_matrix) {
        result.type = InputType::Matrix;
        result.graph = graph_from_matrix(n, values);
    } else {
        throw std::runtime_error("TSPLIB file has no NODE_COORD_SECTION or EDGE_WEIGHT_SECTION");
    }
    return result;
}

}  // namespace

Instance parse_instance(const std::string& text, InputType type) {
    std::istringstream in(text);
    std::string first;
    {
        std::istringstream peek(text);
        peek >> first;
    }
    if (first.empty()) throw std::runtime_error("file is empty");

    double ignored = 0;
    if (parse_number(first, ignored)) {
        return parse_simple(in, type);
    }
    return parse_tsplib(in);
}

Instance load_instance(const std::string& path, InputType type) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("cannot open '" + path + "'");
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return parse_instance(buffer.str(), type);
}

const char* to_string(InputType type) {
    switch (type) {
        case InputType::Matrix: return "matrix";
        case InputType::Points: return "points";
        case InputType::Auto: break;
    }
    return "auto";
}

}  // namespace tsp
