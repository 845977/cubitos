#include "point.h"
// file: point.cc

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

Point Point::truncate(size_t n) const {
    std::vector<std::bitset<NUMBITS>> coors;
    for (auto x : coors_) {
        coors.push_back(x >> (NUMBITS - n) << (NUMBITS - n));
    }
    return Point(coors);
}

bool Point::equalsTruncated(const Point& rhs, size_t depth) const {
    auto x = coors_.begin();
    auto y = rhs.coors_.begin();
    size_t shift = NUMBITS - depth;
    while (x != coors_.end()) {
        if ((*x >> shift << shift) != *y) {
            return false;
        }
        x++;
        y++;
    }
    return true;
}

void Point::directions(size_t depth, std::vector<int>& directions,
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
size_t Point::depthAsCenter() const {
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
