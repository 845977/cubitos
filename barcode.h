#pragma once

#include <map>
#include <ostream>
#include <vector>

namespace cubitos {

class Barcode {
   public:
    Barcode();

    // Adds a [i,j-1) bar in dim barcode
    void add(size_t dim, size_t i, size_t j, size_t numBars);

    std::string tikzbarcode() const;
    friend std::ostream& operator<<(std::ostream& out, const Barcode& bcode);

   private:
    size_t maxLen_;
    std::vector<std::map<std::pair<size_t, size_t>, size_t>> bars_;
};

std::ostream& operator<<(std::ostream& out, const Barcode& bcode);

}  // namespace cubitos
