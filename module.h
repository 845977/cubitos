#pragma once
// file: module.h
// description: Code for a persistent module

#include <cassert>
#include <vector>

#include "algorithms/reductions.h"
#include "barcode.h"
#include "ccomplex.h"
#include "smatrix.h"

namespace cubitos {

template <size_t _N>
class Module {
   public:
    Module() {}
    Module(const CComplex& complex) : lastComplex_(complex), maxDim_(0) {
        Dim dim = {// No basis changes for collapses
                   .R = SMatrix<_N>::identity(1),
                   .R_inv = SMatrix<_N>::identity(1),
                   // The trivial empty collapse map
                   .inducedMap = SMatrix<_N>(1, 1),
                   .firstHomologyIndex = 0};
        Depth depth = {{dim}};
        depths_.push_back(depth);
    }

    // Returns the boundary matrix d_dim
    SMatrix<_N> diffMat(const CComplex& complex, size_t dim) const {
        assert(dim > 0);
        SMatrix<_N> mat(complex.numSimplicesIn(dim - 1),
                        complex.numSimplicesIn(dim));

        for (auto& elm : complex.getDifferentialMap(dim)) {
            mat.insert(elm.first.second, elm.first.first, elm.second);
        }

        return mat;
    }

    // Returns the boundary matrix M_dim: domain -> image
    SMatrix<_N> getCollapsingMatrix(const CComplex& domain,
                                    const CComplex& image, size_t dim) const {
        assert(dim >= 0);

        if (domain.numSimplicesIn(dim) == 0) {
            return SMatrix<_N>::zeroMatrix();
        }
        size_t imageSize = image.numSimplicesIn(dim);
        imageSize = (imageSize > 0) ? imageSize : 1;

        SMatrix<_N> mat(imageSize, domain.numSimplicesIn(dim));
        for (const auto& elm : domain.getCollapsingMap(dim)) {
            mat.insert(elm.second, elm.first, 1);
        }

        return mat;
    }

    // Safe accessor for induced maps
    inline SMatrix<_N> getInducedMap(size_t depth, size_t dim) {
        if (depth >= depths_.size() ||
            dim >= depths_[depth].dimensions.size()) {
            return SMatrix<_N>::zeroMatrix();
        }
        return depths_[depth].dimensions[dim].inducedMap;
    }

    inline void addToLevel(size_t depth) {
        while (depths_.size() <= depth) {
            addLevel();
        }
    }

