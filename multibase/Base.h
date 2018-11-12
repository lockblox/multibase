#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <string_view>
#include <vector>

namespace multibase {

/** Compute base-2 logarithm */
constexpr std::size_t log2(std::size_t n) {
  return ((n == 1) ? 0 : ((n < 2) ? 1 : 1 + log2(n / 2)));
}

/** Available bases */
enum Base {
  BASE_INVALID = 0,
  BASE_16 = 102,
  BASE_16_UPPER = 70,
  BASE_32 = 98,
  BASE_32_UPPER = 66,
  BASE_58_BTC = 122,
  BASE_64 = 109
};

/** Bases are defined by a charset and a valset used for encoding and decoding
 * respectively */
template <Base N>
struct base_traits {
  static const std::array<char, 0> charset;
  static const std::array<unsigned char, 256> valset;
};

/** Compute a lookup table for base decoding */
template <typename Traits>
std::array<unsigned char, 256> compute_valset() {
  using CharT = typename decltype(Traits::charset)::value_type;
  using value_type = typename decltype(Traits::charset)::value_type;
  constexpr auto radix = sizeof(Traits::charset) / sizeof(value_type);
  static_assert(sizeof(Traits::charset) / sizeof(CharT) == radix);
  static_assert(std::is_same<CharT, value_type>::value);
  std::array<unsigned char, 256> output{};
  output.fill(-1);
  unsigned char i = 0;
  std::for_each(std::begin(Traits::charset), std::end(Traits::charset),
                [&i, &output](unsigned char c) { output[c] = i++; });
  return output;
}

/** Multibase provides an encoder and decoder for converting binary data to
 * and from string-based representations, up to base256 */
template <Base N, typename Traits = base_traits<N>>
struct base {
  static std::string encode(std::string_view input);
  static std::vector<unsigned char> decode(std::string_view input);
};

/** Encode raw data into a base string */
template <Base N, typename Traits>
std::string base<N, Traits>::encode(std::string_view input) {
  using value_type = typename decltype(Traits::charset)::value_type;
  constexpr auto radix = sizeof(Traits::charset) / sizeof(value_type);
  auto width = log2(256) / log2(radix);
  auto output = std::string(input.size() * width, 0);
  auto it = output.begin();
  for (unsigned char c : input) {
    auto first = it;
    if (c == 0) *it++ = Traits::charset[c];
    while (c != 0) {
      *it++ = Traits::charset[c % radix];
      c /= radix;
    }
    while (std::distance(first, it) < width) *it++ = '0';  // zero-pad
    std::reverse(first, it);
  }
  return output;
}

/** Decode a base encoded string into raw data */
template <Base N, typename Traits>
std::vector<unsigned char> base<N, Traits>::decode(std::string_view input) {
  using T = typename decltype(Traits::valset)::value_type;
  using CharT = typename decltype(Traits::charset)::value_type;
  static_assert(sizeof(Traits::valset) / sizeof(T) == 256);
  constexpr auto radix = sizeof(Traits::charset) / sizeof(CharT);
  auto ratio = log2(256) / log2(radix);
  auto output = std::vector<unsigned char>(input.size() / ratio);
  auto it = input.rbegin();
  for (auto& o : output) {
    for (int p = 1, n = ratio; n > 0; p *= radix, --n) {
      o += Traits::valset[*it++] * p;
    }
  }
  std::reverse(output.begin(), output.end());
  return output;
}

/** Base 16 traits */
template <>
struct base_traits<BASE_16> {
  constexpr static const std::array<char, 16> charset = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  static const std::array<unsigned char, 256> valset;
};
using base16 = base<BASE_16>;

}  // namespace multibase
