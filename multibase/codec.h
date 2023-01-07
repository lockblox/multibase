#pragma once

#include <multibase/algorithm.h>
#include <multibase/encoding.h>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <functional>
#include <map>
#include <memory>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>

namespace multibase::detail {

std::shared_ptr<algorithm> encoder(encoding base);
std::shared_ptr<algorithm> decoder(encoding base);

}  // namespace multibase::detail

namespace multibase {

template <typename InputIt>
auto encode(InputIt first, InputIt last, encoding base,
            bool multiformat = true);

template <typename Range>
auto encode(Range& input, encoding base, bool multiformat = true);

template <typename InputIt>
auto decode(InputIt first, InputIt last,
            encoding base = encoding::base_unknown);

template <typename Range>
auto decode(Range& input, encoding base = encoding::base_unknown);

/** IMPLEMENTATION */

template <typename InputIt>
auto encode(InputIt first, InputIt last, encoding base, bool multiformat) {
  auto encoder = detail::encoder(base);
  if (encoder == nullptr) throw std::invalid_argument("Unsupported base");
  auto block_size = encoder->block_size();
  block_size = block_size == 0 ? std::distance(first, last) : block_size;
  auto fn = [encoder = std::move(encoder), multiformat,
             base](const auto& input) mutable {
    std::string result;
    if (multiformat) {
      multiformat = false;
      result = std::string{static_cast<char>(base)};
    }
    auto input_view = std::string_view{std::data(input), std::size(input)};
    result += encoder->process(input_view);
    return result;
  };
  return ranges::subrange(first, last) | ranges::views::chunk(block_size) |
         ranges::views::transform(fn) | ranges::views::join;
}

template <typename Range>
auto encode(Range& input, encoding base, bool multiformat) {
  return encode(begin(input), end(input), base, multiformat);
}

template <typename InputIt>
auto decode(InputIt first, InputIt last, encoding base) {
  if (base == encoding::base_unknown && first != last) {
    base = static_cast<encoding>(*first);
    ++first;
  }
  auto decoder = detail::decoder(base);
  if (decoder == nullptr) throw std::invalid_argument("Unsupported base");
  auto block_size = decoder->block_size();
  block_size = block_size == 0 ? std::distance(first, last) : block_size;
  auto fn = [decoder = std::move(decoder)](const auto& input) {
    auto input_view = std::string_view{std::data(input), std::size(input)};
    return decoder->process(input_view);
  };
  return ranges::subrange(first, last) | ranges::views::chunk(block_size) |
         ranges::views::transform(fn) | ranges::views::join;
}

template <typename Range>
auto decode(Range& input, encoding base) {
  return decode(std::begin(input), std::end(input), base);
}

}  // namespace multibase
