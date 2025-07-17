#include "barcode.h"
// file: barcode.cc

#include <cassert>
#include <iomanip>
#include <sstream>

using namespace cubitos;

Barcode::Barcode() : bars_(1) {}

void Barcode::add(size_t dim, size_t i, size_t j, size_t numBars) {
    // We reserve space for bars on demand
    while (bars_.size() <= dim) {
        std::map<std::pair<size_t, size_t>, size_t> bar;
        bars_.push_back(bar);
    }

    bars_[dim][{i, j}] += numBars;
}

std::string Barcode::tikzbarcode() const {
    assert(bars_.size() <= 4);
    std::stringstream ret;

    static std::string colors[] = {"red", "blue", "green", "orange"};

    ret << "\\begin{tikzpicture}" << std::endl;

    double height = 0.4;

    for (size_t dim = 0; dim < bars_.size(); dim++) {
        for (const auto& bar : bars_[dim]) {
            for (size_t i = 0; i < bar.second; i++) {
                if (bar.first.second == 0) {
                    ret << "    \\draw[" << colors[dim] << "] ("
                        << bar.first.first << "," << height << ")"
                        << " -- (" << (double)bar.first.second - 0.1 << ","
                        << height << ");" << std::endl;
                } else {
                    ret << "    \\draw[-stealth," << colors[dim] << "] ("
                        << bar.first.first << "," << height << ")"
                        << " -- (" << bar.first.second - 1 << "," << height
                        << ");" << std::endl;
                }
                height += 0.1;
            }
        }
    }

    size_t maxLen = 0;
    for (const auto& bar : bars_[0]) {
        maxLen = std::max(maxLen, bar.first.first);
    }

    ret << "    \\draw (-0.1,0) (" << maxLen << ",0);" << std::endl;

    for (size_t d = 0; d < maxLen; d++) {
        ret << "    \\draw[dashed, lightgray] (" << d << ", 0) -- (" << d
            << "," << height << ");" << std::endl
            << "    \\node at (" << d << ",0) [below] {$" << d << "$};"
            << std::endl;
    }

    height = -1.0;
    for (size_t dim = 0; dim < bars_.size(); dim++) {
        ret << std::fixed << std::setprecision(2)
            << "    \\draw[fill=" << colors[dim] << "] (0," << height
            << ") rectangle (0.5," << height - 0.4 << ");" << std::endl
            << "    \\node at (1," << height - 0.2 << ") {$H_" << dim << "$};"
            << std::endl;
        height -= 0.4;
    }

    ret << "\\end{tikzpicture}" << std::endl;

    return ret.str();
}

std::ostream& cubitos::operator<<(std::ostream& out, const Barcode& bcode) {
    for (size_t dim = 0; dim < bcode.bars_.size(); dim++) {
        out << "Bars of dimension " << dim << ':' << std::endl;
        for (const auto& bar : bcode.bars_[dim]) {
            for (size_t i = 0; i < bar.second; i++) {
                if (bar.first.second == 0) {
                    out << '[' << bar.first.first << ", " << bar.first.second
                        << ']' << std::endl;
                } else {
                    out << '[' << bar.first.first << ", "
                        << bar.first.second - 1 << ')' << std::endl;
                }
            }
        }
    }
    return out;
}
