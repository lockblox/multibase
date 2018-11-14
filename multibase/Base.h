#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string_view>

namespace multibase {

/** Available bases */
enum Base : unsigned char {
  BASE_INVALID = 0,
  BASE_16 = 102,
  BASE_16_UPPER = 70,
  BASE_32 = 98,
  BASE_32_UPPER = 66,
  BASE_58_BTC = 122,
  BASE_64 = 109
};

/** Codec to encode/decode in a base */
struct codec {
  std::string (*encode)(const std::string_view&);
  std::string (*decode)(const std::string_view&);
};

/** Available codecs */
std::array<codec, 256>& codecs();

/** Encode data in the given multibase */
std::string encode(const std::string_view& input, Base base = BASE_58_BTC);

/** Decode a multibase string */
std::string decode(const std::string_view& input);

/** Bases are defined by a charset used for encoding and decoding
 * respectively */
template <Base N>
struct base_traits {
  static const std::array<char, 0> charset;
};

template <class It, class T>
constexpr It find(It begin, It end, T const& value) noexcept {
  return (begin != end && *begin != value)
             ? multibase::find(++begin, end, value)
             : begin;
}

template <typename Traits>
constexpr unsigned char getval(std::size_t pos) noexcept {
  using Charset = decltype(Traits::charset);
  using value_type = typename Charset::value_type;
  using iterator = typename Charset::const_iterator;
  return multibase::find(Traits::charset.begin(), Traits::charset.end(), pos) ==
                 Traits::charset.end()
             ? 0
             : static_cast<unsigned char>(
                   std::distance(Traits::charset.begin(),
                                 multibase::find(Traits::charset.begin(),
                                                 Traits::charset.end(), pos)));
}

/** Compute a lookup table for base decoding */
template <typename Traits>
constexpr std::array<unsigned char, 256> compute_valset() {
  using CharT = typename decltype(Traits::charset)::value_type;
  using CharsetT = decltype(Traits::charset);
  using value_type = typename CharsetT::value_type;
  constexpr auto radix = sizeof(Traits::charset) / sizeof(value_type);
  static_assert(sizeof(Traits::charset) / sizeof(CharT) == radix);
  static_assert(std::is_same<CharT, value_type>::value);
  constexpr std::array<unsigned char, 256> output = {
      getval<Traits>(0),   getval<Traits>(1),   getval<Traits>(2),
      getval<Traits>(3),   getval<Traits>(4),   getval<Traits>(5),
      getval<Traits>(6),   getval<Traits>(7),   getval<Traits>(8),
      getval<Traits>(9),   getval<Traits>(10),  getval<Traits>(11),
      getval<Traits>(12),  getval<Traits>(13),  getval<Traits>(14),
      getval<Traits>(15),  getval<Traits>(16),  getval<Traits>(17),
      getval<Traits>(18),  getval<Traits>(19),  getval<Traits>(20),
      getval<Traits>(21),  getval<Traits>(22),  getval<Traits>(23),
      getval<Traits>(24),  getval<Traits>(25),  getval<Traits>(26),
      getval<Traits>(27),  getval<Traits>(28),  getval<Traits>(29),
      getval<Traits>(30),  getval<Traits>(31),  getval<Traits>(32),
      getval<Traits>(33),  getval<Traits>(34),  getval<Traits>(35),
      getval<Traits>(36),  getval<Traits>(37),  getval<Traits>(38),
      getval<Traits>(39),  getval<Traits>(40),  getval<Traits>(41),
      getval<Traits>(42),  getval<Traits>(43),  getval<Traits>(44),
      getval<Traits>(45),  getval<Traits>(46),  getval<Traits>(47),
      getval<Traits>(48),  getval<Traits>(49),  getval<Traits>(50),
      getval<Traits>(51),  getval<Traits>(52),  getval<Traits>(53),
      getval<Traits>(54),  getval<Traits>(55),  getval<Traits>(56),
      getval<Traits>(57),  getval<Traits>(58),  getval<Traits>(59),
      getval<Traits>(60),  getval<Traits>(61),  getval<Traits>(62),
      getval<Traits>(63),  getval<Traits>(64),  getval<Traits>(65),
      getval<Traits>(66),  getval<Traits>(67),  getval<Traits>(68),
      getval<Traits>(69),  getval<Traits>(70),  getval<Traits>(71),
      getval<Traits>(72),  getval<Traits>(73),  getval<Traits>(74),
      getval<Traits>(75),  getval<Traits>(76),  getval<Traits>(77),
      getval<Traits>(78),  getval<Traits>(79),  getval<Traits>(80),
      getval<Traits>(81),  getval<Traits>(82),  getval<Traits>(83),
      getval<Traits>(84),  getval<Traits>(85),  getval<Traits>(86),
      getval<Traits>(87),  getval<Traits>(88),  getval<Traits>(89),
      getval<Traits>(90),  getval<Traits>(91),  getval<Traits>(92),
      getval<Traits>(93),  getval<Traits>(94),  getval<Traits>(95),
      getval<Traits>(96),  getval<Traits>(97),  getval<Traits>(98),
      getval<Traits>(99),  getval<Traits>(100), getval<Traits>(101),
      getval<Traits>(102), getval<Traits>(103), getval<Traits>(104),
      getval<Traits>(105), getval<Traits>(106), getval<Traits>(107),
      getval<Traits>(108), getval<Traits>(109), getval<Traits>(110),
      getval<Traits>(111), getval<Traits>(112), getval<Traits>(113),
      getval<Traits>(114), getval<Traits>(115), getval<Traits>(116),
      getval<Traits>(117), getval<Traits>(118), getval<Traits>(119),
      getval<Traits>(120), getval<Traits>(121), getval<Traits>(122),
      getval<Traits>(123), getval<Traits>(124), getval<Traits>(125),
      getval<Traits>(126), getval<Traits>(127), getval<Traits>(128),
      getval<Traits>(129), getval<Traits>(130), getval<Traits>(131),
      getval<Traits>(132), getval<Traits>(133), getval<Traits>(134),
      getval<Traits>(135), getval<Traits>(136), getval<Traits>(137),
      getval<Traits>(138), getval<Traits>(139), getval<Traits>(140),
      getval<Traits>(141), getval<Traits>(142), getval<Traits>(143),
      getval<Traits>(144), getval<Traits>(145), getval<Traits>(146),
      getval<Traits>(147), getval<Traits>(148), getval<Traits>(149),
      getval<Traits>(150), getval<Traits>(151), getval<Traits>(152),
      getval<Traits>(153), getval<Traits>(154), getval<Traits>(155),
      getval<Traits>(156), getval<Traits>(157), getval<Traits>(158),
      getval<Traits>(159), getval<Traits>(160), getval<Traits>(161),
      getval<Traits>(162), getval<Traits>(163), getval<Traits>(164),
      getval<Traits>(165), getval<Traits>(166), getval<Traits>(167),
      getval<Traits>(168), getval<Traits>(169), getval<Traits>(170),
      getval<Traits>(171), getval<Traits>(172), getval<Traits>(173),
      getval<Traits>(174), getval<Traits>(175), getval<Traits>(176),
      getval<Traits>(177), getval<Traits>(178), getval<Traits>(179),
      getval<Traits>(180), getval<Traits>(181), getval<Traits>(182),
      getval<Traits>(183), getval<Traits>(184), getval<Traits>(185),
      getval<Traits>(186), getval<Traits>(187), getval<Traits>(188),
      getval<Traits>(189), getval<Traits>(190), getval<Traits>(191),
      getval<Traits>(192), getval<Traits>(193), getval<Traits>(194),
      getval<Traits>(195), getval<Traits>(196), getval<Traits>(197),
      getval<Traits>(198), getval<Traits>(199), getval<Traits>(200),
      getval<Traits>(201), getval<Traits>(202), getval<Traits>(203),
      getval<Traits>(204), getval<Traits>(205), getval<Traits>(206),
      getval<Traits>(207), getval<Traits>(208), getval<Traits>(209),
      getval<Traits>(210), getval<Traits>(211), getval<Traits>(212),
      getval<Traits>(213), getval<Traits>(214), getval<Traits>(215),
      getval<Traits>(216), getval<Traits>(217), getval<Traits>(218),
      getval<Traits>(219), getval<Traits>(220), getval<Traits>(221),
      getval<Traits>(222), getval<Traits>(223), getval<Traits>(224),
      getval<Traits>(225), getval<Traits>(226), getval<Traits>(227),
      getval<Traits>(228), getval<Traits>(229), getval<Traits>(230),
      getval<Traits>(231), getval<Traits>(232), getval<Traits>(233),
      getval<Traits>(234), getval<Traits>(235), getval<Traits>(236),
      getval<Traits>(237), getval<Traits>(238), getval<Traits>(239),
      getval<Traits>(240), getval<Traits>(241), getval<Traits>(242),
      getval<Traits>(243), getval<Traits>(244), getval<Traits>(245),
      getval<Traits>(246), getval<Traits>(247), getval<Traits>(248),
      getval<Traits>(249), getval<Traits>(250), getval<Traits>(251),
      getval<Traits>(252), getval<Traits>(253), getval<Traits>(254),
      getval<Traits>(255)};
  return output;
}

/** Multibase provides an encoder and decoder for converting binary data to
 * and from string-based representations, up to base256 */
template <Base N, typename Traits = base_traits<N>>
struct base {
  static std::string encode(const std::string_view& input);
  static std::string decode(const std::string_view& input);
  base();
};

template <Base N, typename Traits>
base<N, Traits>::base() {
  codecs()[N] = codec{encode, decode};
}

/** Compute base-2 logarithm */
constexpr std::size_t log2(std::size_t n) {
  return ((n == 1) ? 0 : ((n < 2) ? 1 : 1 + log2(n / 2)));
}

template <Base N, typename Traits>
std::string base<N, Traits>::encode(const std::string_view& input) {
  constexpr auto radix = sizeof(Traits::charset) /
                         sizeof(typename decltype(Traits::charset)::value_type);
  constexpr auto width = log2(256) / log2(radix);
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

template <Base N, typename Traits>
std::string base<N, Traits>::decode(const std::string_view& input) {
  constexpr auto valset = compute_valset<Traits>();
  constexpr auto radix = sizeof(Traits::charset) /
                         sizeof(typename decltype(Traits::charset)::value_type);
  constexpr auto ratio = log2(256) / log2(radix);
  auto output = std::string(input.size() / ratio, 0);
  auto it = input.rbegin();
  for (auto& o : output) {
    for (int p = 1, n = ratio; n > 0; p *= radix, --n) {
      o += valset[*it++] * p;
    }
  }
  std::reverse(output.begin(), output.end());
  return output;
}

template <>
struct base_traits<BASE_16> {
  constexpr static const std::array<char, 16> charset = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
};
extern base<BASE_16> base16;

}  // namespace multibase
