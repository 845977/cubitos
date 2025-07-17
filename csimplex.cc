// file: csimplex.cc

#include "csimplex.h"

#include "bitset_operators.h"

using namespace cubitos;

CSimplex::CSimplex(Point center, size_t depth, size_t dim)
    : dim_(dim), center_(center), depth_(depth) {
    center_.directions(depth_, directions_, nondirections_);
}

std::vector<CSimplex> CSimplex::expansions(Region& region) const {
    std::vector<CSimplex> expansions;
    auto coors = center_.coors_;

    expansionsRec(region, nondirections_.begin(), expansions, coors,
                  center_.dim_);

    return expansions;
}

void CSimplex::expansionsRec(Region& region,
                             std::vector<int>::const_iterator it,
                             std::vector<CSimplex>& expansions,
                             std::vector<std::bitset<NUMBITS>>& coors,
                             size_t dim) const {
    if (it == nondirections_.end() || dim == dim_) {
        // We've got a combination, deep copy and push
        CSimplex possible_simplex(Point(coors), depth_ + 1, dim);
        if (possible_simplex.checkSimplex(region)) {
            expansions.push_back(possible_simplex);
        }
    } else {
        expansionsRec(region, it + 1, expansions, coors, dim);
        std::bitset<NUMBITS> previousValue = coors[*it];
        coors[*it] -= (BIGONE >> (depth_ + 1));
        expansionsRec(region, it + 1, expansions, coors, dim - 1);
        coors[*it] = previousValue + (BIGONE >> (depth_ + 1));
        expansionsRec(region, it + 1, expansions, coors, dim - 1);
        coors[*it] = previousValue;
    }
}

bool CSimplex::checkSimplex(Region& region) const {
    std::bitset<NUMBITS> offset = BIGONE >> (depth_ + 1);
    auto coors = center_.coors_;

    return checkSimplexRecDir(region, directions_.begin(), coors, offset,
                              directions_.size());
}

bool CSimplex::checkSimplexRecDir(Region& region,
                                  std::vector<int>::const_iterator it,
                                  std::vector<std::bitset<NUMBITS>>& coors,
                                  std::bitset<NUMBITS> offset, int k) const {
    if (k == 0) {
        return checkSimplexRecNonDir(region, nondirections_.begin(), coors,
                                     offset, nondirections_.size());
    } else {
        std::bitset<NUMBITS> previousValue = coors[*it];
        coors[*it] += offset;
        bool isPlus = checkSimplexRecDir(region, it + 1, coors, offset, k - 1);
        coors[*it] = previousValue - offset;
        bool isMinus =
            checkSimplexRecDir(region, it + 1, coors, offset, k - 1);
        coors[*it] = previousValue;
        if (!(isPlus && isMinus)) {
            return false;
        }
    }
    return true;
}

bool CSimplex::checkSimplexRecNonDir(Region& region,
                                     std::vector<int>::const_iterator it,
                                     std::vector<std::bitset<NUMBITS>>& coors,
                                     std::bitset<NUMBITS> offset,
                                     int k) const {
    if (k == 0) {
        return region.containsInDepth(Point(coors),
                                      Point(coors).depthAsCenter());
    } else {
        std::bitset<NUMBITS> previousValue = coors[*it];
        coors[*it] += offset;
        if (checkSimplexRecNonDir(region, it + 1, coors, offset, k - 1)) {
            coors[*it] = previousValue;
            return true;
        }
        coors[*it] = previousValue - offset;
        if (checkSimplexRecNonDir(region, it + 1, coors, offset, k - 1)) {
            coors[*it] = previousValue;
            return true;
        }
        coors[*it] = previousValue;
    }
    return false;
}

CChain CSimplex::differential() const {
    CChain chain = CChain();
    if (dim_ == 0) return chain;

    std::bitset<NUMBITS> shift = BIGONE >> depth_;
    bool rotate = false;
    for (auto d : directions_) {
        Point a = center_, b = center_;
        a.coors_[d] += shift;
        b.coors_[d] -= shift;
        if (!rotate) {
            chain += CSimplex(a, depth_, dim_ - 1);
            chain -= CSimplex(b, depth_, dim_ - 1);
        } else {
            chain += CSimplex(b, depth_, dim_ - 1);
            chain -= CSimplex(a, depth_, dim_ - 1);
        }
        rotate = !rotate;
    }

    return chain;
}

bool CSimplex::operator<(const CSimplex& rhs) const {
    if (dim_ < rhs.dim_) {
        return true;
    }
    if (dim_ > rhs.dim_) {
        return false;
    }
    if (depth_ < rhs.depth_) {
        return true;
    }
    if (depth_ > rhs.depth_) {
        return false;
    }
    return center_ < rhs.center_;
}

bool CSimplex::operator==(const CSimplex& rhs) const {
    if (dim_ != rhs.dim_) {
        return false;
    }
    if (depth_ != rhs.depth_) {
        return false;
    }
    return center_ == rhs.center_;
}

CChain::CChain() {};

CChain& CChain::operator+=(CSimplex csimplex) {
    simplices[csimplex] += 1;
    return *this;
}

CChain& CChain::operator-=(CSimplex csimplex) {
    simplices[csimplex] -= 1;
    return *this;
}

#ifdef DEBUG
std::ostream& cubitos::operator<<(std::ostream& out,
                                  const CSimplex& csimplex) {
    out << "<CSimplex dim=" << csimplex.dim_
        << ", depth=" << (int)csimplex.depth_ << " and center "
        << csimplex.center_ << ">";

    return out;
}

std::ostream& cubitos::operator<<(std::ostream& out, const CChain& cchain) {
    for (auto s : cchain.simplices) {
        out << s.first << ' ' << s.second << ", ";
    }

    return out;
}
#endif  // DEBUG
