#ifndef MULTIBASE_CODEC_HPP
#define MULTIBASE_CODEC_HPP

#include <algorithm>    // for copy, transform
#include <iterator>     // for back_inserter, begin, size
#include <ranges>       // for input_range
#include <string>       // for string, operator+
#include <type_traits>  // for underlying_type_t
#include <vector>       // for vector

#pragma warning(push)
#pragma warning(disable : 6285)
#pragma warning(pop)
#include <range/v3/view/subrange.hpp>  // for subrange

#include <multibase/basic_algorithm.hpp>  // for basic_algorithm
#include <multibase/encoding.hpp>         // for encoding, encoding::base_10

namespace multibase {

template <std::ranges::input_range range>
std::string encode(const range& input, encoding base, bool multiformat = true);

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, encoding base,
                bool multiformat = true);

template <typename codec, std::ranges::input_range range,
          std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, bool multiformat);

char encode(encoding base);

encoding decode(char byte);

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input);

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input, encoding base);

template <typename codec, std::ranges::input_range range,
          std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output);

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output);

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output, encoding base);

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

template <std::ranges::input_range range>
std::string encode(const range& input, encoding base, bool multiformat) {
  std::string buffer;
  auto output = std::back_inserter(buffer);
  encode(input, output, base, multiformat);
  return buffer;
}

// Note that the null terminator on char* strings will also be included.
// To avoid this, explicitly create a string view
template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, encoding base,
                bool multiformat) {
  switch (base) {
    using enum multibase::encoding;
    case base_none:
      if (multiformat) {
        *output++ = encode(base);
      }
      std::ranges::copy(input, output);
      break;
    case base_2:
      encode<multibase::base_2>(input, output, multiformat);
      break;
    case base_8:
      encode<multibase::base_8>(input, output, multiformat);
      break;
    case base_10:
      encode<multibase::base_10>(input, output, multiformat);
      break;
    case base_16:
      encode<multibase::base_16>(input, output, multiformat);
      break;
    case base_16_upper:
      encode<multibase::base_16_upper>(input, output, multiformat);
      break;
    case base_32:
      encode<multibase::base_32>(input, output, multiformat);
      break;
    case base_32_upper:
      encode<multibase::base_32_upper>(input, output, multiformat);
      break;
    case base_32_pad:
      encode<multibase::base_32_pad>(input, output, multiformat);
      break;
    case base_32_pad_upper:
      encode<multibase::base_32_pad_upper>(input, output, multiformat);
      break;
    case base_32_hex:
      encode<multibase::base_32_hex>(input, output, multiformat);
      break;
    case base_32_hex_upper:
      encode<multibase::base_32_hex_upper>(input, output, multiformat);
      break;
    case base_32_hex_pad:
      encode<multibase::base_32_hex_pad>(input, output, multiformat);
      break;
    case base_32_hex_pad_upper:
      encode<multibase::base_32_hex_pad_upper>(input, output, multiformat);
      break;
    case base_32_z:
      encode<multibase::base_32_z>(input, output, multiformat);
      break;
    case base_36:
      encode<multibase::base_36>(input, output, multiformat);
      break;
    case base_36_upper:
      encode<multibase::base_36_upper>(input, output, multiformat);
      break;
    case base_58_flickr:
      encode<multibase::base_58_flickr>(input, output, multiformat);
      break;
    case base_58_btc:
      encode<multibase::base_58_btc>(input, output, multiformat);
      break;
    case base_64:
      encode<multibase::base_64>(input, output, multiformat);
      break;
    case base_64_pad:
      encode<multibase::base_64_pad>(input, output, multiformat);
      break;
    case base_64_url:
      encode<multibase::base_64_url>(input, output, multiformat);
      break;
    case base_64_url_pad:
      encode<multibase::base_64_url_pad>(input, output, multiformat);
      break;
  }
  return output;
}

template <typename codec, std::ranges::input_range range,
          std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, bool multiformat) {
  if (multiformat) {
    *output++ = static_cast<std::underlying_type_t<multibase::encoding>>(
        codec::encoding);
  }
  return codec::encode(input, output);
}

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input) {
  std::vector<unsigned char> buffer;
  auto output = std::back_inserter(buffer);
  decode(input, output);
  return buffer;
}

template <std::ranges::input_range range>
std::vector<unsigned char> decode(const range& input, encoding base) {
  std::vector<unsigned char> buffer;
  auto output = std::back_inserter(buffer);
  decode(input, output, base);
  return buffer;
}

template <typename codec, std::ranges::input_range range,
          std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output) {
  return codec::decode(input, output);
}

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output) {
  auto first = std::begin(input);
  auto last = std::end(input);
  auto base = decode(static_cast<char>(*first++));
  return decode(ranges::subrange(first, last), output, base);
}

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt decode(const range& input, OutputIt output, encoding base) {
  switch (base) {
    using enum multibase::encoding;
    case base_none: {
      std::ranges::transform(input, output,
                             [](unsigned char chr) { return chr; });
      break;
      case base_2:
        decode<multibase::base_2>(input, output);
        break;
      case base_8:
        decode<multibase::base_8>(input, output);
        break;
      case base_10:
        decode<multibase::base_10>(input, output);
        break;
      case base_16:
        decode<multibase::base_16>(input, output);
        break;
      case base_16_upper:
        decode<multibase::base_16_upper>(input, output);
        break;
      case base_32:
        decode<multibase::base_32>(input, output);
        break;
      case base_32_upper:
        decode<multibase::base_32_upper>(input, output);
        break;
      case base_32_hex:
        decode<multibase::base_32_hex>(input, output);
        break;
      case base_32_hex_pad:
        decode<multibase::base_32_hex_pad>(input, output);
        break;
      case base_32_hex_pad_upper:
        decode<multibase::base_32_hex_pad_upper>(input, output);
        break;
      case base_32_pad:
        decode<multibase::base_32_pad>(input, output);
        break;
      case base_32_pad_upper:
        decode<multibase::base_32_pad_upper>(input, output);
        break;
      case base_32_hex_upper:
        decode<multibase::base_32_hex_upper>(input, output);
        break;
      case base_32_z:
        decode<multibase::base_32_z>(input, output);
        break;
      case base_36:
        decode<multibase::base_36>(input, output);
        break;
      case base_36_upper:
        decode<multibase::base_36_upper>(input, output);
        break;
      case base_58_flickr:
        decode<multibase::base_58_flickr>(input, output);
        break;
      case base_58_btc:
        decode<multibase::base_58_btc>(input, output);
        break;
      case base_64:
        decode<multibase::base_64>(input, output);
        break;
      case base_64_pad:
        decode<multibase::base_64_pad>(input, output);
        break;
      case base_64_url:
        decode<multibase::base_64_url>(input, output);
        break;
      case base_64_url_pad:
        decode<multibase::base_64_url_pad>(input, output);
        break;
    }
  }
  return output;
}

}  // namespace multibase

#endif
