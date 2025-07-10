#pragma once

#include <algorithm>
#include <vector>

#include "config.h"
#include "csimplex.h"

namespace cubitos {

class CComplex {
   public:
    CComplex();
    CComplex(const size_t depth, Region* region);

    void add(const CSimplex& csimplex);
    size_t numSimplicesIn(size_t dim) const;
    CComplex expand() const;
    std::map<std::pair<size_t, size_t>, int> getDifferentialMap(
        size_t dim) const;
    const std::map<size_t, size_t>& getCollapsingMap(size_t dim) const;

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out,
                                    const CComplex& complex);
#endif  // DEBUG

    size_t dim_;
    size_t depth_;

   private:
    std::vector<std::vector<CSimplex>> simplices_;
    std::vector<std::map<size_t, size_t>> collapsingMaps_;
    Region* region_;
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& out, const CComplex& complex);
#endif  // DEBUG

}  // namespace cubitos
