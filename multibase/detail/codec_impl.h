#pragma once
#include <multibase/codec.h>
#include <map>

namespace multibase {

class codec::impl {
 public:
  encoding base();

  using size_type = cstring_span::size_type;

  bool is_valid(const cstring_span& input, bool include_encoding = true);

  /** Encode to the input, optionally including the encoding type in the output
   */
  std::string encode(const cstring_span& input, bool include_encoding = true);

  /** Encode the input, writing the result to the user-supplied buffer,
   * optionally including the encoding type in the output
   * @return Number of bytes written */
  size_type encode(const cstring_span& input, string_span& output,
                   bool include_encoding = true);

  size_type encoded_size(const cstring_span& input,
                         bool include_encoding = true);

  std::string decode(const cstring_span& input);

  size_type decode(const cstring_span& input, string_span& output);

  size_type decoded_size(const cstring_span& input);

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
  class impl_tag {};
  virtual bool is_valid(const cstring_span& input, impl_tag) = 0;
  virtual size_type encode(const cstring_span& input, string_span& output,
                           impl_tag) = 0;
  virtual size_type decode(const cstring_span& input, string_span& output,
                           impl_tag) = 0;
  virtual encoding get_encoding() = 0;
  virtual size_type get_encoded_size(const cstring_span& input) = 0;
  virtual size_type get_decoded_size(const cstring_span& input) = 0;

 private:
  size_type encoding_size(bool include_encoding);
  size_type write_encoding(string_span& output, bool include_encoding);
};

}  // namespace multibase
