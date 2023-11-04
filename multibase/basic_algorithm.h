#pragma once

#include <multibase/algorithm.h>
#include <multibase/encoding.h>
#include <multibase/log.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <gsl/narrow>
#include <iterator>
#include <multibase/encoding_traits.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <ranges>
#include <ratio>
#include <vector>

namespace multibase {

template <encoding T, typename Traits = encoding_traits<T>>
class basic_algorithm {
 public:
  static constexpr multibase::encoding encoding{T};

  template <std::ranges::input_range InputRange,
            std::output_iterator<char> OutputIt>
  static OutputIt encode(const InputRange& input, OutputIt output) {
    static_assert(is_chunkable());
    auto chunk_output = std::array<char, encoded_chunk_size>{};
    auto func = [&](auto chunk) { return encode(chunk, chunk_output); };
    auto output_view = input | ranges::views::chunk(decoded_chunk_size) |
                       ranges::views::transform(func) | ranges::views::join;
    std::ranges::copy_if(output_view, output,
                         [](auto val) { return val != 0; });
    return output;
  }

  // TODO make non-random access ranges a special case rather than the other way
  // round

  // required to enforce a non chunked encoding for c strings
  static std::string encode(const unsigned char* input, std::size_t len) {
    return encode(ranges::subrange(input, std::next(input, len)));
  }

  template <std::input_iterator FirstIt, std::input_iterator LastIt,
            std::output_iterator<char> OutputIt>
  static OutputIt encode(FirstIt first, LastIt last, OutputIt output) {
    return encode(ranges::subrange(first, last), output);
  }

  template <std::output_iterator<char> OutputIt>
  static OutputIt encode(const char* input, OutputIt output) {
    auto len = strlen(input);
    return encode(ranges::subrange(input, std::next(input, len)), output);
  }

  template <std::ranges::input_range range>
  static std::size_t encoded_size(const range& chunk) {
    return encoded_size(std::size(chunk));
  }

  static constexpr std::size_t encoded_size(std::size_t len) {
    if constexpr (is_chunkable()) {
      auto blocks = std::ceil(static_cast<float>(len) / decoded_chunk_size);
      auto length = encoded_chunk_size * blocks;
      return static_cast<std::size_t>(length);
    } else {
      return static_cast<std::size_t>(len * std::log(256.0) / std::log(radix)) +
             1;
    }
  }

  template <std::ranges::input_range range>
  static std::size_t count_leading_zeros(const range& chunk) {
    return static_cast<std::size_t>(std::distance(
        std::begin(chunk),
        std::ranges::find_if(chunk, [](auto ch) { return decode(ch) != 0; })));
  }

  template <std::ranges::input_range range>
  static std::size_t decoded_size(const range& chunk) {
    if constexpr (is_chunkable()) {
      return decoded_size(std::size(chunk));
    }
    else {
      auto zeros = count_leading_zeros(chunk);
      return zeros + decoded_size(std::size(chunk) - zeros);
    }
  }

  static constexpr std::size_t decoded_size(std::size_t len) {
    if constexpr (is_chunkable()) {
      return static_cast<std::size_t>(
          decoded_chunk_size * std::ceil(1.0 * len / encoded_chunk_size));
    } else {
      // need to include number of zeros here
      return static_cast<std::size_t>(
                 std::ceil(len * std::log(radix) / std::log(256.0))) +
             1;
    }
  }

  template <std::ranges::input_range range>
  static std::string encode(const range& input) {
    auto output = std::string(encoded_size(input), 0);
    auto view = encode(input, std::span{output.data(), output.size()});
    std::ranges::copy(view, std::begin(output));
    output.resize(std::size(view));
    return output;
  }

