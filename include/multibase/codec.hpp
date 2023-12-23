#pragma once

#include <algorithm>    // for copy, transform
#include <iterator>     // for back_inserter, begin, size
#include <ranges>       // for input_range
#include <span>         // for span
#include <stdexcept>    // for invalid_argument
#include <string>       // for string, operator+
#include <type_traits>  // for underlying_type_t
#include <vector>       // for vector

#pragma warning(push)
#pragma warning(disable : 6285)
#pragma warning(pop)
#include <gsl/assert>                  // for Expects
#include <magic_enum.hpp>              // for enum_cast
#include <range/v3/view/subrange.hpp>  // for subrange

#include <multibase/basic_algorithm.hpp>  // for basic_algorithm
#include <multibase/encoding.hpp>         // for encoding, encoding::base_10

namespace multibase {

template <std::ranges::input_range range>
std::string encode(const range& input, encoding base, bool multiformat = true);

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input, encoding base);

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input);

using base_2 = basic_algorithm<encoding::base_2>;
using base_8 = basic_algorithm<encoding::base_8>;
using base_10 = basic_algorithm<encoding::base_10>;
using base_16 = basic_algorithm<encoding::base_16>;
using base_16_upper = basic_algorithm<encoding::base_16_upper>;
using base_32 = basic_algorithm<encoding::base_32>;
using base_32_upper = basic_algorithm<encoding::base_32_upper>;
using base_32_pad = basic_algorithm<encoding::base_32_pad>;
using base_32_pad_upper = basic_algorithm<encoding::base_32_pad_upper>;
using base_32_hex = basic_algorithm<encoding::base_32_hex>;
using base_32_hex_upper = basic_algorithm<encoding::base_32_hex_upper>;
using base_32_hex_pad = basic_algorithm<encoding::base_32_hex_pad>;
using base_32_hex_pad_upper = basic_algorithm<encoding::base_32_hex_pad_upper>;
using base_32_z = basic_algorithm<encoding::base_32_z>;
using base_36 = basic_algorithm<encoding::base_36>;
using base_36_upper = basic_algorithm<encoding::base_36_upper>;
using base_58_flickr = basic_algorithm<encoding::base_58_flickr>;
using base_58_btc = basic_algorithm<encoding::base_58_btc>;
using base_64 = basic_algorithm<encoding::base_64>;
using base_64_pad = basic_algorithm<encoding::base_64_pad>;
using base_64_url = basic_algorithm<encoding::base_64_url>;
using base_64_url_pad = basic_algorithm<encoding::base_64_url_pad>;

/** IMPLEMENTATION */

// Note that the null terminator on char* strings will also be included.
// To avoid this, explicitly create a string view
template <typename codec, std::ranges::input_range range>
std::string encode(const range& input, bool multibase = true) {
  auto output = codec::encode(input);
  using encoding_type = std::underlying_type_t<encoding>;
  return multibase ? static_cast<encoding_type>(codec::encoding) + output
                   : output;
}

template <typename codec, std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input) {
  std::vector<unsigned char> output(codec::decoded_size(input), 0);
  auto view = codec::decode(input, std::span{output.data(), output.size()});
  std::ranges::copy(view, std::begin(output));
  output.resize(std::size(view));
  return output;
}

