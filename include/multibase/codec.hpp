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

#include <multibase/base_none.hpp>
#include <multibase/basic_algorithm.hpp>  // for basic_algorithm
#include <multibase/encoding.hpp>         // for encoding, encoding::base_10

namespace multibase {

/// Encapsulates the stateless functions required to perform base conversion.
///
class codec {
 public:
  explicit codec(encoding base);

  template <std::ranges::input_range range>
  std::size_t encoded_size(const range& chunk);
  std::size_t encoded_size(std::size_t len);
  char encode(std::byte byte);
  std::string_view encode(std::span<const std::byte> input,
                          std::span<char> output);

  template <std::ranges::input_range range>
  std::size_t decoded_size(const range& chunk);
  std::size_t decoded_size(std::size_t len);
  std::byte decode(char chr);
  std::span<std::byte> decode(std::string_view input,
                              std::span<std::byte> output);
  [[nodiscard]] std::optional<std::size_t> encoded_chunk_size() const;
  [[nodiscard]] std::optional<std::size_t> decoded_chunk_size() const;

 private:
  std::size_t (*encoded_size_)(std::size_t len){nullptr};
  std::string_view (*encode_)(std::span<const std::byte>,
                              std::span<char>){nullptr};
  char (*encode_byte_)(std::byte){nullptr};
  std::size_t (*decoded_size_)(std::size_t len){nullptr};
  std::span<std::byte> (*decode_)(std::string_view,
                                  std::span<std::byte>){nullptr};
  std::byte (*decode_byte_)(char chr){nullptr};
  std::optional<std::size_t> (*encoded_chunk_size_)(){nullptr};
  std::optional<std::size_t> (*decoded_chunk_size_)(){nullptr};

  template <typename impl>
  constexpr void init();

  template <std::ranges::input_range range>
  std::size_t count_leading_zeros(const range& chunk);
};

template <std::ranges::input_range range>
std::size_t codec::encoded_size(const range& chunk) {
  return encoded_size(std::size(chunk));
}

template <std::ranges::input_range range>
std::string encode(const range& input, encoding base, bool multiformat = true);

template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, encoding base,
                bool multiformat = true);

char encode(encoding base);

template <std::ranges::input_range range>
std::size_t codec::decoded_size(const range& chunk) {
  if (decoded_chunk_size()) {
    return decoded_size(std::size(chunk));
  }
  auto zeros = count_leading_zeros(chunk);
  return zeros + decoded_size(std::size(chunk) - zeros);
}
encoding decode(char byte);

template <std::ranges::input_range range>
std::vector<std::byte> decode(const range& input);

template <std::ranges::input_range range>
std::vector<std::byte> decode(const range& input, encoding base);

template <std::ranges::input_range range,
          std::output_iterator<std::byte> OutputIt>
OutputIt decode(const range& input, OutputIt output);

template <std::ranges::input_range range,
          std::output_iterator<std::byte> OutputIt>
OutputIt decode(const range& input, OutputIt output, encoding base);

using base_none = base_none;
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
  auto encoder = codec{base};
  auto output = std::string{};
  if constexpr (ranges::sized_range<range>) {
    auto size = encoder.encoded_size(input);
    auto offset = std::size_t{0};
    if (multiformat) {
      output.push_back(encode(base));
      offset = 1;
    }
    output.resize(offset + size);
    auto view = encoder.encode(std::as_bytes(std::span{input}),
                               std::span{&output[offset], size});
    std::ranges::copy(view,
                      std::next(output.begin(), static_cast<int>(offset)));
    output.resize(view.size() + offset);
  } else {
    encode(input, std::back_inserter(output), base, multiformat);
  }
  return output;
}

