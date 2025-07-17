#pragma once
// file: bitset_operators.h
// description: defines bitset arithmetics that will be used for computing
//              the complex

template <std::size_t N>
std::bitset<N> operator+(const std::bitset<N>& lhs,
                         const std::bitset<N>& rhs) {
    return std::bitset<N>(lhs.to_ullong() + rhs.to_ullong());
}

template <std::size_t N>
std::bitset<N> operator-(const std::bitset<N>& lhs,
                         const std::bitset<N>& rhs) {
    return std::bitset<N>(lhs.to_ullong() - rhs.to_ullong());
}

template <std::size_t N>
std::bitset<N> operator*(const std::bitset<N>& lhs,
                         const std::bitset<N>& rhs) {
    return std::bitset<N>(lhs.to_ullong() * rhs.to_ullong());
}

template <std::size_t N>
std::bitset<N>& operator+=(std::bitset<N>& lhs, const std::bitset<N>& rhs) {
    lhs = lhs + rhs;
    return lhs;
}

template <std::size_t N>
std::bitset<N>& operator-=(std::bitset<N>& lhs, const std::bitset<N>& rhs) {
    lhs = lhs - rhs;
    return lhs;
}

template <std::size_t N>
std::bitset<N>& operator*=(std::bitset<N>& lhs, const std::bitset<N>& rhs) {
    lhs = lhs.to_ullong() * rhs.to_ullong();
    return lhs;
}
