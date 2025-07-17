#pragma once
// file: region.h
// description: represents a region of the [0, 2^N)^n discrete space.

#include "point.h"

namespace cubitos {

class Region {
   public:
    // Constructors
    Region();
    Region(size_t depth, std::vector<Point>::iterator begin,
           std::vector<Point>::iterator end);

    // Boolean function used to determine whether there a point is in subregion
    bool containsInDepth(const Point& p, size_t depth);

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out, const Region& r);
#endif  // DEBUG

   private:
    inline void subdivide();
    std::vector<Region>& getSubregions();
    // Returns whether p is in this region
    bool contains(const Point& p) const;

    size_t depth_;

    // First and last points contained in this region
    std::vector<Point>::iterator firstPoint_, endPoint_;

    std::vector<Region> subregions_;
    // The corner is stored instead of the center to reduce bit shifting
    // operations
    Point corner_;

    // Degenerate regions contain only one point and do not subdivide.
    // Instead, we store the only point they contain.
    bool isDegenerate_;
    Point degenerateCenter_;
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& out, const Region& r);
#endif  // DEBUG

}  // namespace cubitos
