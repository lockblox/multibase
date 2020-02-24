#pragma once

#include <chunx/join.h>
#include <chunx/variable_length_policy.h>
#include <multibase/encoding.h>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>
#include <boost/tokenizer.hpp>
#include <functional>
#include <map>
#include <memory>

namespace multibase {

/** A symmetric pair of functions which encode and decode data */
class codec {
 public:
  explicit codec(encoding base);
  codec(const codec&) = delete;
  codec(codec&&) = delete;
  codec& operator=(const codec&) = delete;
  codec& operator=(codec&&) = delete;

  class algorithm {
   public:
    virtual ~algorithm() = default;

    /** Resets the algorithm */
    virtual void reset() {}

    /** Returns the input size required to decode a single block */
    virtual std::size_t block_size() { return 0; }

    /** Returns the size of a processed block */
    virtual std::size_t output_size() { return 0; }

    /** Processes an input block returning any intermediate result */
    virtual std::string process(std::string_view input) = 0;

    /** Returns the final output */
    virtual std::string final() { return std::string(); }
  };

  [[nodiscard]] encoding base() const;

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

  template <typename InputIt>
  range_type<InputIt> encode(InputIt first, InputIt last,
                             bool multiformat = true);

  template <typename Range>
  range_type<typename boost::range_iterator<Range>::type> encode(
      Range& input, bool multiformat = true);

  template <typename InputIt>
  range_type<InputIt> decode(InputIt first, InputIt last);

  template <typename Range>
  range_type<typename boost::range_iterator<Range>::type> decode(Range& input);

 private:
  static std::unique_ptr<algorithm> encoder(encoding base);
  static std::unique_ptr<algorithm> decoder(encoding base);
  encoding base_;
  std::shared_ptr<algorithm> encoder_;
  std::shared_ptr<algorithm> decoder_;
};

/** Returns a output_range for an input range. */
template <typename InputIt>
codec::range_type<InputIt> decode(InputIt first, InputIt last, encoding base);

template <typename InputIt>
codec::range_type<InputIt> decode(InputIt first, InputIt last);

template <typename InputIt>
codec::range_type<InputIt> encode(InputIt first, InputIt last, encoding base,
                                  bool multicodec = true);

template <typename InputIt>
codec::range_type<InputIt> encode(InputIt first, InputIt last,
                                  bool multiformat = true);

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> encode(
    Range& input, encoding base, bool multiformat = true);

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> encode(
    Range& input, bool multiformat = true);

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> decode(
    Range& input);

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> decode(
    Range& input, encoding base);

template <typename InputIt>
codec::range_type<InputIt> codec::encode(InputIt first, InputIt last,
                                         bool multiformat) {
  assert(encoder_);
  auto encoder = encoder_;
  auto base = base_;
  function_type f = [encoder, multiformat, base](auto input) mutable {
    std::string result;
    if (multiformat) {
      multiformat = false;
      result = std::string{static_cast<char>(base)};
    }
    result += encoder->process(input);
    return result;
  };
  auto block_size = base == encoding::base_58_btc ? 0 : encoder_->block_size();
  auto p = chunx::variable_length_policy<InputIt>{block_size};
  auto chunker = chunker_type<InputIt>{first, last, p};
  auto begin = transform_iterator<InputIt>{chunker.begin(), f};
  auto end = transform_iterator<InputIt>{chunker.end()};
  return chunx::join(begin, end);
}

template <typename InputIt>
codec::range_type<InputIt> codec::decode(InputIt first, InputIt last) {
  assert(decoder_);
  auto decoder = decoder_;
  function_type f = [decoder](std::string_view input) -> std::string {
    return decoder->process(input);
  };
  auto block_size =
      base() == encoding::base_58_btc ? 0 : decoder_->block_size();
  auto p = chunx::variable_length_policy<InputIt>{block_size};
  auto chunker = chunker_type<InputIt>{first, last, p};
  auto begin = transform_iterator<InputIt>{chunker.begin(), f};
  auto end = transform_iterator<InputIt>{chunker.end()};
  return chunx::join(begin, end);
}

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> codec::encode(
    Range& input, bool multiformat) {
  return encode(input.begin(), input.end(), multiformat);
}

template <typename Range>
typename codec::range_type<typename boost::range_iterator<Range>::type>
codec::decode(Range& input) {
  return decode(input.begin(), input.end());
}

template <typename Range>
codec::range_type<typename boost::range_iterator<Range>::type> decode(
    Range& input) {
  return decode(input.begin(), input.end());
}

template <typename InputIt>
codec::range_type<InputIt> decode(InputIt first, InputIt last) {
  auto base = encoding::base_unknown;
  if (first != last) {
    base = static_cast<encoding>(*first);
    ++first;
  }
  codec f{base};
  return f.decode(first, last);
}

}  // namespace multibase
