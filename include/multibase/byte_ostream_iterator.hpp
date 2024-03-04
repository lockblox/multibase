#ifndef MULTIBASE_BYTE_OSTREAM_ITERATOR_HPP
#define MULTIBASE_BYTE_OSTREAM_ITERATOR_HPP

#include <cstddef>  // For std::byte
#include <iostream>
#include <iterator>

class byte_ostream_iterator {
 public:
  using value_type = std::byte;
  using difference_type = std::ptrdiff_t;
  using pointer = void;    // Not applicable for output iterators
  using reference = void;  // Not applicable for output iterators
  using iterator_category = std::output_iterator_tag;

  explicit byte_ostream_iterator(std::ostream& ostream)
      : ostream_iterator_(ostream) {}

  byte_ostream_iterator& operator++() {
    *ostream_iterator_ = static_cast<unsigned char>(val_);
    return *this;
  }

  // NOLINTNEXTLINE(cert-dcl21-cpp)
  byte_ostream_iterator operator++(int) {
    auto backup = *this;
    ++*this;
    return backup;
  }

  std::byte& operator*() { return val_; }

 private:
  std::byte val_{};
  std::ostream_iterator<unsigned char> ostream_iterator_;
};

#endif  // MULTIBASE_BYTE_OSTREAM_ITERATOR_HPP
