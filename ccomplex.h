#pragma once
// file: ccomplex.h
// description: class representing a cubical complex equipped with an expand
//              function to generate another complex of greater depth

#include <algorithm>
#include <vector>

#include "config.h"
#include "csimplex.h"

namespace cubitos {

class CComplex {
   public:
    // Empty constructor
    CComplex();
    // Creates a cubical complex of <depth> depth and links it to <region>
    // Pre: <region> points to a valid memory address
    CComplex(const size_t depth, Region* region);

    // Adds a cubical simplex to the complex
    void add(const CSimplex& csimplex);

    // Returns the number of simplices in each dimension (0 if empty)
    size_t numSimplicesIn(size_t dim) const;

    // Returns an expanded complex of depth+1
    CComplex expand() const;

    // Returns the dim-boundary matrix as a map
    //     key: (i,j), value: 1 or -1
    const std::map<std::pair<size_t, size_t>, int> getDifferentialMap(
        size_t dim) const;

    // Returns the dim-collapsing matrix as a map
    //     key: (i), value: j and a 1 value is assumed for each existing
    //         pair
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
