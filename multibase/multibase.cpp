#include "multibase.h"
#include <multibase/multibase.h>

namespace multibase {

std::string encode(const cstring_span& input, encoding base,
                   bool include_encoding) {
  return codec(base).encode(input, include_encoding);
}

std::size_t encode(const cstring_span& input, string_span& output,
                   encoding base, bool include_encoding) {
  return codec(base).encode(input, output, include_encoding);
}

std::size_t encoded_size(const cstring_span& input, encoding base,
                         bool include_encoding) {
  return codec(base).encoded_size(input, include_encoding);
}

std::string decode(const cstring_span& input, encoding base) {
  assert(input.size() > 0);
  auto decoder = codec(multibase::base(input, base));
  auto view = cstring_span(&input[0] + 1, input.size() - 1);
  return decoder.decode(view);
}

std::size_t decode(const cstring_span& input, string_span& output,
                   encoding base) {
  return codec(multibase::base(input, base)).decode(input, output);
}

std::size_t decoded_size(const cstring_span& input, encoding base) {
  return codec(multibase::base(input, base)).decoded_size(input);
}

encoding base(const cstring_span& input, encoding base) {
  return base == encoding::base_unknown ? static_cast<encoding>(input[0])
                                        : base;
}
bool is_valid(const cstring_span& input) {
  if (input.empty()) {
    return false;
  }
  return is_valid(input, base(input));
}

bool is_valid(const cstring_span& input, encoding base) {
  if (base == encoding::base_unknown || input.size() < 2) {
    return false;
  }
  return codec(base).is_valid(input.subspan(1), false);
}

}  // namespace multibase
