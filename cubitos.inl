#include <algorithm>
#include <cassert>

namespace cubitos {

template <int _N>
Cubitos<_N>::Cubitos(std::vector<std::vector<float>> points) {
    // We want a sorted cloud of points, first we transform them from float
    std::vector<std::bitset<NUMBITS>> center;

    assert(!points.empty());

    spaceDimension_ = points.begin()->size();
    cloud_.push_back(Point(points[0]));

    for (auto point_it = points.begin() + 1; point_it != points.end();
         point_it++) {
        assert(point_it->size() == spaceDimension_);
        cloud_.push_back(Point(*point_it));
    }

    std::sort(cloud_.begin(), cloud_.end());

    // Points should be unique
    for (auto point_it = points.begin(); point_it != points.end() - 1;
         point_it++) {
        assert(*point_it != *(point_it + 1));
    }

    // Now we have confirmed that the cloud is well-formed, there shouldn't
    // be any more errors.

    for (size_t i = 0; i < spaceDimension_; i++) {
        center.push_back(BIGONE);
    }

    region_ = Region(0, cloud_.begin(), cloud_.end());

    CComplex init_complex(0, &region_);
    init_complex.add(CSimplex(center, 0, 0));
    // complexes_.push_back(init_complex);
    module_ = Module<_N>(init_complex);
}

template <int _N>
void Cubitos<_N>::computeComplex(uint8_t depth) {
    assert(depth < NUMBITS - 1);

    while (complexes_.size() <= depth) {
        complexes_.push_back(complexes_.back().expand());
    }
}

/* Debugging functions */
#ifdef DEBUG
template <int _N>
std::ostream& operator<<(std::ostream& out, const Cubitos<_N>& cub) {
    out << cub.module_;
    return out;
}
#endif  // DEBUG

}  // namespace cubitos
