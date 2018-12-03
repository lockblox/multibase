#include "multibase.h"
#include <multibase/multibase.h>

namespace multibase {

std::string encode(const std::string_view& input, encoding base,
                   bool include_encoding) {
  return codec(base).encode(input, include_encoding);
}

std::size_t encode(const std::string_view& input, std::string_view& output,
                   encoding base, bool include_encoding) {
  return codec(base).encode(input, output, include_encoding);
}

std::size_t encoded_size(const std::string_view& input, encoding base,
                         bool include_encoding) {
  return codec(base).encoded_size(input, include_encoding);
}

std::string decode(const std::string_view& input, encoding base) {
  return codec(multibase::base(input, base)).decode(input);
}

std::size_t decode(const std::string_view& input, std::string_view& output,
                   encoding base) {
  return codec(multibase::base(input, base)).decode(input, output);
}

std::size_t decoded_size(const std::string_view& input, encoding base) {
  return codec(multibase::base(input, base)).decoded_size(input);
}

encoding base(const std::string_view& input, encoding base) {
  return base == encoding::base_unknown ? static_cast<encoding>(input[0])
                                        : base;
}
}  // namespace multibase
