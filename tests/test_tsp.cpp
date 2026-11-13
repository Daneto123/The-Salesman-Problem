// Small self-contained test runner: no external libraries needed.

#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "algorithms.hpp"
#include "io.hpp"

namespace {

int g_failures = 0;
int g_checks = 0;

#define CHECK(cond)                                                                       \
    do {                                                                                  \
        ++g_checks;                                                                       \
        if (!(cond)) {                                                                    \
            ++g_failures;                                                                 \
            std::cerr << __FILE__ << ':' << __LINE__ << ": CHECK failed: " #cond "\n"; \
        }                                                                                 \
    } while (0)

#define CHECK_EQ(a, b)                                                                     \
    do {                                                                                   \
        ++g_checks;                                                                        \
        const auto va = (a);                                                               \
        const auto vb = (b);                                                               \
        if (!(va == vb)) {                                                                 \
            ++g_failures;                                                                  \
            std::cerr << __FILE__ << ':' << __LINE__ << ": CHECK_EQ failed: " #a " == " #b \
                      << " (" << va << " vs " << vb << ")\n";                              \
        }                                                                                  \
    } while (0)

std::string data(const std::string& name) { return std::string(TSP_DATA_DIR) + "/" + name; }

tsp::Graph random_graph(int n, bool symmetric, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> w(1, 100);
    tsp::Graph g(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            if (symmetric && j < i) {
                g.set_dist(i, j, g.dist(j, i));
            } else {
                g.set_dist(i, j, w(rng));
            }
        }
    }
    return g;
}

long long brute_force(const tsp::Graph& g) {
    std::vector<int> rest(static_cast<std::size_t>(g.size() - 1));
    std::iota(rest.begin(), rest.end(), 1);
    long long best = -1;
    do {
        tsp::Tour t{0};
        t.insert(t.end(), rest.begin(), rest.end());
        const long long len = tsp::tour_length(g, t);
        if (best < 0 || len < best) best = len;
    } while (std::next_permutation(rest.begin(), rest.end()));
    return best;
}

void test_loads_all_sample_files() {
    struct Case {
        const char* file;
        int cities;
        tsp::InputType type;
        bool warning;
    };
    const Case cases[] = {
        {"16.tsp", 16, tsp::InputType::Points, false},
        {"arr48.tsp", 48, tsp::InputType::Points, false},
        {"pbm436.tsp", 436, tsp::InputType::Points, false},
        {"rbx711.tsp", 711, tsp::InputType::Points, false},  // ends with "EOF"
        {"xql662.tsp", 662, tsp::InputType::Points, false},
        {"br17.atsp", 17, tsp::InputType::Matrix, false},
        {"ftv64.atsp", 65, tsp::InputType::Matrix, true},    // header says 64
        {"ftv170.atsp", 171, tsp::InputType::Matrix, true},  // header says 170
        {"ftv323.atsp", 323, tsp::InputType::Matrix, false},
        {"rbg403.atsp", 403, tsp::InputType::Matrix, false},
    };
    for (const auto& c : cases) {
        const auto inst = tsp::load_instance(data(c.file));
        CHECK_EQ(inst.graph.size(), c.cities);
        CHECK(inst.type == c.type);
        CHECK_EQ(inst.warnings.empty(), !c.warning);
    }
}

void test_rounded_euclidean_distance() {
    const auto inst = tsp::parse_instance("2\n1 0 0\n2 3 4\n");
    CHECK(inst.type == tsp::InputType::Points);
    CHECK_EQ(inst.graph.dist(0, 1), 5);
    CHECK_EQ(inst.graph.dist(1, 0), 5);
    CHECK(inst.graph.is_symmetric());
}

