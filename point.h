#pragma once
// file: point.h
// description: Defines a point in a ([0, 2^NUMBITS))^n discrete space

#include <bitset>
#include <ostream>
#include <vector>

#include "config.h"

namespace cubitos {

// Encapsulating points complicates bit operations, so I define them as a
// struct
struct Point {
    /* Data */
    size_t dim_;
    std::vector<std::bitset<NUMBITS>> coors_;

    // Constructors
    Point();
    Point(std::vector<std::bitset<NUMBITS>> point);
    Point(std::vector<float> coors);

    // The order relationship gives preference to the first coordinates
    bool operator<(const Point& rhs) const;
    bool operator==(const Point& rhs) const;
    bool operator!=(const Point& rhs) const;

    // Truncates a point to the first n bits
    Point truncate(size_t n) const;

    bool equalsTruncated(const Point& center, size_t depth) const;
    // Computes directions and non-directions for the simplex (where the
    //  simplex can expand and where not).
    void directions(size_t depth, std::vector<int>& directions,
                    std::vector<int>& nondirections) const;
    // Returns the depth of this point as a center in the mesh
    size_t depthAsCenter() const;
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& out, const Point& p);
#endif  // DEBUG

}  // namespace cubitos