template <std::ranges::input_range range>
std::string encode(const range& input, encoding base, bool multiformat) {
  switch (base) {
    using enum multibase::encoding;
    case base_none:
      return static_cast<std::underlying_type_t<multibase::encoding>>(base) +
             std::string{std::begin(input), std::end(input)};
    case base_2:
      return encode<multibase::base_2>(input, multiformat);
    case base_8:
      return encode<multibase::base_8>(input, multiformat);
    case base_10:
      return encode<multibase::base_10>(input, multiformat);
    case base_16:
      return encode<multibase::base_16>(input, multiformat);
    case base_16_upper:
      return encode<multibase::base_16_upper>(input, multiformat);
    case base_32:
      return encode<multibase::base_32>(input, multiformat);
    case base_32_upper:
      return encode<multibase::base_32_upper>(input, multiformat);
    case base_32_pad:
      return encode<multibase::base_32_pad>(input, multiformat);
    case base_32_pad_upper:
      return encode<multibase::base_32_pad_upper>(input, multiformat);
    case base_32_hex:
      return encode<multibase::base_32_hex>(input, multiformat);
    case base_32_hex_upper:
      return encode<multibase::base_32_hex_upper>(input, multiformat);
    case base_32_hex_pad:
      return encode<multibase::base_32_hex_pad>(input, multiformat);
    case base_32_hex_pad_upper:
      return encode<multibase::base_32_hex_pad_upper>(input, multiformat);
    case base_32_z:
      return encode<multibase::base_32_z>(input, multiformat);
    case base_36:
      return encode<multibase::base_36>(input, multiformat);
    case base_36_upper:
      return encode<multibase::base_36_upper>(input, multiformat);
    case base_58_flickr:
      return encode<multibase::base_58_flickr>(input, multiformat);
    case base_58_btc:
      return encode<multibase::base_58_btc>(input, multiformat);
    case base_64:
      return encode<multibase::base_64>(input, multiformat);
    case base_64_pad:
      return encode<multibase::base_64_pad>(input, multiformat);
    case base_64_url:
      return encode<multibase::base_64_url>(input, multiformat);
    case base_64_url_pad:
      return encode<multibase::base_64_url_pad>(input, multiformat);
  }
  return std::string{""};
}

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input, encoding base) {
  switch (base) {
    using enum multibase::encoding;
    case base_none: {
      auto result = std::vector<unsigned char>{};
      std::ranges::transform(input, std::back_inserter(result),
                             [](unsigned char ch) { return ch; });
      return result;
    }
    case base_2:
      return decode<multibase::base_2>(input);
    case base_8:
      return decode<multibase::base_8>(input);
    case base_10:
      return decode<multibase::base_10>(input);
    case base_16:
      return decode<multibase::base_16>(input);
    case base_16_upper:
      return decode<multibase::base_16_upper>(input);
    case base_32:
      return decode<multibase::base_32>(input);
    case base_32_upper:
      return decode<multibase::base_32_upper>(input);
    case base_32_hex:
      return decode<multibase::base_32_hex>(input);
    case base_32_hex_pad:
      return decode<multibase::base_32_hex_pad>(input);
    case base_32_hex_pad_upper:
      return decode<multibase::base_32_hex_pad_upper>(input);
    case base_32_pad:
      return decode<multibase::base_32_pad>(input);
    case base_32_pad_upper:
      return decode<multibase::base_32_pad_upper>(input);
    case base_32_hex_upper:
      return decode<multibase::base_32_hex_upper>(input);
    case base_32_z:
      return decode<multibase::base_32_z>(input);
    case base_36:
      return decode<multibase::base_36>(input);
    case base_36_upper:
      return decode<multibase::base_36_upper>(input);
    case base_58_flickr:
      return decode<multibase::base_58_flickr>(input);
    case base_58_btc:
      return decode<multibase::base_58_btc>(input);
    case base_64:
      return decode<multibase::base_64>(input);
    case base_64_pad:
      return decode<multibase::base_64_pad>(input);
    case base_64_url:
      return decode<multibase::base_64_url>(input);
    case base_64_url_pad:
      return decode<multibase::base_64_url_pad>(input);
  }
  return std::vector<unsigned char>{};
}

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input) {
  Expects(std::size(input) > 0);
  auto first = std::begin(input);
  auto last = std::end(input);
  auto base = magic_enum::enum_cast<encoding>(*first);
  if (!base) {
    throw std::invalid_argument{fmt::format("Unsupported base {}", *first)};
  }
  return decode(ranges::subrange(++first, last), *base);
}
}  // namespace multibase
