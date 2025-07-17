#pragma once
// file: smatrix.h
// description: Implements all matrix operations needed for linalg algorithms
//              (chapter 3)

#include <linbox/linbox-config.h>
#include <linbox/matrix/transpose-matrix.h>
#include <linbox/ring/modular.h>
#include <linbox/solutions/echelon.h>
#include <linbox/solutions/methods.h>
#include <linbox/solutions/rank.h>

#include "config.h"

namespace cubitos {

typedef Givaro::Modular<double> Field;
typedef Field::Element Element;

template <size_t _N>
class SMatrix {
   public:
    typedef Givaro::Modular<double> Field;
    typedef Field::Element Element;
    SMatrix() : n_(0), m_(0), matrix_(field_, 0, 0) {}
    SMatrix(int n, int m) : n_(n), m_(m), matrix_(field_, n, m) {}

    // Returns the identity matrix I_n
    static SMatrix<_N> identity(size_t n) {
        SMatrix id(n, n);
        for (size_t i = 0; i < n; i++) {
            id.insert(i, i, 1);
        }
        return id;
    }

    // Returns the null matrix
    static SMatrix<_N> zeroMatrix() { return SMatrix<_N>(0, 0); }

    // Returns lhs * rhs
    static SMatrix<_N> mul(const SMatrix<_N>& lhs, const SMatrix<_N>& rhs) {
        if (lhs.isNull() || rhs.isNull()) {
            return zeroMatrix();
        }
        SMatrix<_N> mat(lhs.n_, rhs.m_);
        SMatrix<_N>::matrixDomain_.mul(mat.matrix_, lhs.matrix_, rhs.matrix_);
        return mat;
    }

    // Whether this matrix is the null one
    bool isNull() const { return (n_ == 0 && m_ == 0); }

    inline void insert(int i, int j, const Element& v) {
        matrix_.setEntry(i, j, v);
    }

    inline void add(int i, int j, const Element& v) {
        auto& elem = matrix_.refEntry(i, j);
        elem += v;
    }

    inline void sub(int i, int j, const Element& v) {
        auto& elem = matrix_.refEntry(i, j);
        elem -= v;
    }

    const inline Element& get(int i, int j) const {
        return matrix_.getEntry(i, j);
    }

    inline void scaleRow(size_t row, const Element& elm) {
        for (size_t i = 0; i < m_; i++) {
            field_.mulin(matrix_.refEntry(row, i), elm);
        }
    }

    inline void scaleCol(size_t col, const Element& elm) {
        for (size_t j = 0; j < n_; j++) {
            field_.mulin(matrix_.refEntry(j, col), elm);
        }
    }

    inline void colSwap(size_t col1, size_t col2) {
        for (size_t i = 0; i < n_; i++) {
            Element aux = matrix_.getEntry(i, col1);
            matrix_.setEntry(i, col1, matrix_.getEntry(i, col2));
            matrix_.setEntry(i, col2, aux);
        }
    }

    inline void rowSwap(size_t row1, size_t row2) {
        for (size_t j = 0; j < m_; j++) {
            Element aux = matrix_.getEntry(row1, j);
            matrix_.setEntry(row1, j, matrix_.getEntry(row2, j));
            matrix_.setEntry(row2, j, aux);
        }
    }

    inline void colCombine(size_t addTo, size_t scaleCol,
                           const Element& scaleAmt) {
        for (size_t i = 0; i < n_; i++) {
            field_.axpyin(matrix_.refEntry(i, addTo),
                          matrix_.getEntry(i, scaleCol), scaleAmt);
        }
    }

    inline void rowCombine(size_t addTo, size_t scaleRow,
                           const Element& scaleAmt) {
        for (size_t j = 0; j < m_; j++) {
            field_.axpyin(matrix_.refEntry(addTo, j),
                          matrix_.getEntry(scaleRow, j), scaleAmt);
        }
    }

    inline void rightMulIn(const SMatrix<_N>& rhs) { *this = mul(*this, rhs); }

    inline void leftMulIn(const SMatrix<_N>& lhs) { *this = mul(lhs, *this); }

    // Pre: firstRow < n_ && firstCol < m_
    SMatrix<_N> submatrix(size_t firstRow, size_t firstCol) {
        SMatrix<_N> mat(n_ - firstRow, m_ - firstCol);
        for (size_t i = firstRow; i < n_; i++) {
            for (size_t j = firstCol; j < m_; j++) {
                mat.insert(i - firstRow, j - firstCol, get(i, j));
            }
        }
        return mat;
    }

    size_t rank() {
        if (isNull()) {
            return 0;
        }
        size_t r;
        LinBox::rank(r, matrix_);
        return r;
    }

    size_t nullity() {
        if (isNull()) {
            return 0;
        }
        size_t r;
        LinBox::rank(r, matrix_);
        return m_ - r;
    }

    // All templated reduction algorithms
    template <size_t _M, bool _EnableComplementary>
    friend void rowReduce(SMatrix<_M>& A, SMatrix<_M>* B, SMatrix<_M>& P,
                          SMatrix<_M>& P_inv, size_t& i);
    template <size_t _M, bool _EnableComplementary>
    friend void columnReduce(SMatrix<_M>& A, SMatrix<_M>* B, SMatrix<_M>& Q,
                             SMatrix<_M>& Q_inv, size_t& j);

    // The ones that are actually used
    template <size_t _M>
    friend void rowReduce(SMatrix<_M>& A, SMatrix<_M>& P, SMatrix<_M>& P_inv,
                          size_t& firstHomologyIndex);
    template <size_t _M>
    friend void columnReduce(SMatrix<_M>& A, SMatrix<_M>& Q,
                             SMatrix<_M>& Q_inv, size_t& firstHomologyIndex);
    template <size_t _M>
    friend void simultaneousReduce(SMatrix<_M>& A, SMatrix<_M>& B,
                                   SMatrix<_M>& R, SMatrix<_M>& R_inv,
                                   size_t& firstHomologyIndex);

#ifdef DEBUG
    friend std::ostream& operator<<(std::ostream& out,
                                    const SMatrix<_N>& smatrix) {
        if (smatrix.isNull()) {
            out << "Empty matrix" << std::endl;
            return out;
        }
        for (size_t i = 0; i < smatrix.matrix_.rowdim(); i++) {
            for (size_t j = 0; j < smatrix.matrix_.coldim(); j++) {
                out << std::setw(3) << smatrix.get(i, j) << ' ';
            }
            out << std::endl;
        }
        return out;
    }
#endif  // DEBUG
   private:
    size_t n_, m_;
    static const Field field_;
    static const LinBox::MatrixDomain<Field> matrixDomain_;
    LinBox::DenseMatrix<Field> matrix_;
};

template <size_t _N>
const Field SMatrix<_N>::field_ = Field(_N);
template <size_t _N>
const LinBox::MatrixDomain<Field> SMatrix<_N>::matrixDomain_ =
    LinBox::MatrixDomain<Field>(SMatrix<_N>::field_);

}  // namespace cubitos
