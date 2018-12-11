#pragma once

#include <multibase/encoding.h>
#include <memory>
#include <string>
#include <string_view>
#include <gsl/string_span>

namespace multibase {

using cstring_span = gsl::cstring_span<>;
using string_span = gsl::string_span<>;

class codec {
 public:
  explicit codec(encoding base);

  encoding base() const;

  std::string encode(const cstring_span& input,
                     bool include_encoding = true);

  /** Encode the input, writing the result to the user-supplied buffer,
   * optionally including the encoding type in the output
   * @return Number of bytes written */
  std::size_t encode(const cstring_span& input, string_span& output,
                     bool include_encoding=true);

  std::size_t encoded_size(const cstring_span& input,
                           bool include_encoding=true);

  std::string decode(const cstring_span& input);

  virtual std::size_t decode(const cstring_span& input,
      string_span& output);

  std::size_t decoded_size(const cstring_span& input);

  class impl;

 private:
  impl* pImpl;
};

}  // namespace multibase
