#pragma once

#include <multibase/algorithm.h>
#include <multibase/encoding.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <numeric>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <ranges>
#include <ratio>
#include <vector>

namespace multibase {

template <encoding T>
struct traits {
  static const std::array<char, 0> charset;
  static const char name[];
  static const char padding = 0;
  using execution_style = algorithm::block_tag;
};

/** Template implementation of base encoding which computes a lookup table at
 * compile time and avoids the virtual algorithm lookup penalty */
template <encoding T, typename Traits = traits<T>>
class basic_algorithm {
 public:
  template <std::ranges::range Range>
  static std::string encode(const Range& input) {
    std::string output;
    encode(input, std::back_inserter(output));
    return output;
  }

  // todo base52/non chunked support
  // todo c api wrapper functions
  // write is chunkable constepxpr

  template <std::ranges::range InputRange, std::output_iterator<char> OutputIt>
  static OutputIt encode(const InputRange& input, OutputIt output) {
    auto output_view = input | ranges::views::chunk(ratio.den) |
                       ranges::views::transform(encode_chunk<InputRange>) |
                       ranges::views::join;
    std::ranges::copy_if(output_view, output,
                         [](auto val) { return val != 0; });
    return output;
  }

  template <std::input_iterator FirstIt, std::input_iterator LastIt,
            std::output_iterator<char> OutputIt>
  static OutputIt encode(FirstIt first, LastIt last, OutputIt output) {
    return encode(ranges::subrange(first, last), output);
  }

  static std::string encode(const char* input) {
    auto len = static_cast<long long>(
        strnlen(input, std::numeric_limits<long long>::max()));
    return encode(ranges::subrange(input, std::next(input, len)));
  }

  template <std::output_iterator<char> OutputIt>
  static OutputIt encode(const char* input, OutputIt output) {
    auto len = static_cast<long long>(
        strnlen(input, std::numeric_limits<long long>::max()));
    return encode(ranges::subrange(input, std::next(input, len)), output);
  }

  template <std::ranges::range InputRange, std::output_iterator<char> OutputIt>
  static OutputIt decode(const InputRange& input, OutputIt output) {
    auto chunk_size = ratio.num;
    auto output_view = input | ranges::views::chunk(chunk_size) |
                       ranges::views::transform(decode_chunk<InputRange>);
    std::ranges::for_each(output_view, [&output](auto&& chunk) {
      auto first = std::begin(chunk);
      auto last = std::end(chunk);
      assert(first != last);
      auto chunk_size = *first++;
      auto _ = std::find_if(first, last, [&, i = 0](auto elem) mutable {
        if (i++ < chunk_size) {
          *output++ = elem;
          return false;
        }
        return true;
      });
    });
    return output;
  }

  template <std::output_iterator<char> OutputIt>
  static OutputIt decode(const char* input, OutputIt output) {
    auto len = static_cast<long long>(
        strnlen(input, std::numeric_limits<long long>::max()));
    return decode(ranges::subrange(input, std::next(input, len)), output);
  }

  class encoder : public algorithm {
   public:
    size_t output_size() override;
    size_t block_size() override;
    std::string process(std::string_view input) override;

    constexpr static std::size_t input_size() { return ratio.den; }
  };

  class decoder : public algorithm {
   public:
    size_t output_size() override;
    size_t block_size() override;
    std::string process(std::string_view input) override;

   private:
    constexpr static size_t input_size() { return ratio.num; }
  };

  constexpr static bool is_chunkable() {
    // an encoding is chunkable if the ratio of input/output is reducable
    return ratio.den < log2(radix);
  };
 private:
  using CharsetT = decltype(Traits::charset);
  using value_type = typename CharsetT::value_type;
  using iterator = typename CharsetT::const_iterator;


  constexpr static iterator find(const value_type& v) noexcept {
    return std::ranges::find(std::cbegin(Traits::charset),
                             std::cend(Traits::charset), v);
  }

  /** Determine the character encoding for a given value
  @return character encoding, or 0 if none such encoding exists */
  constexpr static unsigned char getval(unsigned char p) noexcept {
    return find(p) == std::cend(Traits::charset)
               ? static_cast<unsigned char>(255)
               : static_cast<unsigned char>(
                     std::distance(std::cbegin(Traits::charset), find(p)));
  }

