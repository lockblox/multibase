#pragma once

#include <chunx/join.h>
#include <chunx/variable_length_policy.h>
#include <multibase/algorithm.h>
#include <multibase/encoding.h>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <functional>
#include <map>
#include <memory>

namespace multibase::detail {

using function_type = std::function<std::string(std::string_view)>;

template <typename InputIt>
using chunker_type = boost::tokenizer<chunx::variable_length_policy<InputIt>,
                                      InputIt, std::string>;

template <typename InputIt>
using chunk_iterator = typename chunker_type<InputIt>::iterator;

template <typename InputIt>
using transform_iterator =
    boost::transform_iterator<function_type, chunk_iterator<InputIt>>;

template <typename InputIt>
using range_type = decltype(chunx::join<transform_iterator<InputIt>>(
    std::declval<transform_iterator<InputIt>>(),
    std::declval<transform_iterator<InputIt>>()));

std::shared_ptr<algorithm> encoder(encoding base);
std::shared_ptr<algorithm> decoder(encoding base);

}  // namespace multibase::detail

namespace multibase {

template <typename InputIt>
detail::range_type<InputIt> encode(InputIt first, InputIt last, encoding base,
                                   bool multiformat = true);

template <typename Range>
detail::range_type<typename boost::range_iterator<Range>::type> encode(
    Range& input, encoding base, bool multiformat = true);

template <typename InputIt>
detail::range_type<InputIt> decode(InputIt first, InputIt last,
                                   encoding base = encoding::base_unknown);

template <typename Range>
detail::range_type<typename boost::range_iterator<Range>::type> decode(
    Range& input, encoding base = encoding::base_unknown);

/** IMPLEMENTATION */

template <typename InputIt>
detail::range_type<InputIt> encode(InputIt first, InputIt last, encoding base,
                                   bool multiformat) {
  auto encoder = detail::encoder(base);
  if (encoder == nullptr) throw std::invalid_argument("Unsupported base");
  auto block_size = encoder->block_size();
  detail::function_type f = [encoder = std::move(encoder), multiformat,
                             base](auto input) mutable {
    std::string result;
    if (multiformat) {
      multiformat = false;
      result = std::string{static_cast<char>(base)};
    }
    result += encoder->process(input);
    return result;
  };
  auto p = chunx::variable_length_policy<InputIt>{block_size};
  auto chunker = detail::chunker_type<InputIt>{first, last, p};
  auto begin =
      detail::transform_iterator<InputIt>{chunker.begin(), std::move(f)};
  auto end = detail::transform_iterator<InputIt>{chunker.end()};
  return chunx::join(begin, end);
}

template <typename Range>
detail::range_type<typename boost::range_iterator<Range>::type> encode(
    Range& input, encoding base, bool multiformat) {
  return encode(input.begin(), input.end(), base, multiformat);
}

template <typename InputIt>
detail::range_type<InputIt> decode(InputIt first, InputIt last, encoding base) {
  if (base == encoding::base_unknown && first != last) {
    base = static_cast<encoding>(*first);
    ++first;
  }
  auto decoder = detail::decoder(base);
  if (decoder == nullptr) throw std::invalid_argument("Unsupported base");
  auto block_size = decoder->block_size();
  detail::function_type f =
      [decoder = std::move(decoder)](std::string_view input) -> std::string {
    return decoder->process(input);
  };
  auto p = chunx::variable_length_policy<InputIt>{block_size};
  auto chunker = detail::chunker_type<InputIt>{first, last, p};
  auto begin =
      detail::transform_iterator<InputIt>{chunker.begin(), std::move(f)};
  auto end = detail::transform_iterator<InputIt>{chunker.end()};
  return chunx::join(begin, end);
}

template <typename Range>
typename detail::range_type<typename boost::range_iterator<Range>::type> decode(
    Range& input, encoding base) {
  return decode(input.begin(), input.end(), base);
}

}  // namespace multibase