// Note that the null terminator on char* strings will also be included.
// To avoid this, explicitly create a string view
template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt encode(const range& input, OutputIt output, encoding base,
                bool multiformat) {
  auto encoder = codec{base};
  auto encoded_chunk_size = encoder.encoded_chunk_size();
  auto decoded_chunk_size = encoder.decoded_chunk_size();
  if (!encoded_chunk_size || !decoded_chunk_size) {
    // can't chunk and don't know size of output
    std::string output_buffer;
    if constexpr (ranges::sized_range<range>) {
      output_buffer = encode(input, base, multiformat);
    } else {
      using input_type = std::ranges::range_value_t<range>;
      const std::vector<input_type> input_buffer{std::begin(input),
                                                 std::end(input)};
      output_buffer = encode(input_buffer, base, multiformat);
    }
    return std::ranges::copy(output_buffer, output).out;
  }
  if (multiformat) {
    *output++ = encode(base);
  }
  auto chunk_output = std::vector<char>(*encoded_chunk_size);
  auto chunk_input = std::vector<std::byte>(*decoded_chunk_size);
  auto first = std::begin(input);
  auto last = std::end(input);
  auto first2 = chunk_input.begin();
  auto last2 = chunk_input.end();
  while (first != last) {
    if (first2 != last2) {
      *first2++ = static_cast<std::byte>(*first++);  // fill chunk_input
    } else {
      encoder.encode(chunk_input, chunk_output);
      std::ranges::copy_if(chunk_output, output,
                           [](auto val) { return val != 0; });
      std::ranges::fill(chunk_input, std::byte{0});
      first2 = chunk_input.begin();
    }
  }
  // process incomplete end chunk
  auto input_span = std::span{
      chunk_input.data(),
      static_cast<std::size_t>(std::distance(chunk_input.begin(), first2))};
  auto output_span = encoder.encode(input_span, chunk_output);
  std::ranges::copy(output_span, output);
  return output;
}

template <std::ranges::input_range range>
std::vector<std::byte> decode(const range& input) {
  auto first = input.begin();
  auto base = decode(static_cast<char>(*first++));
  return decode(ranges::subrange{first, input.end()}, base);
}

template <std::ranges::input_range range>
std::vector<std::byte> decode(const range& input, encoding base) {
  std::vector<std::byte> buffer;
  auto decoder = codec{base};
  if constexpr (ranges::sized_range<range>) {
    buffer.resize(decoder.decoded_size(input));
    const auto* data = static_cast<const char*>(input.data());
    auto size = input.size() * sizeof(std::ranges::range_value_t<range>);
    auto view = decoder.decode(std::string_view{data, size}, std::span{buffer});
    std::ranges::copy(view, buffer.begin());
    buffer.resize(view.size());
  } else {
    decoder.decode(input, std::back_inserter(buffer), base);
  }
  return buffer;
}

template <std::ranges::input_range range,
          std::output_iterator<std::byte> OutputIt>
OutputIt decode(const range& input, OutputIt output) {
  auto first = std::begin(input);
  auto last = std::end(input);
  auto base = decode(static_cast<char>(*first++));
  return decode(ranges::subrange(first, last), output, base);
}

template <std::ranges::input_range range,
          std::output_iterator<std::byte> OutputIt>
OutputIt decode(const range& input, OutputIt output, encoding base) {
  auto decoder = codec{base};
  auto encoded_chunk_size = decoder.encoded_chunk_size();
  auto decoded_chunk_size = decoder.decoded_chunk_size();
  if (!encoded_chunk_size || !decoded_chunk_size) {
    const std::string input_buffer{std::begin(input), std::end(input)};
    std::vector<std::byte> output_buffer(decoder.decoded_size(input_buffer),
                                         std::byte{0});
    const std::span<std::byte> output_span =
        decoder.decode(input_buffer, output_buffer);
    return std::ranges::copy(output_span, output).out;
  }
  auto chunk_output = std::vector<std::byte>(*decoded_chunk_size);
  auto chunk_input = std::vector<char>(*encoded_chunk_size);
  auto first = std::begin(input);
  auto last = std::end(input);
  auto first2 = chunk_input.begin();
  auto last2 = chunk_input.end();
  while (first != last) {
    if (first2 == last2) {
      decoder.decode(std::string_view{chunk_input.data(), chunk_input.size()},
                     chunk_output);
      std::ranges::copy(chunk_output, output);
      first2 = chunk_input.begin();
    } else {
      *first2++ = static_cast<char>(*first++);
    }
  }
  auto output_view = decoder.decode(
      std::string_view{
          chunk_input.data(),
          static_cast<std::size_t>(std::distance(chunk_input.begin(), first2))},
      std::span<std::byte>{chunk_output.data(), chunk_output.size()});
  std::ranges::copy(output_view, output);
  return output;
}

template <typename impl>
constexpr void codec::init() {
  encoded_size_ = &impl::encoded_size;
  encode_ = &impl::encode;
  encode_byte_ = &impl::encode;
  encoded_chunk_size_ = &impl::encoded_chunk_size;
  decoded_size_ = &impl::decoded_size;
  decode_ = &impl::decode;
  decode_byte_ = &impl::decode;
  decoded_chunk_size_ = &impl::decoded_chunk_size;
}

template <std::ranges::input_range range>
std::size_t codec::count_leading_zeros(const range& chunk) {
  return static_cast<std::size_t>(std::distance(
      std::begin(chunk), std::ranges::find_if(chunk, [this](auto c) {
        return this->decode(c) != std::byte{0};
      })));
}
}  // namespace multibase

#endif
