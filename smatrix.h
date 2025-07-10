#pragma once
/* file: smatrix.h
 */

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

    static SMatrix<_N> identity(size_t n) {
        SMatrix id(n, n);
        for (size_t i = 0; i < n; i++) {
            id.insert(i, i, 1);
        }
        return id;
    }

    static SMatrix<_N> zeroMatrix() { return SMatrix<_N>(0, 0); }

    static SMatrix<_N> mul(const SMatrix<_N>& lhs, const SMatrix<_N>& rhs) {
        if (lhs.isNull() || rhs.isNull()) {
            return zeroMatrix();
        }
        SMatrix<_N> mat(lhs.n_, rhs.m_);
        SMatrix<_N>::matrixDomain_.mul(mat.matrix_, lhs.matrix_, rhs.matrix_);
        return mat;
    }

    bool isNull() const { return (n_ == 0 && m_ == 0); }

    void insert(int i, int j, const Element& v) { matrix_.setEntry(i, j, v); }

    void add(int i, int j, const Element& v) {
        auto& elem = matrix_.refEntry(i, j);
        elem += v;
    }

    void sub(int i, int j, const Element& v) {
        auto& elem = matrix_.refEntry(i, j);
        elem -= v;
    }

    const Element& get(int i, int j) const { return matrix_.getEntry(i, j); }

    void scaleRow(size_t row, const Element& elm) {
        for (size_t i = 0; i < m_; i++) {
            field_.mulin(matrix_.refEntry(row, i), elm);
        }
    }

    void scaleCol(size_t col, const Element& elm) {
        for (size_t j = 0; j < n_; j++) {
            field_.mulin(matrix_.refEntry(j, col), elm);
        }
    }

    void colSwap(size_t col1, size_t col2) {
        for (size_t i = 0; i < n_; i++) {
            Element aux = matrix_.getEntry(i, col1);
            matrix_.setEntry(i, col1, matrix_.getEntry(i, col2));
            matrix_.setEntry(i, col2, aux);
        }
    }

    void rowSwap(size_t row1, size_t row2) {
        for (size_t j = 0; j < m_; j++) {
            Element aux = matrix_.getEntry(row1, j);
            matrix_.setEntry(row1, j, matrix_.getEntry(row2, j));
            matrix_.setEntry(row2, j, aux);
        }
    }

    void colCombine(size_t addTo, size_t scaleCol, const Element& scaleAmt) {
        for (size_t i = 0; i < n_; i++) {
            field_.axpyin(matrix_.refEntry(i, addTo),
                          matrix_.getEntry(i, scaleCol), scaleAmt);
        }
    }

    void rowCombine(size_t addTo, size_t scaleRow, const Element& scaleAmt) {
        for (size_t j = 0; j < m_; j++) {
            field_.axpyin(matrix_.refEntry(addTo, j),
                          matrix_.getEntry(scaleRow, j), scaleAmt);
        }
    }

    // Pivots of a row echelonized matrix
    std::vector<size_t> pivots() {
        std::vector<size_t> ret;

        for (size_t i = 0, j = 0; i < n_ && j < m_; i++) {
            for (size_t notNullCol = j;; notNullCol++) {
                if (notNullCol == m_) {
                    return ret;
                }
                if (!field_.isZero(matrix_.getEntry(i, notNullCol))) {
                    ret.push_back(i);
                    j = notNullCol + 1;
                    break;
                }
            }
        }

        return ret;
    }

    std::vector<size_t> emptyCols() {
        std::vector<size_t> ret;
        for (size_t j = 0; j < m_; j++) {
            for (size_t i = 0; i < n_; i++) {
                if (!field_.isZero(matrix_.getEntry(i, j))) {
                    goto Skip;
                }
            }
            ret.push_back(j);
        Skip:
            continue;
        }
        return ret;
    }

    SMatrix<_N> getCol(const size_t col) {
        SMatrix<_N> column(n_, 1);
        for (size_t i = 0; i < n_; i++) {
            column.matrix_.setEntry(i, 0, matrix_.getEntry(i, col));
        }
        return column;
    }

    SMatrix<_N> transpose() const {
        SMatrix<_N> ret(m_, n_);
        for (size_t i = 0; i < n_; i++) {
            for (size_t j = 0; j < m_; j++) {
                ret.matrix_.setEntry(j, i, matrix_.getEntry(i, j));
            }
        }
        return ret;
    }

    SMatrix<_N> submatrix(std::vector<size_t> rows, std::vector<size_t> cols) {
        SMatrix<_N> mat(rows.size(), cols.size());
        for (size_t i = 0; i < rows.size(); i++) {
            for (size_t j = 0; j < cols.size(); j++) {
                mat.insert(i, j, get(rows[i], cols[j]));
            }
        }

        return mat;
    }

    SMatrix<_N> submatrixCols(std::vector<size_t> cols) {
        SMatrix<_N> mat(n_, cols.size());
        for (size_t i = 0; i < n_; i++) {
            for (size_t j = 0; j < cols.size(); j++) {
                mat.insert(i, j, get(i, cols[j]));
            }
        }

        return mat;
    }

    void rightMulIn(const SMatrix<_N>& rhs) { *this = mul(*this, rhs); }

    void leftMulIn(const SMatrix<_N>& lhs) { *this = mul(lhs, *this); }

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
        return m_ - rank();
    }

    /* All column reduction algorithms */
    template <size_t _M, bool _EnableComplementary>
    friend void rowReduce(SMatrix<_M>& A, SMatrix<_M>& B, SMatrix<_M>& P,
                          SMatrix<_M>& P_inv);
    template <size_t _M, bool _EnableComplementary>
    friend void columnReduce(SMatrix<_M>& A, SMatrix<_M>& B, SMatrix<_M>& Q,
                             SMatrix<_M>& Q_inv);

    template <size_t _M>
    friend void rowReduce(SMatrix<_M>& A, SMatrix<_M>& B, SMatrix<_M>& P,
                          SMatrix<_M>& P_inv);

    template <size_t _M>
    friend void rowReduce(SMatrix<_M>& A, SMatrix<_M>& P, SMatrix<_M>& P_inv);

    template <size_t _M>
    friend void columnReduce(SMatrix<_M>& A, SMatrix<_M>& B, SMatrix<_M>& Q,
                             SMatrix<_M>& Q_inv);

    template <size_t _M>
    friend void columnReduce(SMatrix<_M>& A, SMatrix<_M>& Q,
                             SMatrix<_M>& Q_inv);

    template <size_t _M>
    friend void simultaneousReduce(SMatrix<_M>& A, SMatrix<_M>& B,
                                   SMatrix<_M>& R, SMatrix<_M>& R_inv);

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
