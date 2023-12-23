#pragma once

#include <concepts>  // for integral

#include <multibase/portability.hpp>

namespace multibase {

/** Compute base-2 logarithm */
template <std::integral integer>
MULTIBASE_CONSTEVAL static integer log2(integer n) noexcept {
  if (n == 0) {
    return 1;
  } else if (n == 1) {
    return 0;
  }
  return 1 + log2(n / 2);
}

}  // namespace multibase
