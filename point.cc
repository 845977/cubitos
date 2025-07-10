#include "point.h"

using namespace cubitos;

Point::Point() {}

Point::Point(std::vector<std::bitset<NUMBITS>> coors) : coors_(coors) {
    dim_ = coors_.size();
}

Point::Point(std::vector<float> coors) {
    for (auto x : coors) {
        coors_.push_back(
            std::bitset<NUMBITS>((x * (float)ALLONES.to_ullong())));
    }
    dim_ = coors_.size();
}

bool Point::operator<(const Point& rp) const {
    for (ssize_t j = NUMBITS - 1; j >= 0; j--) {
        for (size_t i = 0; i < coors_.size(); i++) {
            if ((coors_[i][j] == 0) && (rp.coors_[i][j] == 1)) {
                return true;
            } else if ((coors_[i][j] == 1) && (rp.coors_[i][j] == 0)) {
                return false;
            }
        }
    }
    return false;
}

bool Point::operator==(const Point& rhs) const {
    if (dim_ != rhs.dim_) {
        return false;
    }
    for (size_t i = 0; i < dim_; i++) {
        if (coors_[i] != rhs.coors_[i]) {
            return false;
        }
    }
    return true;
}

bool Point::operator!=(const Point& rhs) const { return !(*this == rhs); }

Point Point::truncate(uint8_t n) const {
    ;
    std::vector<std::bitset<NUMBITS>> coors;
    for (auto x : coors_) {
        coors.push_back(x >> (NUMBITS - n) << (NUMBITS - n));
    }
    return Point(coors);
}

Point Point::makeCenter(uint8_t depth) const {
    std::vector<std::bitset<NUMBITS>> coors;
    uint8_t shift = NUMBITS - depth;
    std::bitset<NUMBITS> half = SMALLONE << (shift - 1);
    for (auto x : coors_) {
        coors.push_back(x >> shift << shift | half);
    }
    return Point(coors);
}

bool Point::hasCenter(const Point& center, uint8_t depth) const {
    auto x = coors_.begin();
    auto y = center.coors_.begin();
    uint8_t shift = NUMBITS - depth;
    while (x != coors_.end()) {
        if ((*x >> shift << shift) != (*y >> shift << shift)) {
            return false;
        }
        x++;
        y++;
    }
    return true;
}

bool Point::equalsTruncated(const Point& rhs, uint8_t depth) const {
    auto x = coors_.begin();
    auto y = rhs.coors_.begin();
    uint8_t shift = NUMBITS - depth;
    while (x != coors_.end()) {
        if ((*x >> shift << shift) != *y) {
            return false;
        }
        x++;
        y++;
    }
    return true;
}

void Point::directions(uint8_t depth, std::vector<int>& directions,
                       std::vector<int>& nondirections) const {
    std::bitset<NUMBITS> oneOne = (BIGONE >> depth);
    for (size_t i = 0; i < coors_.size(); i++) {
        if ((coors_[i] & oneOne) == 0) {
            directions.push_back(i);
        } else {
            nondirections.push_back(i);
        }
    }
}

// Pre: It's a center
uint8_t Point::depthAsCenter() const {
    for (int i = 0; i < NUMBITS; i++) {
        bool centerFound = true;
        for (auto coor : coors_) {
            if ((coor & (SMALLONE << i)) == 0) {
                centerFound = false;
                break;
            }
        }
        if (centerFound) {
            return NUMBITS - i - 1;
        }
    }
    return 0;
}

#ifdef DEBUG
std::ostream& cubitos::operator<<(std::ostream& out, const Point& p) {
    out << "[";
    auto c = p.coors_.begin();
    out << *c;
    for (c++; c != p.coors_.end(); c++) {
        out << ", " << *c;
    }
    out << "]";

    return out;
}
#endif  // DEBUG
