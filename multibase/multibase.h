#pragma once

#include <multibase/basic_codec.h>

namespace multibase {

class multibase {
 public:
  multibase() = default;
  explicit multibase(cstring_span data, encoding base = encoding::base_unknown);

  encoding base() const;
  cstring_span data() const;

  bool operator==(const multibase& rhs) const;
  bool operator!=(const multibase& rhs) const;
  bool operator<(const multibase& rhs) const;
  bool operator>(const multibase& rhs) const;

 private:
  cstring_span data_ = cstring_span();
  encoding base_ = encoding::base_unknown;
};

/** Determine whether the input is a valid encoding in the embedded base */
bool is_valid(const multibase& input);

bool is_valid(const cstring_span& input,
              encoding base = encoding::base_unknown);

std::string encode(const cstring_span& input, encoding base,
                   bool include_encoding = true);

std::size_t encode(const cstring_span& input, string_span& output,
                   encoding base, bool include_encoding = true);

std::size_t encoded_size(const cstring_span& input, encoding base,
                         bool include_encoding = true);

std::string decode(const cstring_span& input,
                   encoding base = encoding::base_unknown);

std::size_t decode(const cstring_span& input, string_span& output,
                   encoding base = encoding::base_unknown);

std::size_t decoded_size(const cstring_span& input,
                         encoding base = encoding::base_unknown);

}  // namespace multibase
