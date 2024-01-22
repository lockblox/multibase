#ifndef MULTIBASE_BASIC_ALGORITHM_HPP
#define MULTIBASE_BASIC_ALGORITHM_HPP

#include <algorithm>    // for min, copy, fill
#include <array>        // for array
#include <cctype>       // for tolower, toupper
#include <cmath>        // for log, ceil
#include <cstddef>      // for size_t
#include <cstdint>      // for int64_t
#include <iterator>     // for size, begin, distance, outp...
#include <limits>       // for numeric_limits
#include <memory>       // for to_address
#include <ranges>       // for input_range, find_if, find
#include <ratio>        // for ratio
#include <span>         // for span
#include <stdexcept>    // for invalid_argument
#include <string>       // for string
#include <string_view>  // for string_view
#include <vector>       // for vector

#include <fmt/core.h>                   // for format
#include <range/v3/range/concepts.hpp>  // for sized_range
#include <range/v3/view/reverse.hpp>    // for reverse_view

#include "multibase/encoding_case.hpp"    // for encoding_case
#include "multibase/encoding_traits.hpp"  // for encoding_traits
#include "multibase/log.hpp"              // for log2
#include "multibase/portability.hpp"      // for MULTIBASE_CONSTEVAL

namespace multibase {

enum class encoding : char;

template <encoding T, typename Traits = encoding_traits<T>>
class basic_algorithm {
 public:
  static constexpr multibase::encoding encoding{T};

  template <std::ranges::input_range range>
  static std::size_t encoded_size(const range& chunk);

  static constexpr std::size_t encoded_size(std::size_t len);

  template <std::ranges::input_range InputRange,
            std::output_iterator<char> OutputIt>
  static OutputIt encode(const InputRange& input, OutputIt output);

  template <std::ranges::input_range range>
  static std::string encode(const range& input);

  template <std::ranges::input_range range>
  static std::string_view encode(const range& chunk, std::span<char> output);

  template <std::ranges::input_range range>
  static std::size_t decoded_size(const range& chunk);

  static constexpr std::size_t decoded_size(std::size_t len);

  static constexpr int decode(char c);

  template <std::ranges::input_range range>
  static std::span<unsigned char> decode(const range& chunk,
                                         std::span<unsigned char> output);

  template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
  static OutputIt decode(const range& input, OutputIt output);

  template <std::ranges::input_range range>
  static std::string decode(const range& input);

 private:
  using CharsetT = decltype(Traits::alphabet);
  using value_type = typename CharsetT::value_type;
  using iterator = typename CharsetT::const_iterator;

  template <std::ranges::input_range range>
  static std::size_t count_leading_zeros(const range& chunk);

  MULTIBASE_CONSTEVAL static bool is_chunkable();

  constexpr static iterator find(const value_type& val) noexcept;

  /** Determine the character encoding for a given value
  @return character encoding, or 0 if none such encoding exists */
  constexpr static unsigned char getval(char val) noexcept;

  /** encoding as determined by size of character set */
  constexpr static auto radix =
      static_cast<int>(sizeof(Traits::alphabet) / sizeof(value_type));
  /** Ratio of bits per unencoded value to bits per encoded value */
  constexpr static auto ratio = std::ratio<log2(256), log2(radix)>{};
  constexpr static auto encoded_chunk_size = ratio.num;
  constexpr static auto decoded_chunk_size = ratio.den;
  constexpr static auto byte_max = 256;
  constexpr static auto invalid_value =
      std::numeric_limits<unsigned char>::max();
  constexpr static auto log256 = 5.545177444479562;

