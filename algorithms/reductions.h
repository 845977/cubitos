#pragma once
// file: algorithms/reductions.h
// description: implements column and row echelonizers for smatrix

#include "../smatrix.h"

namespace cubitos {

/* Algorithms based on
 * https://www.jeremykun.com/2013/04/10/computing-homology/
 */

template <
    size_t _N, 
    bool _EnableComplementary
>
inline void 
rowReduce(SMatrix<_N>& A,
        SMatrix<_N>* B,
        SMatrix<_N>& P,
        SMatrix<_N>& P_inv,
        size_t& i
        ) {
    auto& field = SMatrix<_N>::field_;

    size_t numRows = A.n_, numCols = A.m_;

    /* Row echelonizes A into P A */
    for (size_t j = 0; i < numRows && j < numCols;) {
        size_t nonzeroRow = i;
        for (; nonzeroRow < numRows && field.isZero(A.get(nonzeroRow, j));
             nonzeroRow++);
        if (nonzeroRow == numRows) {
            j++;
            continue;
        } else if (nonzeroRow != i) {
            A.rowSwap(i, nonzeroRow);
            P.rowSwap(i, nonzeroRow);
            if constexpr (_EnableComplementary) {
                B->colSwap(i, nonzeroRow);
            }
            P_inv.colSwap(i, nonzeroRow);
        }

        Element pivot = A.get(i, j);
        field.invin(pivot);
        field.negin(pivot);
        for (size_t otherRow = i + 1; otherRow < numRows; otherRow++) {
            Element c = A.get(otherRow, j);
            if (!field.isZero(c)) {
                field.mulin(c, pivot);
                A.rowCombine(otherRow, i, c);
                P.rowCombine(otherRow, i, c);
                field.negin(c);
                P_inv.colCombine(i, otherRow, c);
                // If we have set a complementary matrix
                if constexpr (_EnableComplementary) {
                    B->colCombine(i, otherRow, c);
                }
            }
        }

        i++; j++;
    }
}

template <
    size_t _N, 
    bool _EnableComplementary
>
inline void 
columnReduce(SMatrix<_N>& A, 
        SMatrix<_N>* B, 
        SMatrix<_N>& Q, 
        SMatrix<_N>& Q_inv,
        size_t& j
        ) {
    auto& field = SMatrix<_N>::field_;
    size_t numRows = A.n_, numCols = A.m_;

    /* Column echelonizes A into A Q */
    for (size_t i = 0; i < numRows && j < numCols;) {
        size_t nonzeroCol = j;
        for (; nonzeroCol < numCols && field.isZero(A.get(i, nonzeroCol));
             nonzeroCol++);

        if (nonzeroCol == numCols) {
            i++;
            continue;
        } else if (nonzeroCol != j) {
            A.colSwap(j, nonzeroCol);
            Q.colSwap(j, nonzeroCol);
            if constexpr (_EnableComplementary) {
                B->rowSwap(j, nonzeroCol);
            }
            Q_inv.rowSwap(j, nonzeroCol);
        }

        Element pivot = A.get(i, j);
        field.invin(pivot);
        field.negin(pivot);
        for (size_t otherCol = j + 1; otherCol < numCols; otherCol++) {
            Element c = A.get(i, otherCol);
            if (!field.isZero(c)) {
                field.mulin(c, pivot);
                A.colCombine(otherCol, j, c);
                Q.colCombine(otherCol, j, c);
                field.negin(c);
                // If we have set a complementary matrix
                if constexpr (_EnableComplementary) {
                    B->rowCombine(j, otherCol, c);
                }
                Q_inv.rowCombine(j, otherCol, c);
            }
        }

        i++; j++;
    }
}

template <size_t _N>
inline void 
columnReduce(SMatrix<_N>& A, 
        SMatrix<_N>& Q, 
        SMatrix<_N>& Q_inv,
        size_t& firstHomologyIndex
        ) {
    Q = SMatrix<_N>::identity(A.m_);
    Q_inv = SMatrix<_N>::identity(A.m_);
    firstHomologyIndex = 0;
    columnReduce<_N, false>(A, nullptr, Q, Q_inv, firstHomologyIndex);
}

template <size_t _N>
inline void 
rowReduce(SMatrix<_N>& A, 
        SMatrix<_N>& P, 
        SMatrix<_N>& P_inv,
        size_t& firstHomologyIndex
        ) {
    P = SMatrix<_N>::identity(A.n_);
    P_inv = SMatrix<_N>::identity(A.n_);
    firstHomologyIndex = 0;
    rowReduce<_N, false>(A, nullptr, P, P_inv, firstHomologyIndex);
}

template <size_t _N>
inline void 
simultaneousReduce(SMatrix<_N>& A, 
        SMatrix<_N>& B, 
        SMatrix<_N>& R,
        SMatrix<_N>& R_inv,
        size_t& firstHomologyIndex
        ) {
    assert(A.m_ == B.n_);

    R = SMatrix<_N>::identity(A.m_);
    R_inv = SMatrix<_N>::identity(A.m_);

    firstHomologyIndex = 0;

    columnReduce<_N, true>(A, &B, R, R_inv, firstHomologyIndex);
    rowReduce<_N, true>(B, &A, R_inv, R, firstHomologyIndex);
}

}  // namespace cubitos
