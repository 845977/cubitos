#pragma once
// file: barcode.h
// description: Stores a barcode as a vector of P-intervals in each dimension
//              and outputs it

#include <map>
#include <ostream>
#include <vector>

namespace cubitos {

class Barcode {
   public:
    // Constructors
    Barcode();

    // Adds a [i,j-1) bar in dim barcode
    void add(size_t dim, size_t i, size_t j, size_t numBars);

    // Prints a barcode in Tikz format
    std::string tikzbarcode() const;
    friend std::ostream& operator<<(std::ostream& out, const Barcode& bcode);

   private:
    std::vector<std::map<std::pair<size_t, size_t>, size_t>> bars_;
};

std::ostream& operator<<(std::ostream& out, const Barcode& bcode);

}  // namespace cubitos
