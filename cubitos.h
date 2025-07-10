#pragma once
/* persistentor.h
 * A persistentor class is useful for loading and storing the whole cloud of
 * points, so that Region only works with iterators.
 */

#include "ccomplex.h"
#include "csimplex.h"
#include "module.h"
#include "region.h"
#include "smatrix.h"

namespace cubitos {

template <int _N>
class Cubitos {
   public:
    /*
     * Used to dynamically compute the cubical complex
     */
    void computeComplex(uint8_t depth);

    void addToLevel(uint8_t depth) { module_.addToLevel(depth); }

    Barcode barcode() { return module_.computeBarcode(); }

    /*
     * A persistentor class is related to  a cloud of points
     * Pre: points is not empty
     */
    Cubitos(std::vector<std::vector<float>> points);

/*
 * Debugging functions
 */
#ifdef DEBUG
    template <int _M>
    friend std::ostream& operator<<(std::ostream& out, const Cubitos<_M>& p);
#endif  // DEBUG

   private:
    /*
     * The whole cloud of points
     */
    std::vector<Point> cloud_;

    // All the simplicial complexes
    std::vector<CComplex> complexes_;
    Module<_N> module_;
    Region region_;

    size_t spaceDimension_;
};

template <int _N>
std::ostream& operator<<(std::ostream& out, const Cubitos<_N>& p);

}  // namespace cubitos

#include "cubitos.inl"