  /// Real underlying encoding routine which operates on either a chunk or a
  /// full range
  /// @param output Buffer in which to write the output. We inevitably need a
  /// buffer so this cannot be an iterator
  template <std::ranges::input_range range>
  static std::string_view encode(const range& chunk, std::span<char> output) {
    std::ranges::fill(output, static_cast<char>(0));
    auto input_size = std::size(chunk);
    auto partial_blocks =
        static_cast<float>(std::size(chunk)) / decoded_chunk_size;
    auto unpadded_size = std::min(
        std::size(output), static_cast<std::size_t>(
                               std::ceil(partial_blocks * encoded_chunk_size)));
    if constexpr (is_chunkable()) {
      input_size =
          static_cast<int>(std::ceil(partial_blocks) * decoded_chunk_size);
    }
    auto elem = std::begin(chunk);
    auto len = std::size_t{0};
    // zero can be represented by a single 0 value in all bases
    // this means we can count and prepend
    auto leading_zeroes = std::size_t{0};
    auto all_zeroes = true;
    for (int i = 0; i < input_size; ++i) {
      int carry = 0;
      if (elem != std::end(chunk)) {
        if (*elem != 0) {
          all_zeroes = false;
        } else if (all_zeroes) {
          ++leading_zeroes;
          ++elem;
          continue;
        }
        carry = *elem++;
      }
      int j = 0;
      [[maybe_unused]] auto _ =
          std::ranges::find_if(std::views::reverse(output), [&](auto& out_val) {
            carry += 256 * out_val;
            auto byte = (unsigned char*)(&out_val);
            *byte = carry % radix;
            carry /= radix;
            auto halt = carry == 0 && j >= len;
            ++j;
            return halt;
          });
      len = j;
    }
    std::ranges::transform(output, std::begin(output),
                           [unpadded_size, output_size = 0](auto c) mutable {
                             return output_size++ < unpadded_size
                                        ? Traits::alphabet[c]
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

  static constexpr int decode(char ch) {
    if (ch == Traits::padding) {
      return 0;
    }
    if constexpr (!Traits::is_case_sensitive) {
      switch (Traits::encoding_case) {
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
    int val = valset[ch];
    if (val == 255) {
      throw std::invalid_argument{
          std::format("Invalid input character {}", ch)};
    }
    return val;
  }

  template <std::ranges::input_range range>
  static std::span<unsigned char> decode(const range& chunk,
                                         std::span<unsigned char> output) {
    auto first = std::begin(chunk);
    auto last = std::end(chunk);
    std::size_t length = 0;
    std::size_t leading_zeroes = 0;
    std::size_t non_zeroes = 0;
    [[maybe_unused]] int dcz = decoded_chunk_size;
    [[maybe_unused]] int ecs = encoded_chunk_size;
    auto padding = std::size_t{0};
    auto input_size =
        is_chunkable() ? encoded_size(output.size()) : std::size(chunk);
    for (int i = 0; i < input_size; ++i) {
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
      auto j = 0;
      for (auto rfirst = output.rbegin(), rlast = output.rend();
           rfirst != rlast && (carry != 0 || j < length); ++rfirst, ++j) {
        carry += radix * (*rfirst);
        *rfirst = static_cast<unsigned char>(carry % 256);
        carry /= 256;
      }
      assert(carry == 0);
      length = j;
    }
    auto non_zero =
        std::ranges::find_if(output, [](auto ch) { return ch != 0; });
    [[maybe_unused]] auto output_size =
        is_chunkable()
            ? static_cast<std::size_t>(1.0 * (std::size(chunk) - padding) /
                                       encoded_chunk_size * decoded_chunk_size)
            : std::min(length + leading_zeroes, output.size());
    auto offset = static_cast<std::int64_t>(leading_zeroes);
    std::advance(
        non_zero,
        -1 * std::min(std::distance(output.begin(), non_zero), offset));
    auto result = std::span{std::to_address(non_zero), output_size};
    return result;
  }

  /** Decoders */
  /*
    template <std::ranges::range InputRange, std::output_iterator<char>
    OutputIt> static OutputIt decode(const InputRange& input, OutputIt output) {
      static_assert(is_chunkable());
      auto chunk_output = std::array<char, decoded_chunk_size>{};
      auto func = [&](const auto& chunk) { return decode(chunk, chunk_output);
    }; auto output_view = input | ranges::views::chunk(encoded_chunk_size) |
                         ranges::views::transform(func) | ranges::views::join;
      std::ranges::copy_if(output_view, output,
                           [](auto val) { return val != 0; });
      return output;
    }
    */

  /*
  template <std::output_iterator<char> OutputIt>
  static OutputIt decode(const char* input, OutputIt output) {
    auto len = static_cast<long long>(
        strnlen(input, std::numeric_limits<long long>::max()));
    return decode(ranges::subrange(input, std::next(input, len)), output);
  }
   */

  consteval static bool is_chunkable() {
    for (auto i = radix; i > 1; i /= 2) {
      if ((i / 2) * 2 != i) {
        return false;
      }
    }
    return true;
  };

 private:
  using CharsetT = decltype(Traits::alphabet);
  using value_type = typename CharsetT::value_type;
  using iterator = typename CharsetT::const_iterator;

  constexpr static iterator find(const value_type& v) noexcept {
    return std::ranges::find(std::cbegin(Traits::alphabet),
                             std::cend(Traits::alphabet), v);
  }

  /** Determine the character encoding for a given value
  @return character encoding, or 0 if none such encoding exists */
  constexpr static unsigned char getval(unsigned char p) noexcept {
    return find(p) == std::cend(Traits::alphabet)
               ? static_cast<unsigned char>(255)
               : static_cast<unsigned char>(
                     std::distance(std::cbegin(Traits::alphabet), find(p)));
  }

  /** encoding as determined by size of character set */
  constexpr static auto radix = sizeof(Traits::alphabet) / sizeof(value_type);
  /** Ratio of bits per unencoded value to bits per encoded value */
  constexpr static auto ratio = std::ratio<log2(256), log2(radix)>{};
  constexpr static auto encoded_chunk_size = ratio.num;
  constexpr static auto decoded_chunk_size = ratio.den;

  /** Map from value to corresponding character in base encoding */
  constexpr static std::array<unsigned char, 256> valset{
      getval(0),   getval(1),   getval(2),   getval(3),   getval(4),
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
      getval(125), getval(126), getval(127), getval(128), getval(129),
      getval(130), getval(131), getval(132), getval(133), getval(134),
      getval(135), getval(136), getval(137), getval(138), getval(139),
      getval(140), getval(141), getval(142), getval(143), getval(144),
      getval(145), getval(146), getval(147), getval(148), getval(149),
      getval(150), getval(151), getval(152), getval(153), getval(154),
      getval(155), getval(156), getval(157), getval(158), getval(159),
      getval(160), getval(161), getval(162), getval(163), getval(164),
      getval(165), getval(166), getval(167), getval(168), getval(169),
      getval(170), getval(171), getval(172), getval(173), getval(174),
      getval(175), getval(176), getval(177), getval(178), getval(179),
      getval(180), getval(181), getval(182), getval(183), getval(184),
      getval(185), getval(186), getval(187), getval(188), getval(189),
      getval(190), getval(191), getval(192), getval(193), getval(194),
      getval(195), getval(196), getval(197), getval(198), getval(199),
      getval(200), getval(201), getval(202), getval(203), getval(204),
      getval(205), getval(206), getval(207), getval(208), getval(209),
      getval(210), getval(211), getval(212), getval(213), getval(214),
      getval(215), getval(216), getval(217), getval(218), getval(219),
      getval(220), getval(221), getval(222), getval(223), getval(224),
      getval(225), getval(226), getval(227), getval(228), getval(229),
      getval(230), getval(231), getval(232), getval(233), getval(234),
      getval(235), getval(236), getval(237), getval(238), getval(239),
      getval(240), getval(241), getval(242), getval(243), getval(244),
      getval(245), getval(246), getval(247), getval(248), getval(249),
      getval(250), getval(251), getval(252), getval(253), getval(254),
      getval(255)};

  constexpr static auto base = T;
};

}  // namespace multibase