    // Expands the module to a greater depth using Algorithm 1
    void addLevel() {
        Dim dimension;
        Depth currentDepth;

        CComplex prevComplex = lastComplex_;
        lastComplex_ = lastComplex_.expand();
        maxDim_ = std::max(maxDim_, lastComplex_.dim_);
        auto& complex = lastComplex_;

        size_t firstHomologyIndex;

        SMatrix<_N> A, B, R, R_inv;

        if (complex.dim_ > 0) {
            A = diffMat(complex, 1);
            rowReduce(A, R, R_inv, firstHomologyIndex);
            dimension = {.R = R_inv,
                         .R_inv = R,
                         .firstHomologyIndex = firstHomologyIndex};
            currentDepth.dimensions.push_back(dimension);

            B = A;
            for (size_t dim = 1; dim < complex.dim_; dim++) {
                A = diffMat(complex, dim + 1);
                simultaneousReduce(B, A, R, R_inv, firstHomologyIndex);

                dimension = {.R = R,
                             .R_inv = R_inv,
                             .firstHomologyIndex = firstHomologyIndex};
                currentDepth.dimensions.push_back(dimension);
                B = A;
            }

            B = diffMat(complex, complex.dim_);
            columnReduce(B, R, R_inv, firstHomologyIndex);
            dimension = {.R = R,
                         .R_inv = R_inv,
                         .firstHomologyIndex = firstHomologyIndex};
            currentDepth.dimensions.push_back(dimension);
        } else {  // All simplices are in the homology group, i.e. trivial
                  // case 0 <- C_0 <- 0
            dimension = {
                .R = SMatrix<_N>::identity(complex.numSimplicesIn(0)),
                .R_inv = SMatrix<_N>::identity(complex.numSimplicesIn(0)),
                .firstHomologyIndex = 0};
            currentDepth.dimensions.push_back(dimension);
        }

        for (size_t dim = 0; dim < currentDepth.dimensions.size(); dim++) {
            auto matrix_map = getCollapsingMatrix(complex, prevComplex, dim);
            if (currentDepth.dimensions[dim].firstHomologyIndex ==
                    complex.numSimplicesIn(dim) ||
                matrix_map.isNull()) {
                // The domain and image spaces are emptysets. Even though
                // it would be (0), we store it as null
                currentDepth.dimensions[dim].inducedMap =
                    SMatrix<_N>::zeroMatrix();
            } else if (dim >= depths_.back().dimensions.size() ||
                       depths_.back().dimensions[dim].firstHomologyIndex ==
                           prevComplex.numSimplicesIn(dim)) {
                // The image space is emptyset, so we want a matrix with
                // a single row of zeroes
                currentDepth.dimensions[dim].inducedMap = SMatrix<_N>(
                    1, complex.numSimplicesIn(dim) -
                           currentDepth.dimensions[dim].firstHomologyIndex);
            } else {
                // The complete calculation for the induced map
                matrix_map.rightMulIn(currentDepth.dimensions[dim].R);
                matrix_map.leftMulIn(depths_.back().dimensions[dim].R_inv);
                currentDepth.dimensions[dim].inducedMap = matrix_map.submatrix(
                    depths_.back().dimensions[dim].firstHomologyIndex,
                    currentDepth.dimensions[dim].firstHomologyIndex);
            }
        }

        depths_.push_back(currentDepth);
    }

    // Computes the barcode using Algorithm 2
    Barcode computeBarcode() {
        Barcode bcode;
        int n = depths_.size();

        // Only holes up to maxDim can be found
        for (size_t dim = 0; dim <= maxDim_; dim++) {
            // We create a zeroed triangular matrix
            std::vector<std::vector<size_t>> D;
            for (int i = 0; i < n; i++) {
                D.push_back(std::vector<size_t>(i + 1, 0));
            }

            for (ssize_t i = 0; i < n; i++) {
                auto m = getInducedMap(i, dim);
                D[i][i] = m.nullity();
                for (int j = i - 1; j >= 0; j--) {
                    m.leftMulIn(getInducedMap(j, dim));
                    if (m.isNull()) {
                        // We can copy the previous nullity
                        D[i][j] = D[i][j + 1];
                    } else {
                        D[i][j] = m.nullity();
                    }
                }
            }

            for (int i = 1; i < n; i++) {
                for (int j = i; j > 0; j--) {
                    if (D[i][j] > 0) {
                        for (int k = 0; k < j; k++) {
                            D[i][k] -= D[i][j];
                        }
                    }
                }
            }

            for (int i = n - 1; i > 0; i--) {
                for (int j = 0; j < i; j++) {
                    for (int k = j; k < i; k++) {
                        D[k][j] -= D[i][j];
                    }
                }
            }

            for (int i = 0; i < n; i++) {
                for (int j = 0; j <= i; j++) {
                    bcode.add(dim, i, j, D[i][j]);
                }
            }
        }

        return bcode;
    }

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out,
                                    const Module<_N>& module) {
        for (size_t depth = 0; depth < module.depths_.size(); depth++) {
            out << "Depth " << depth << std::endl;
            for (size_t dim = 0; dim < module.depths_[depth].dimensions.size();
                 dim++) {
                out << "Dimension " << dim << std::endl;
                out << module.depths_[depth].dimensions[dim].inducedMap
                    << std::endl;
            }
        }
        return out;
    }
#endif  // DEBUG

   private:
    struct Dim {
        SMatrix<_N> R, R_inv;
        SMatrix<_N> inducedMap;
        size_t firstHomologyIndex;
    };
    struct Depth {
        std::vector<Dim> dimensions;
    };
    std::vector<Depth> depths_;
    CComplex lastComplex_;
    size_t maxDim_;
};

}  // namespace cubitos
