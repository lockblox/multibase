#pragma once

#include <multibase/basic_codec.h>

namespace multibase {

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

encoding base(const cstring_span& input,
              encoding base = encoding::base_unknown);

}  // namespace multibase
