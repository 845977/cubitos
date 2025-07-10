#pragma once
/* region.h
 * Represents a region of the [0, 2^N)^n discrete space
 */

#include "point.h"

namespace cubitos {

class Region {
   public:
    Region();
    Region(uint8_t depth, std::vector<Point>::iterator begin,
           std::vector<Point>::iterator end);

    std::vector<Point>::iterator lastPoint();

    bool containsInDepth(const Point& p, uint8_t depth);

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out, const Region& r);
#endif  // DEBUG

   private:
    void subdivide();
    std::vector<Region>& getSubregions();
    // Returns whether p is in this region
    bool contains(const Point& p) const;

    uint8_t depth_;  // Not going to be greater than 64

    std::vector<Point>::iterator firstPoint_, endPoint_;

    std::vector<Region> subregions_;
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
