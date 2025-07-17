#pragma once
// file: cubitos.h
// description: main class of our program. Loads a point cloud and
//      computes cubical persistent homology on it

#include "ccomplex.h"
#include "csimplex.h"
#include "module.h"
#include "region.h"
#include "smatrix.h"

namespace cubitos {

// _N is the prime for the finite field
template <int _N>
class Cubitos {
   public:
    // A persistentor class is related to  a cloud of points
    // Pre: points is not empty
    Cubitos(std::vector<std::vector<float>> points) {
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
        module_ = Module<_N>(init_complex);
    }

    // Computes the cubical complex up to <depth> depth
    void addToLevel(size_t depth) { module_.addToLevel(depth); }

    // Returns the computed barcode
    Barcode barcode() { return module_.computeBarcode(); }

// Debugging functions
#ifdef DEBUG
    template <int _M>
    friend std::ostream& operator<<(std::ostream& out, const Cubitos<_M>& p);
#endif  // DEBUG

   private:
    std::vector<Point> cloud_;

    std::vector<CComplex> complexes_;
    Module<_N> module_;
    Region region_;

    size_t spaceDimension_;
};

template <int _N>
std::ostream& operator<<(std::ostream& out, const Cubitos<_N>& p);

/* Debugging functions */
#ifdef DEBUG
template <int _N>
std::ostream& operator<<(std::ostream& out, const Cubitos<_N>& cub) {
    out << cub.module_;
    return out;
}
#endif  // DEBUG

}  // namespace cubitos
