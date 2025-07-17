#pragma once
/*
 * file: csimplex.h
 * description: class representing a cubical complex equipped with an
 *      expand and checkSimplex operation
 */

#include <map>

#include "point.h"
#include "region.h"

namespace cubitos {

struct CChain;

class CSimplex {
   public:
    // Constructors
    CSimplex() {};
    CSimplex(Point center, size_t depth, size_t dim);

    // Returns all possible simplex expansions
    std::vector<CSimplex> expansions(Region& region) const;
    // Returns the image of the boundary map
    CChain differential() const;

    // Order relationship for std::map. Doesn't have any real meaning.
    bool operator<(const CSimplex& rhs) const;
    bool operator==(const CSimplex& rhs) const;

    size_t dim_;

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out,
                                    const CSimplex& csimplex);
    Point get_center() const { return center_; };  // for plotting
#endif                                             // DEBUG

   private:
    void expansionsRec(Region& region, std::vector<int>::const_iterator it,
                       std::vector<CSimplex>& expansions,
                       std::vector<std::bitset<NUMBITS>>& coors,
                       size_t dim) const;

    bool checkSimplex(Region& region) const;
    bool checkSimplexRecDir(Region& region,
                            std::vector<int>::const_iterator it,
                            std::vector<std::bitset<NUMBITS>>& coors,
                            std::bitset<NUMBITS> offset, int k) const;

    bool checkSimplexRecNonDir(Region& region,
                               const std::vector<int>::const_iterator it,
                               std::vector<std::bitset<NUMBITS>>& coors,
                               std::bitset<NUMBITS> offset, int k) const;

    Point center_;
    size_t depth_;
    std::vector<int> directions_, nondirections_;
    std::vector<int> simplices;
};

// Element in a chain complex
struct CChain {
    CChain();
    CChain& operator+=(CSimplex csimplex);
    CChain& operator-=(CSimplex csimplex);

    std::map<CSimplex, int> simplices;
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& out, const CSimplex& csimplex);
std::ostream& operator<<(std::ostream& out, const CChain& cchain);

#endif  // DEBUG

}  // namespace cubitos
