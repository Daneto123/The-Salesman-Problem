// Travelling salesman problem: nearest neighbour, repeated nearest neighbour,
// 2-opt and an exact solver for small graphs.
//
// Originally written for the Data Structures course, winter semester
// 2020/2021. The original university version lives on the
// `university-2020` branch.

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms.hpp"
#include "io.hpp"
#include "svg.hpp"

namespace {

struct Options {
    std::string file;
    int start = 0;
    std::string algo = "all";
    tsp::InputType type = tsp::InputType::Auto;
    std::string out_file;
    std::string svg_file;
};

void print_usage(std::ostream& os) {
    os << "Usage: tsp <file> [options]\n"
          "\n"
          "Options:\n"
          "  -s, --start N     starting city, 0-based (default 0)\n"
          "  -a, --algo NAME   nn | rnn | 2opt | exact | all (default all)\n"
          "  -t, --type TYPE   auto | matrix | points (default auto)\n"
          "  -o, --out FILE    save the tours to FILE\n"
          "      --svg FILE    draw the tours as an SVG picture (points files only)\n"
          "  -h, --help        show this help\n"
          "\n"
          "Algorithms:\n"
          "  nn     nearest neighbour from the start city\n"
          "  rnn    repeated nearest neighbour (best start over all cities)\n"
          "  2opt   repeated nearest neighbour improved with 2-opt\n"
          "  exact  Held-Karp optimum, up to "
       << tsp::kHeldKarpMaxCities
       << " cities\n"
          "\n"
          "Run without arguments for interactive mode.\n";
}

tsp::InputType parse_type(const std::string& s) {
    if (s == "auto") return tsp::InputType::Auto;
    if (s == "matrix") return tsp::InputType::Matrix;
    if (s == "points") return tsp::InputType::Points;
    throw std::invalid_argument("unknown type '" + s + "' (use auto, matrix or points)");
}

int parse_int(const std::string& s, const std::string& what) {
    try {
        std::size_t used = 0;
        const int value = std::stoi(s, &used);
        if (used == s.size()) return value;
    } catch (const std::exception&) {
    }
    throw std::invalid_argument(what + " must be a whole number, got '" + s + "'");
}

Options parse_args(int argc, char** argv) {
    Options opt;
    std::vector<std::string> args(argv + 1, argv + argc);
    for (std::size_t i = 0; i < args.size(); ++i) {
        const std::string& a = args[i];
        auto value = [&]() -> const std::string& {
            if (i + 1 >= args.size()) throw std::invalid_argument(a + " needs a value");
            return args[++i];
        };
        if (a == "-h" || a == "--help") {
            print_usage(std::cout);
            std::exit(0);
        } else if (a == "-s" || a == "--start") {
            opt.start = parse_int(value(), "start");
        } else if (a == "-a" || a == "--algo") {
            opt.algo = value();
        } else if (a == "-t" || a == "--type") {
            opt.type = parse_type(value());
        } else if (a == "-o" || a == "--out") {
            opt.out_file = value();
        } else if (a == "--svg") {
            opt.svg_file = value();
        } else if (!a.empty() && a[0] == '-') {
            throw std::invalid_argument("unknown option '" + a + "'");
        } else if (opt.file.empty()) {
            opt.file = a;
        } else {
            throw std::invalid_argument("only one input file is supported");
        }
    }
    if (opt.algo != "nn" && opt.algo != "rnn" && opt.algo != "2opt" && opt.algo != "exact" &&
        opt.algo != "all") {
        throw std::invalid_argument("unknown algorithm '" + opt.algo + "'");
    }
    if (opt.file.empty()) throw std::invalid_argument("no input file given");
    return opt;
}

Options ask_interactively() {
    Options opt;
    std::cout << "hello user\n";
    std::cout << "name of file: ";
    std::getline(std::cin, opt.file);
    std::cout << "type of file (auto/matrix/points) [auto]: ";
    std::string type;
    std::getline(std::cin, type);
    if (!type.empty()) opt.type = parse_type(type);
    std::cout << "from what point do you want to start [0]: ";
    std::string start;
    std::getline(std::cin, start);
    if (!start.empty()) opt.start = parse_int(start, "start");
    return opt;
}

template <typename F>
tsp::NamedTour timed(const std::string& name, const tsp::Graph& g, F&& run, double& ms) {
    const auto t0 = std::chrono::steady_clock::now();
    tsp::Tour tour = run();
    const auto t1 = std::chrono::steady_clock::now();
    ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return {name, tour, tsp::tour_length(g, tour)};
}

int run(const Options& opt) {
    const tsp::Instance inst = tsp::load_instance(opt.file, opt.type);
    const tsp::Graph& g = inst.graph;
    for (const auto& w : inst.warnings) std::cerr << "warning: " << w << '\n';

    if (opt.start < 0 || opt.start >= g.size()) {
        throw std::invalid_argument("start must be between 0 and " + std::to_string(g.size() - 1));
    }

    const bool symmetric = g.is_symmetric();
    std::cout << "Instance: " << opt.file << "  (" << g.size() << " cities, " << tsp::to_string(inst.type)
              << ", " << (symmetric ? "symmetric" : "asymmetric") << ")\n\n";

    const bool all = opt.algo == "all";
    std::vector<tsp::NamedTour> results;
    std::vector<double> times;
    double ms = 0;

    if (all || opt.algo == "nn") {
        results.push_back(timed("Nearest neighbour", g, [&] { return tsp::nearest_neighbour(g, opt.start); }, ms));
        times.push_back(ms);
    }
    std::optional<tsp::Tour> rnn;
    if (all || opt.algo == "rnn" || opt.algo == "2opt") {
        auto r = timed("Repeated NN", g, [&] { return tsp::repeated_nearest_neighbour(g, opt.start); }, ms);
        rnn = r.tour;
        if (all || opt.algo == "rnn") {
            results.push_back(r);
            times.push_back(ms);
        }
    }
    if (all || opt.algo == "2opt") {
        results.push_back(timed("2-opt", g, [&] { return tsp::two_opt(g, *rnn); }, ms));
        times.push_back(ms);
    }
    if (all || opt.algo == "exact") {
        if (g.size() <= tsp::kHeldKarpMaxCities) {
            results.push_back(timed("Exact (Held-Karp)", g, [&] { return *tsp::held_karp(g, opt.start); }, ms));
            times.push_back(ms);
        } else if (!all) {
            throw std::invalid_argument("exact solver supports at most " +
                                        std::to_string(tsp::kHeldKarpMaxCities) + " cities");
        }
    }

    std::size_t best = 0;
    std::cout << std::left << std::setw(22) << "Algorithm" << std::right << std::setw(12) << "Length"
              << std::setw(14) << "Time (ms)" << '\n';
    for (std::size_t i = 0; i < results.size(); ++i) {
        std::cout << std::left << std::setw(22) << results[i].name << std::right << std::setw(12)
                  << results[i].length << std::setw(14) << std::fixed << std::setprecision(2) << times[i]
                  << '\n';
        if (results[i].length < results[best].length) best = i;
    }
    std::cout << "\nShortest tour: " << results[best].length << " (" << results[best].name << ")\n";

    if (!opt.out_file.empty()) {
        std::ofstream out(opt.out_file);
        if (!out) throw std::runtime_error("cannot write '" + opt.out_file + "'");
        out << "# " << opt.file << '\n';
        for (const auto& r : results) {
            out << r.name << " (" << r.length << "):";
            for (int c : r.tour) out << ' ' << c;
            out << ' ' << r.tour.front() << '\n';
        }
        std::cout << "Tours saved to " << opt.out_file << '\n';
    }

    if (!opt.svg_file.empty()) {
        const auto slash = opt.file.find_last_of("/\\");
        const std::string name = slash == std::string::npos ? opt.file : opt.file.substr(slash + 1);
        tsp::write_svg(opt.svg_file, g, results, name + " (" + std::to_string(g.size()) + " cities)");
        std::cout << "Picture saved to " << opt.svg_file << '\n';
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options opt = argc > 1 ? parse_args(argc, argv) : ask_interactively();
        return run(opt);
    } catch (const std::invalid_argument& e) {
        std::cerr << "error: " << e.what() << "\n\n";
        print_usage(std::cerr);
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
