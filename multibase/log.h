#pragma once

#include <numeric>

namespace multibase {

/** Compute base-2 logarithm */
template <std::integral integer>
consteval static integer log2(integer n) noexcept {
  if (n == 0) {
    return 1;
  } else if (n == 1) {
    return 0;
  }
  return 1 + log2(n / 2);
}

}