void test_tsplib_format() {
    const auto coords = tsp::parse_instance(
        "NAME : square\nTYPE : TSP\nDIMENSION : 4\nEDGE_WEIGHT_TYPE : EUC_2D\n"
        "NODE_COORD_SECTION\n1 0 0\n2 10 0\n3 10 10\n4 0 10\nEOF\n");
    CHECK_EQ(coords.graph.size(), 4);
    CHECK_EQ(coords.graph.dist(0, 2), 14);

    const auto matrix = tsp::parse_instance(
        "NAME: tiny\nTYPE: ATSP\nDIMENSION: 3\nEDGE_WEIGHT_TYPE: EXPLICIT\n"
        "EDGE_WEIGHT_FORMAT: FULL_MATRIX\nEDGE_WEIGHT_SECTION\n9999 1 2\n3 9999 4\n5 6 9999\nEOF\n");
    CHECK_EQ(matrix.graph.size(), 3);
    CHECK_EQ(matrix.graph.dist(0, 0), 0);  // diagonal is ignored
    CHECK_EQ(matrix.graph.dist(1, 2), 4);
    CHECK(!matrix.graph.is_symmetric());
}

void test_bad_input_is_rejected() {
    bool threw = false;
    try {
        tsp::parse_instance("3\n1 2\n");
    } catch (const std::exception&) {
        threw = true;
    }
    CHECK(threw);

    threw = false;
    try {
        tsp::load_instance(data("no-such-file.tsp"));
    } catch (const std::exception&) {
        threw = true;
    }
    CHECK(threw);
}

void test_all_algorithms_return_valid_tours() {
    for (const char* file : {"16.tsp", "arr48.tsp", "br17.atsp", "ftv64.atsp"}) {
        const auto g = tsp::load_instance(data(file)).graph;
        const int start = 3;
        const auto nn = tsp::nearest_neighbour(g, start);
        const auto rnn = tsp::repeated_nearest_neighbour(g, start);
        const auto opt = tsp::two_opt(g, rnn);
        for (const auto* t : {&nn, &rnn, &opt}) {
            CHECK(tsp::is_valid_tour(g, *t));
            CHECK_EQ(t->front(), start);
        }
        CHECK(tsp::tour_length(g, rnn) <= tsp::tour_length(g, nn));
        CHECK(tsp::tour_length(g, opt) <= tsp::tour_length(g, rnn));
    }
}

void test_two_opt_untangles_a_square() {
    // 0 -> 2 -> 1 -> 3 crosses itself; the perimeter (length 40) is optimal.
    const auto g = tsp::parse_instance("4\n1 0 0\n2 10 0\n3 10 10\n4 0 10\n").graph;
    const tsp::Tour crossed{0, 2, 1, 3};
    const auto fixed = tsp::two_opt(g, crossed);
    CHECK(tsp::is_valid_tour(g, fixed));
    CHECK_EQ(tsp::tour_length(g, fixed), 40);
}

void test_held_karp_matches_brute_force() {
    for (unsigned seed = 1; seed <= 20; ++seed) {
        const bool symmetric = seed % 2 == 0;
        const auto g = random_graph(8, symmetric, seed);
        const auto exact = tsp::held_karp(g, 0);
        CHECK(exact.has_value());
        CHECK(tsp::is_valid_tour(g, *exact));
        const long long best = brute_force(g);
        CHECK_EQ(tsp::tour_length(g, *exact), best);

        // Heuristics can never beat the optimum.
        const auto opt = tsp::two_opt(g, tsp::repeated_nearest_neighbour(g, 0));
        CHECK(tsp::tour_length(g, opt) >= best);
    }
}

void test_held_karp_start_and_limits() {
    const auto g = tsp::load_instance(data("br17.atsp")).graph;
    const auto a = tsp::held_karp(g, 0);
    const auto b = tsp::held_karp(g, 5);
    CHECK(a && b);
    CHECK_EQ(b->front(), 5);
    CHECK_EQ(tsp::tour_length(g, *a), tsp::tour_length(g, *b));
    CHECK_EQ(tsp::tour_length(g, *a), 39);  // known optimum of br17 (TSPLIB)

    const auto big = tsp::load_instance(data("arr48.tsp")).graph;
    CHECK(!tsp::held_karp(big, 0).has_value());
}

void test_asymmetric_two_opt_never_gets_worse() {
    for (unsigned seed = 100; seed < 130; ++seed) {
        const auto g = random_graph(30, false, seed);
        const auto start = tsp::nearest_neighbour(g, 0);
        const auto improved = tsp::two_opt(g, start);
        CHECK(tsp::is_valid_tour(g, improved));
        CHECK(tsp::tour_length(g, improved) <= tsp::tour_length(g, start));
    }
}

