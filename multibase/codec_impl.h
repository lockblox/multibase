#pragma once
#include <multibase/codec.h>
#include <map>
#include "codec.h"

namespace multibase {

class codec::impl {
 public:
  /** Encode to the input, optionally including the encoding type in the output
   */
  std::string encode(const std::string_view& input, bool include_encoding);

  std::string encode(const std::string_view& input);

  /** Encode the input, writing the result to the user-supplied buffer,
   * optionally including the encoding type in the output
   * @return Number of bytes written */
  std::size_t encode(const std::string_view& input, std::string_view& output,
                     bool include_encoding);

  virtual std::size_t encode(const std::string_view& input,
                             std::string_view& output) = 0;

  std::size_t encoded_size(const std::string_view& input,
                           bool include_encoding);

  std::string decode(const std::string_view& input);

  virtual std::size_t decode(const std::string_view& input,
                             std::string_view& output) = 0;

  std::size_t decoded_size(const std::string_view& input);

  /** Registry of codec implementations */
  class registry {
   public:
    registry() = default;
    using key_type = encoding;
    using mapped_type = std::shared_ptr<codec::impl>;
    using value_type = std::pair<key_type, mapped_type>;
    mapped_type& operator[](const key_type& key);

   private:
    using data_type = std::map<key_type, mapped_type>;
    static data_type& data();
  };

 protected:
  virtual encoding get_encoding() = 0;
  virtual std::size_t get_encoded_size(const std::string_view& input) = 0;
  virtual std::size_t get_decoded_size(const std::string_view& input) = 0;

 private:
  std::size_t encoding_size(bool include_encoding);
  std::size_t write_encoding(std::string_view& output, bool include_encoding);
};

}  // namespace multibase