  /** Compute base-2 logarithm */
  constexpr static std::intmax_t log2(std::intmax_t n) noexcept {
    switch (n) {
      case 0:
        return 1;
      case 1:
        return 0;
      default:
        return 1 + log2(n / 2);
    }
  }

  /** encoding as determined by size of character set */
  constexpr static auto radix = sizeof(Traits::charset) / sizeof(value_type);
  /** Ratio of bits per unencoded value to bits per encoded value */
  constexpr static auto ratio = std::ratio<log2(256), log2(radix)>{};
  /** Map from value to corresponding character in base encoding */
  static constexpr std::array<unsigned char, 256> valset{
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

  template <std::ranges::range Range>
  static std::array<char, ratio.num> encode_chunk(const Range& chunk) {
    auto output = std::array<char, ratio.num>{};
    std::ranges::fill(output, 0);
    auto input_size = std::size(chunk);
    auto elem = std::begin(chunk);
    for (int i = 0; i < ratio.den; ++i) {
      int carry = i < std::size(chunk) ? *elem++ : 0;
      [[maybe_unused]] auto _ = std::ranges::for_each(
          std::views::reverse(output), [&carry](auto& out_val) {
            carry += 256 * out_val;
            auto byte = (unsigned char*)(&out_val);
            *byte = carry % radix;
            carry /= radix;
          });
    }
    auto output_size = static_cast<int>(
        std::ceil(1.0 * std::size(output) * input_size / ratio.den));
    std::ranges::transform(
        output, std::begin(output), [output_size](auto c) mutable {
          return output_size-- > 0 ? Traits::charset[c] : Traits::padding;
        });
    return output;
  }

  template <std::ranges::range Range>
  static auto decode_chunk(const Range& chunk) {
    static_assert(ratio.den < 254, "Radix too large");
    constexpr static auto buffer_size = ratio.den + 1;
    auto output = std::array<unsigned char, buffer_size>{};
    std::ranges::fill(output, 0);
    auto input_size = 0;
    auto elem = std::begin(chunk);
    for (int i = 0; i < ratio.num; ++i) {
      int carry = 0;
      if (elem != std::end(chunk) && *elem != Traits::padding) {
        carry = valset[(unsigned char)(*elem++)];
        ++input_size;
      }
      if (carry == 255) {
        throw std::invalid_argument(std::string{"Invalid input character "} +
                                    *elem);
      }
      [[maybe_unused]] auto _ =
          std::for_each(std::rbegin(output), std::prev(std::rend(output)),
                        [&carry](auto& out_val) {
                          carry += radix * out_val;
                          out_val = static_cast<unsigned char>(carry % 256);
                          carry /= 256;
                        });
    }
    assert(input_size <= ratio.num);
    // store size as first byte
    output.front() =
        static_cast<unsigned char>(1.0 * ratio.den * input_size / ratio.num);
    return output;
  }
};

template <encoding T, typename Traits>
std::string basic_algorithm<T, Traits>::encoder::process(
    std::string_view input) {
  std::string output;
  std::size_t isize = input.size();
  auto partial_blocks = static_cast<float>(input.size()) / input_size();
  auto num_blocks = static_cast<std::size_t>(partial_blocks);
  auto osize = static_cast<size_t>(std::ceil(partial_blocks * output_size()));
  if constexpr (std::is_same_v<typename Traits::execution_style, block_tag>) {
    num_blocks = static_cast<size_t>(std::ceil(partial_blocks));
    isize = input_size() * num_blocks;
  }
  output.resize(std::max(osize, (output_size() * num_blocks)));
  auto input_it = std::begin(input);
  int length = 0;
  for (std::size_t i = 0; i < isize; ++i, ++input_it) {
    int carry = i > input.size() ? 0 : static_cast<unsigned char>(*input_it);
    int j = 0;
    for (auto oi = output.rbegin();
         (oi != output.rend()) && (carry != 0 || j < length); ++oi, ++j) {
      carry += 256 * (*oi);
      auto byte = (unsigned char*)(std::to_address(oi));
      *byte = carry % radix;
      carry /= radix;
    }
    length = j;
  }
  std::transform(output.rbegin(), output.rend(), output.rbegin(),
                 [](auto c) { return Traits::charset[c]; });
  if constexpr (Traits::padding == 0) {
    output.resize(osize);
  } else {
    auto pad_size = output.size() - osize;
    output.replace(osize, pad_size, pad_size, Traits::padding);
  }
  if constexpr (std::is_same_v<typename Traits::execution_style, stream_tag>) {
    output.erase(0, output.size() % output_size() ? output.size() - length : 0);
  }
  return output;
}

template <encoding T, typename Traits>
std::size_t basic_algorithm<T, Traits>::encoder::block_size() {
  return std::is_same_v<typename Traits::execution_style, block_tag>
             ? input_size()
             : 0;
}

template <encoding T, typename Traits>
std::size_t basic_algorithm<T, Traits>::encoder::output_size() {
  return ratio.num;
}

template <encoding T, typename Traits>
std::size_t basic_algorithm<T, Traits>::decoder::block_size() {
  return std::is_same_v<typename Traits::execution_style, block_tag>
             ? input_size()
             : 0;
}

template <encoding T, typename Traits>
std::size_t basic_algorithm<T, Traits>::decoder::output_size() {
  return ratio.den;
}

template <encoding T, typename Traits>
std::string basic_algorithm<T, Traits>::decoder::process(
    std::string_view input) {
  std::string output;
  auto end = std::ranges::find(input, Traits::padding);
  size_t input_size = std::distance(input.begin(), end);
  auto partial_blocks = static_cast<float>(input_size) / this->input_size();
  auto output_size = static_cast<size_t>(this->output_size() * partial_blocks);
  if constexpr (std::is_same_v<typename Traits::execution_style, block_tag>) {
    std::size_t num_blocks = 0;
    auto input_size_float = static_cast<float>(input.size());
    num_blocks =
        static_cast<size_t>(std::ceil(input_size_float / this->input_size()));
    output.resize(this->output_size() * num_blocks);
    input_size = this->input_size() * num_blocks;
  } else {
    output.resize(output_size);
  }
  auto input_it = input.begin();
  for (size_t i = 0; i < input_size; ++i, ++input_it) {
    int carry = i > input.size() || *input_it == Traits::padding
                    ? 0
                    : valset[(unsigned char)(*input_it)];
    if (carry == 255)
      throw std::invalid_argument(std::string{"Invalid input character "} +
                                  *input_it);
    auto j = output.size();
    while (carry != 0 || j > 0) {
      auto index = j - 1;
      carry += radix * static_cast<unsigned char>(output[index]);
      output[index] = static_cast<unsigned char>(carry % 256);
      carry /= 256;
      if (carry > 0 && index == 0) {
        output.insert(0, 1, 0);
      } else {
        j = index;
      }
    }
  }
  if constexpr (std::is_same_v<typename Traits::execution_style, block_tag>) {
    output.erase(output_size, output.size());
  }
  return output;
}

template <>
struct traits<encoding::base_16> {
  constexpr static const std::array<char, 16> charset = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  constexpr static const char name[] = "base_16";
  using execution_style = algorithm::block_tag;
  constexpr static const char padding = 0;
};
using base_16 = basic_algorithm<encoding::base_16>;

template <>
struct traits<encoding::base_58_btc> {
  constexpr static const std::array<char, 58> charset = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
      'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
      'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  constexpr static const char name[] = "base_58_btc";
  using execution_style = algorithm::stream_tag;
  constexpr static const char padding = 0;
};
using base_58_btc = basic_algorithm<encoding::base_58_btc>;

template <>
struct traits<encoding::base_64_pad> {
  constexpr static const std::array<char, 64> charset = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
  constexpr static const char name[] = "base_64_pad";
  using execution_style = algorithm::block_tag;
  constexpr static const char padding = '=';
};
using base_64_pad = basic_algorithm<encoding::base_64_pad>;

template <>
struct traits<encoding::base_64> {
  constexpr static const std::array<char, 64> charset = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
  constexpr static const char name[] = "base_64";
  using base_64 = basic_algorithm<encoding::base_64>;
  using execution_style = algorithm::block_tag;
  constexpr static const char padding = 0;
};
using base_64 = basic_algorithm<encoding::base_64>;

}  // namespace multibase
