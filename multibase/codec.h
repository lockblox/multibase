#pragma once

#include <multibase/encoding.h>
#include <memory>
#include <string>
#include <string_view>

namespace multibase {

class codec {
 public:
  explicit codec(encoding base);

  encoding base() const;

  std::string encode(const std::string_view& input,
                     bool include_encoding = true);

  /** Encode the input, writing the result to the user-supplied buffer,
   * optionally including the encoding type in the output
   * @return Number of bytes written */
  std::size_t encode(const std::string_view& input, std::string_view& output,
                     bool include_encoding=true);

  std::size_t encoded_size(const std::string_view& input,
                           bool include_encoding=true);

  std::string decode(const std::string_view& input);

  virtual std::size_t decode(const std::string_view& input,
                             std::string_view& output);

  std::size_t decoded_size(const std::string_view& input);

  class impl;

 private:
  impl* pImpl;
};

}  // namespace multibase
