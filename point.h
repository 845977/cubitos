#pragma once
/* point.h
 * Defines a point in a ([0, 2^N))^n discrete space
 */

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

    Point();
    Point(std::vector<std::bitset<NUMBITS>> point);
    Point(std::vector<float> coors);

    // The order relationship gives preference to the first coordinates
    bool operator<(const Point& rhs) const;
    bool operator==(const Point& rhs) const;
    bool operator!=(const Point& rhs) const;

    Point truncate(uint8_t n) const;

    Point makeCenter(uint8_t depth) const;

    bool hasCenter(const Point& center, uint8_t depth) const;
    bool equalsTruncated(const Point& center, uint8_t depth) const;
    void directions(uint8_t depth, std::vector<int>& directions,
                    std::vector<int>& nondirections) const;
    uint8_t depthAsCenter() const;
};

#ifdef DEBUG
std::ostream& operator<<(std::ostream& out, const Point& p);
#endif  // DEBUG

}  // namespace cubitos
