#include "svg.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace tsp {

namespace {

std::string escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            default: out += c;
        }
    }
    return out;
}

}  // namespace

void write_svg(const std::string& path, const Graph& g, const std::vector<NamedTour>& tours,
               const std::string& title) {
    if (!g.has_coordinates()) {
        throw std::runtime_error("this instance has no coordinates (it is a distance matrix), "
                                 "so it cannot be drawn");
    }
    if (tours.empty()) throw std::runtime_error("nothing to draw");

    const auto& pts = g.points();
    double min_x = std::numeric_limits<double>::max(), max_x = std::numeric_limits<double>::lowest();
    double min_y = min_x, max_y = max_x;
    for (const auto& p : pts) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }
    const double span_x = std::max(max_x - min_x, 1e-9);
    const double span_y = std::max(max_y - min_y, 1e-9);
    const double span = std::max(span_x, span_y);

    // Each panel keeps the aspect ratio of the cities, longest side kPanel.
    constexpr double kPanel = 460.0;
    constexpr double kInset = 12.0;   // keeps dots off the panel border
    constexpr double kPad = 20.0;
    constexpr double kHeader = 64.0;  // title + panel captions
    const double draw_w = kPanel * span_x / span;
    const double draw_h = kPanel * span_y / span;
    const double panel_w = draw_w + 2 * kInset;
    const double panel_h = draw_h + 2 * kInset;
    const double width = static_cast<double>(tours.size()) * (panel_w + kPad) + kPad;
    const double height = kHeader + panel_h + kPad;
    const double radius = pts.size() > 300 ? 1.6 : 3.0;
    const double stroke = pts.size() > 300 ? 1.0 : 1.6;

    std::ofstream out(path);
    if (!out) throw std::runtime_error("cannot write '" + path + "'");

    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height
        << "\" viewBox=\"0 0 " << width << ' ' << height << "\" font-family=\"system-ui, sans-serif\">\n";
    out << "<rect width=\"100%\" height=\"100%\" fill=\"#fafaf9\"/>\n";
    out << "<text x=\"" << kPad << "\" y=\"28\" font-size=\"18\" font-weight=\"600\" fill=\"#1c1917\">"
        << escape(title) << "</text>\n";

    for (std::size_t k = 0; k < tours.size(); ++k) {
        const auto& nt = tours[k];
        const double ox = kPad + static_cast<double>(k) * (panel_w + kPad);
        const double oy = kHeader;
        auto sx = [&](double x) { return ox + kInset + (x - min_x) / span * kPanel; };
        // SVG y grows downwards; flip so the picture matches a normal plot.
        auto sy = [&](double y) { return oy + kInset + draw_h - (y - min_y) / span * kPanel; };

        out << "<text x=\"" << ox << "\" y=\"" << (oy - 10) << "\" font-size=\"14\" fill=\"#44403c\">"
            << escape(nt.name) << " &#8212; " << nt.length << "</text>\n";
        out << "<rect x=\"" << ox << "\" y=\"" << oy << "\" width=\"" << panel_w << "\" height=\"" << panel_h
            << "\" fill=\"white\" stroke=\"#e7e5e4\"/>\n";

        out << "<polygon fill=\"none\" stroke=\"#2563eb\" stroke-width=\"" << stroke
            << "\" stroke-linejoin=\"round\" points=\"";
        for (int city : nt.tour) {
            out << sx(pts[city].x) << ',' << sy(pts[city].y) << ' ';
        }
        out << "\"/>\n";

        for (std::size_t i = 0; i < pts.size(); ++i) {
            out << "<circle cx=\"" << sx(pts[i].x) << "\" cy=\"" << sy(pts[i].y) << "\" r=\"" << radius
                << "\" fill=\"#1c1917\"/>\n";
        }
        if (!nt.tour.empty()) {
            const auto& s = pts[nt.tour.front()];
            out << "<circle cx=\"" << sx(s.x) << "\" cy=\"" << sy(s.y) << "\" r=\"" << radius * 2.2
                << "\" fill=\"#dc2626\"><title>start</title></circle>\n";
        }
    }
    out << "</svg>\n";
}

}  // namespace tsp
