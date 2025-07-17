#include "region.h"

using namespace cubitos;

Region::Region() : isDegenerate_(false) {}

// Pre: begin matches an existing point
Region::Region(size_t depth, std::vector<Point>::iterator begin,
               std::vector<Point>::iterator end)
    : depth_(depth), firstPoint_(begin), isDegenerate_(false) {
    corner_ = firstPoint_->truncate(depth_);
    for (endPoint_ = firstPoint_ + 1; endPoint_ != end && contains(*endPoint_);
         endPoint_++);
    if (firstPoint_ == (endPoint_ - 1)) {
        // This region only contains one point, there is no reason to
        // subdivide any longer.
        isDegenerate_ = true;
        degenerateCenter_ = *firstPoint_;
    }
}

bool Region::contains(const Point& p) const {
    return p.equalsTruncated(corner_, depth_);
}

// Pre: p is in this region
bool Region::containsInDepth(const Point& p, size_t depth) {
    if (depth == depth_) {
        return true;
    }
    if (isDegenerate_) {
        return p.equalsTruncated(degenerateCenter_.truncate(depth), depth);
    }
    for (auto x : getSubregions()) {
        if (x.contains(p)) {
            return x.containsInDepth(p, depth);
        }
    }
    return false;
}

std::vector<Region>& Region::getSubregions() {
    if (subregions_.size() == 0) {
        subdivide();
    }
    return subregions_;
}

void Region::subdivide() {
    auto it = firstPoint_;
    while (it != endPoint_) {
        auto r = Region(depth_ + 1, it, endPoint_);
        subregions_.push_back(r);
        it = r.endPoint_;
    }
}

#ifdef DEBUG
std::ostream& cubitos::operator<<(std::ostream& out, const Region& r) {
    std::string tab = "";
    for (size_t i = 0; i < r.depth_; i++) {
        tab += "  ";
    }
    out << tab << "Region {";
    for (auto x = r.firstPoint_; x != r.endPoint_ - 1; x++) {
        out << *x << ", ";
    }
    out << *(r.endPoint_ - 1);

    for (auto x : r.subregions_) {
        out << std::endl << x;
    }
    out << "}";

    return out;
}
#endif  // DEBUG
