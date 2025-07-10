#include "ccomplex.h"

#include <algorithm>
#include <cassert>

using namespace cubitos;

CComplex::CComplex() : dim_(0), depth_(0) {}
CComplex::CComplex(const size_t depth, Region* region)
    : dim_(0), depth_(depth), region_(region) {
    // We create the vector for the 0-simplices
    std::vector<CSimplex> vector;
    simplices_.push_back(vector);
}

void CComplex::add(const CSimplex& csimplex) {
    while (dim_ < csimplex.dim_) {
        std::vector<CSimplex> vector;
        simplices_.push_back(vector);
        dim_++;
    }
    simplices_[csimplex.dim_].push_back(csimplex);
}

size_t CComplex::numSimplicesIn(size_t dim) const {
    if (dim > dim_) {
        return 0;
    }
    return simplices_[dim].size();
}

CComplex CComplex::expand() const {
    CComplex expanded_complex(depth_ + 1, region_);
    for (size_t level = 0; level < simplices_.size(); level++) {
        std::map<CSimplex, size_t> temp_collapsing;
        std::map<size_t, size_t> final_collapsing;
        for (size_t i = 0; i < simplices_[level].size(); i++) {
            // For the ones of the same size, we save where they came from
            auto simplex = simplices_[level][i];
            for (auto exp : simplex.expansions(*region_)) {
                expanded_complex.add(exp);
                if (exp.dim_ == simplex.dim_) {
                    temp_collapsing[exp] = i;
                }
            }
        }
        // We only create the collapsing map if there are any simplices of the
        // same size
        if (level < expanded_complex.simplices_.size()) {
            std::sort(expanded_complex.simplices_[level].begin(),
                      expanded_complex.simplices_[level].end());
            for (auto& image : temp_collapsing) {
                size_t pos = std::distance(
                    expanded_complex.simplices_[level].begin(),
                    std::find(expanded_complex.simplices_[level].begin(),
                              expanded_complex.simplices_[level].end(),
                              image.first));
                final_collapsing[pos] = image.second;
            }
            expanded_complex.collapsingMaps_.push_back(final_collapsing);
        }
    }
    // Finally we sort the ones that weren't sorted
    for (size_t level = simplices_.size();
         level < expanded_complex.simplices_.size(); level++) {
        std::sort(expanded_complex.simplices_[level].begin(),
                  expanded_complex.simplices_[level].end());
    }
    return expanded_complex;
}

std::map<std::pair<size_t, size_t>, int> CComplex::getDifferentialMap(
    size_t dim) const {
    assert(dim > 0);

    std::map<std::pair<size_t, size_t>, int> diffMap;

    for (size_t i = 0; i < simplices_[dim].size(); i++) {
        for (auto& item : simplices_[dim][i].differential().simplices) {
            for (size_t j = 0; j < simplices_[dim - 1].size(); j++) {
                if (item.first == simplices_[dim - 1][j]) {
                    diffMap[{i, j}] = item.second;
                }
            }
        }
    }

    return diffMap;
}

const std::map<size_t, size_t>& CComplex::getCollapsingMap(size_t dim) const {
    static std::map<size_t, size_t> empty_map;
    if (collapsingMaps_.size() < dim + 1) {
        return empty_map;
    }
    return collapsingMaps_[dim];
}

#ifdef DEBUG
std::ostream& cubitos::operator<<(std::ostream& out, const CComplex& complex) {
    out << "CComplex of dim = " << complex.dim_ << " and depth "
        << complex.depth_ << " with simplexes: [" << std::endl;
    for (auto x : complex.simplices_) {
        out << "[";
        for (auto y : x) {
            out << y << std::endl;
        }
        out << "]" << std::endl;
    }
    return out;
}
#endif  // DEBUG
