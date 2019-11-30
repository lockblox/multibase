#pragma once

#include <multibase/encoding.h>
#include <gsl/string_span>
#include <memory>
#include <string>
#include <string_view>

namespace multibase {

using cstring_span = gsl::cstring_span<>;
using string_span = gsl::string_span<>;

class codec_impl;

class codec {
 public:
  explicit codec(encoding base);

  [[nodiscard]] encoding base() const;

  /** Determine whether the input is a valid encoding in this base */
  bool is_valid(const cstring_span& input, bool include_encoding = true);

  std::string encode(const cstring_span& input, bool include_encoding = true);

  /** Encode the input, writing the result to the user-supplied buffer,
   * optionally including the encoding type in the output
   * @return Number of bytes written */
  std::size_t encode(const cstring_span& input, string_span& output,
                     bool include_encoding = true);

  std::size_t encoded_size(const cstring_span& input,
                           bool include_encoding = true);

  std::string decode(const cstring_span& input);

  virtual std::size_t decode(const cstring_span& input, string_span& output);

  std::size_t decoded_size(const cstring_span& input);

 private:
  codec_impl* pImpl;
};

}  // namespace multibase