  /** Map from value to corresponding character in base encoding */
  constexpr static std::array<unsigned char,
                              std::numeric_limits<char>::max() + 1>
      valset{getval(0),   getval(1),   getval(2),   getval(3),   getval(4),
             getval(5),   getval(6),   getval(7),   getval(8),   getval(9),
             getval(10),  getval(11),  getval(12),  getval(13),  getval(14),
             getval(15),  getval(16),  getval(17),  getval(18),  getval(19),
             getval(20),  getval(21),  getval(22),  getval(23),  getval(24),
             getval(25),  getval(26),  getval(27),  getval(28),  getval(29),
             getval(30),  getval(31),  getval(32),  getval(33),  getval(34),
             getval(35),  getval(36),  getval(37),  getval(38),  getval(39),
             getval(40),  getval(41),  getval(42),  getval(43),  getval(44),
             getval(45),  getval(46),  getval(47),  getval(48),  getval(49),
             getval(50),  getval(51),  getval(52),  getval(53),  getval(54),
             getval(55),  getval(56),  getval(57),  getval(58),  getval(59),
             getval(60),  getval(61),  getval(62),  getval(63),  getval(64),
             getval(65),  getval(66),  getval(67),  getval(68),  getval(69),
             getval(70),  getval(71),  getval(72),  getval(73),  getval(74),
             getval(75),  getval(76),  getval(77),  getval(78),  getval(79),
             getval(80),  getval(81),  getval(82),  getval(83),  getval(84),
             getval(85),  getval(86),  getval(87),  getval(88),  getval(89),
             getval(90),  getval(91),  getval(92),  getval(93),  getval(94),
             getval(95),  getval(96),  getval(97),  getval(98),  getval(99),
             getval(100), getval(101), getval(102), getval(103), getval(104),
             getval(105), getval(106), getval(107), getval(108), getval(109),
             getval(110), getval(111), getval(112), getval(113), getval(114),
             getval(115), getval(116), getval(117), getval(118), getval(119),
             getval(120), getval(121), getval(122), getval(123), getval(124),
             getval(125), getval(126), getval(127)};
};

// IMPLEMENTATION
template <encoding T, typename Traits>
template <std::ranges::input_range InputRange,
          std::output_iterator<char> OutputIt>
OutputIt basic_algorithm<T, Traits>::encode(const InputRange& input,
                                            OutputIt output) {
  if constexpr (!is_chunkable()) {
    const std::vector<unsigned char> input_buffer{std::begin(input),
                                                  std::end(input)};
    std::string output_buffer(encoded_size(input_buffer), 0);
    const std::string_view output_span = encode(input_buffer, output_buffer);
    output = std::ranges::copy(output_span, output).out;
    return output;
  }
  auto chunk_output = std::array<char, encoded_chunk_size>{};
  auto chunk_input = std::array<unsigned char, decoded_chunk_size>{};
  auto first = std::begin(input);
  auto last = std::end(input);
  auto first2 = chunk_input.begin();
  auto last2 = chunk_input.end();
  while (first != last) {
    if (first2 == last2) {
      encode(chunk_input, chunk_output);
      std::ranges::copy_if(chunk_output, output,
                           [](auto val) { return val != 0; });
      chunk_input.fill(0);
      first2 = chunk_input.begin();
    } else {
      *first2++ = static_cast<unsigned char>(*first++);
    }
  }
  auto input_span = std::span<unsigned char>{
      chunk_input.data(),
      static_cast<std::size_t>(std::distance(chunk_input.begin(), first2))};
  auto output_span = encode(input_span, chunk_output);
  std::ranges::copy(output_span, output);
  return output;
}

template <encoding T, typename Traits>
constexpr std::size_t basic_algorithm<T, Traits>::encoded_size(
    std::size_t len) {
  if constexpr (is_chunkable()) {
    auto blocks = std::ceil(static_cast<float>(len) / decoded_chunk_size);
    auto length = encoded_chunk_size * blocks;
    return static_cast<std::size_t>(length);
  } else {
    return static_cast<std::size_t>(static_cast<double>(len) * log256 /
                                    std::log(radix)) +
           1;
  }
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::size_t basic_algorithm<T, Traits>::count_leading_zeros(
    const range& chunk) {
  return static_cast<std::size_t>(std::distance(
      std::begin(chunk),
      std::ranges::find_if(chunk, [](auto c) { return decode(c) != 0; })));
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::size_t basic_algorithm<T, Traits>::decoded_size(const range& chunk) {
  if constexpr (is_chunkable()) {
    return decoded_size(std::size(chunk));
  } else {
    auto zeros = count_leading_zeros(chunk);
    return zeros + decoded_size(std::size(chunk) - zeros);
  }
}

template <encoding T, typename Traits>
constexpr std::size_t basic_algorithm<T, Traits>::decoded_size(
    std::size_t len) {
  if constexpr (is_chunkable()) {
    return static_cast<std::size_t>(
        decoded_chunk_size *
        std::ceil(static_cast<double>(len) / encoded_chunk_size));
  } else {
    // need to include number of zeros here
    return static_cast<std::size_t>(
               std::ceil(static_cast<double>(len) * std::log(radix) / log256)) +
           1;
  }
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::string basic_algorithm<T, Traits>::encode(const range& input) {
  auto output = std::string{};
  if constexpr (ranges::sized_range<range>) {
    output.resize(encoded_size(input));
    auto view = encode(input, std::span{output.data(), output.size()});
    std::ranges::copy(view, std::begin(output));
    output.resize(std::size(view));
  } else {
    encode(input, std::back_inserter(output));
  }
  return output;
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::size_t basic_algorithm<T, Traits>::encoded_size(const range& chunk) {
  return encoded_size(std::size(chunk));
}

/// Real underlying encoding routine which operates on either a chunk or a
/// full range
/// @param output Buffer in which to write the output. We inevitably need a
/// buffer so this cannot be an iterator
template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::string_view basic_algorithm<T, Traits>::encode(const range& chunk,
                                                    std::span<char> output) {
  std::ranges::fill(output, static_cast<char>(0));
  auto input_size = std::size(chunk);
  auto partial_blocks =
      static_cast<float>(std::size(chunk)) / decoded_chunk_size;
  auto unpadded_size = std::min(
      std::size(output),
      static_cast<std::size_t>(std::ceil(partial_blocks * encoded_chunk_size)));
  if constexpr (is_chunkable()) {
    input_size = static_cast<std::size_t>(std::ceil(partial_blocks) *
                                          decoded_chunk_size);
  }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
  auto elem = std::begin(chunk);
#pragma clang diagnostic pop
  auto len = std::size_t{0};
  // zero can be represented by a single 0 value in all bases
  // this means we can count and prepend
  auto leading_zeroes = std::size_t{0};
  auto all_zeroes = true;
  for (std::size_t i = 0; i < input_size; ++i) {
    int carry = 0;
    if (elem != std::end(chunk)) {
      if (*elem != 0) {
        all_zeroes = false;
      } else if (all_zeroes) {
        ++leading_zeroes;
        ++elem;
        continue;
      }
      carry = static_cast<unsigned char>(*elem++);
    }
    auto j = std::size_t{0};
    [[maybe_unused]] auto _ =
        std::ranges::find_if(ranges::reverse_view(output), [&](auto& out_val) {
          carry += byte_max * out_val;
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          auto* byte = reinterpret_cast<unsigned char*>(&out_val);
          *byte = static_cast<unsigned char>(carry % radix);
          carry /= radix;
          auto halt = carry == 0 && j >= len;
          ++j;
          return halt;
        });
    len = j;
  }
  std::ranges::transform(
      output, std::begin(output),
      [unpadded_size, output_size = std::size_t{0}](std::size_t c) mutable {
        return output_size++ < unpadded_size ? Traits::alphabet.at(c)
                                             : Traits::padding;
      });
  auto data = output.begin();
  auto offset = std::size_t{0};
  if constexpr (!is_chunkable()) {
    offset = output.size() - std::min(output.size(), len);
    std::advance(data, offset - std::min(offset, leading_zeroes));
    len = unpadded_size - offset + leading_zeroes;
  } else if constexpr (Traits::padding == 0) {
    len = unpadded_size - offset;
  } else {
    len = output.size() - offset;
  }
  len = std::min(static_cast<std::size_t>(std::distance(data, output.end())),
                 len);
  return std::string_view{std::to_address(data), len};
}

template <encoding T, typename Traits>
constexpr int basic_algorithm<T, Traits>::decode(char ch) {
  if (ch == Traits::padding) {
    return 0;
  }
  if constexpr (!Traits::is_case_sensitive) {
    switch (Traits::type_case) {
      using enum multibase::encoding_case;
      case lower:
        ch = static_cast<char>(std::tolower(ch));
        break;
      case upper:
        ch = static_cast<char>(std::toupper(ch));
        break;
      case both:
      case none:
        break;
    }
  }
  const int val = valset.at(static_cast<std::size_t>(ch));
  if (val == invalid_value) {
    throw std::invalid_argument{fmt::format("Invalid input character {}", ch)};
  }
  return val;
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::span<unsigned char> basic_algorithm<T, Traits>::decode(
    const range& chunk, std::span<unsigned char> output) {
  std::ranges::fill(output, static_cast<unsigned char>(0));
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
  auto first = std::begin(chunk);
#pragma clang diagnostic pop
  auto last = std::end(chunk);
  std::size_t length = 0;
  std::size_t leading_zeroes = 0;
  std::size_t non_zeroes = 0;
  [[maybe_unused]] const int dcz = decoded_chunk_size;
  [[maybe_unused]] const int ecs = encoded_chunk_size;
  auto padding = std::size_t{0};
  auto input_size =
      is_chunkable() ? encoded_size(output.size()) : std::size(chunk);
  for (std::size_t i = 0; i < input_size; ++i) {
    int carry = 0;
    if (first != last) {
      padding += *first == Traits::padding;
      carry = decode(*first++);
      if (carry != 0) {
        ++non_zeroes;
      } else if (non_zeroes == 0) {
        ++leading_zeroes;
        continue;
      }
    }
    std::size_t j = 0;
    for (auto rfirst = output.rbegin(), rlast = output.rend();
         rfirst != rlast && (carry != 0 || j < length); ++rfirst, ++j) {
      carry += static_cast<int>(radix) * (*rfirst);
      *rfirst = static_cast<unsigned char>(carry % byte_max);
      carry /= byte_max;
    }
    length = j;
  }
  auto non_zero =
      std::ranges::find_if(output, [](auto chr) { return chr != 0; });
  [[maybe_unused]] auto output_size =
      is_chunkable() ? static_cast<std::size_t>(
                           static_cast<double>(std::size(chunk) - padding) /
                           encoded_chunk_size * decoded_chunk_size)
                     : std::min(length + leading_zeroes, output.size());
  auto offset = static_cast<std::int64_t>(leading_zeroes);
  std::advance(non_zero,
               -1 * std::min(std::distance(output.begin(), non_zero), offset));
  auto result = std::span{std::to_address(non_zero), output_size};
  return result;
}

template <encoding T, typename Traits>
template <std::ranges::input_range range, std::output_iterator<char> OutputIt>
OutputIt basic_algorithm<T, Traits>::decode(const range& input,
                                            OutputIt output) {
  if constexpr (!is_chunkable()) {
    const std::string input_buffer{std::begin(input), std::end(input)};
    std::vector<unsigned char> output_buffer(decoded_size(input_buffer), 0);
    const std::span<unsigned char> output_span =
        decode(input_buffer, output_buffer);
    output = std::ranges::copy(output_span, output).out;
    return output;
  }
  auto chunk_output = std::array<unsigned char, decoded_chunk_size>{};
  auto chunk_input = std::array<char, encoded_chunk_size>{};
  auto first = std::begin(input);
  auto last = std::end(input);
  auto first2 = chunk_input.begin();
  auto last2 = chunk_input.end();
  while (first != last) {
    if (first2 == last2) {
      decode(chunk_input, chunk_output);
      std::ranges::copy(chunk_output, output);
      first2 = chunk_input.begin();
    } else {
      *first2++ = static_cast<char>(*first++);
    }
  }
  auto input_span = std::span<char>{
      chunk_input.data(),
      static_cast<std::size_t>(std::distance(chunk_input.begin(), first2))};
  auto output_span = decode(input_span, chunk_output);
  std::ranges::copy(output_span, output);
  return output;
}

template <encoding T, typename Traits>
template <std::ranges::input_range range>
std::string basic_algorithm<T, Traits>::decode(const range& input) {
  auto output = std::string(decoded_size(input), 0);
  auto view = decode(input, std::span{output.data(), output.size()});
  std::ranges::copy(view, std::begin(output));
  output.resize(std::size(view));
  return output;
}

template <encoding T, typename Traits>
MULTIBASE_CONSTEVAL bool basic_algorithm<T, Traits>::is_chunkable() {
  for (auto i = radix; i > 1; i /= 2) {
    if ((i / 2) * 2 != i) {
      return false;
    }
  }
  return true;
}

template <encoding T, typename Traits>
constexpr typename basic_algorithm<T, Traits>::iterator
basic_algorithm<T, Traits>::find(const value_type& val) noexcept {
  return std::ranges::find(std::cbegin(Traits::alphabet),
                           std::cend(Traits::alphabet), val);
}

/** Determine the character encoding for a given value
  @return character encoding, or 0 if none such encoding exists */
template <encoding T, typename Traits>
constexpr unsigned char basic_algorithm<T, Traits>::getval(char val) noexcept {
  return find(val) == std::cend(Traits::alphabet)
             ? invalid_value
             : static_cast<unsigned char>(
                   std::distance(std::cbegin(Traits::alphabet), find(val)));
}
}  // namespace multibase

#endif