void test_or_opt_never_gets_worse() {
    for (unsigned seed = 200; seed < 240; ++seed) {
        const bool symmetric = seed % 2 == 0;
        const auto g = random_graph(25, symmetric, seed);
        const auto start = tsp::repeated_nearest_neighbour(g, 2);
        const auto moved = tsp::or_opt(g, start);
        const auto both = tsp::two_opt_or_opt(g, start);
        const auto two = tsp::two_opt(g, start);
        for (const auto* t : {&moved, &both}) {
            CHECK(tsp::is_valid_tour(g, *t));
            CHECK_EQ(t->front(), 2);
            CHECK(tsp::tour_length(g, *t) <= tsp::tour_length(g, start));
        }
        CHECK(tsp::tour_length(g, both) <= tsp::tour_length(g, two));
    }
}

void test_or_opt_respects_the_optimum() {
    for (unsigned seed = 300; seed < 320; ++seed) {
        const auto g = random_graph(9, seed % 2 == 0, seed);
        const auto exact = tsp::held_karp(g, 0);
        const auto t = tsp::two_opt_or_opt(g, tsp::nearest_neighbour(g, 0));
        CHECK(tsp::tour_length(g, t) >= tsp::tour_length(g, *exact));
    }
}

void test_or_opt_moves_a_misplaced_city() {
    // Cities on a line 0..5; visiting 3 out of order costs extra.
    const auto g = tsp::parse_instance("6\n1 0 0\n2 1 0\n3 2 0\n4 3 0\n5 4 0\n6 5 0\n").graph;
    const tsp::Tour bad{0, 1, 3, 2, 4, 5};
    const auto fixed = tsp::or_opt(g, bad);
    CHECK_EQ(tsp::tour_length(g, fixed), 10);
}

void test_tiny_graphs() {
    for (int n = 1; n <= 3; ++n) {
        const auto g = random_graph(n, false, 7);
        CHECK(tsp::is_valid_tour(g, tsp::nearest_neighbour(g, 0)));
        CHECK(tsp::is_valid_tour(g, tsp::two_opt(g, tsp::nearest_neighbour(g, 0))));
        CHECK(tsp::is_valid_tour(g, tsp::two_opt_or_opt(g, tsp::nearest_neighbour(g, 0))));
        CHECK(tsp::is_valid_tour(g, *tsp::held_karp(g, n - 1)));
    }
}

}  // namespace

int main() {
    const std::vector<std::pair<const char*, std::function<void()>>> tests = {
        {"loads all sample files", test_loads_all_sample_files},
        {"rounded euclidean distance", test_rounded_euclidean_distance},
        {"tsplib format", test_tsplib_format},
        {"bad input is rejected", test_bad_input_is_rejected},
        {"all algorithms return valid tours", test_all_algorithms_return_valid_tours},
        {"2-opt untangles a square", test_two_opt_untangles_a_square},
        {"held-karp matches brute force", test_held_karp_matches_brute_force},
        {"held-karp start and limits", test_held_karp_start_and_limits},
        {"asymmetric 2-opt never gets worse", test_asymmetric_two_opt_never_gets_worse},
        {"or-opt never gets worse", test_or_opt_never_gets_worse},
        {"or-opt respects the optimum", test_or_opt_respects_the_optimum},
        {"or-opt moves a misplaced city", test_or_opt_moves_a_misplaced_city},
        {"tiny graphs", test_tiny_graphs},
    };
    for (const auto& [name, fn] : tests) {
        const int before = g_failures;
        try {
            fn();
        } catch (const std::exception& e) {
            ++g_failures;
            std::cerr << "unexpected exception: " << e.what() << '\n';
        }
        std::cout << (g_failures == before ? "[ OK ] " : "[FAIL] ") << name << '\n';
    }
    std::cout << '\n' << g_checks << " checks, " << g_failures << " failed\n";
    return g_failures == 0 ? 0 : 1;
}
