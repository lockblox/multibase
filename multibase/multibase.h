#pragma once

#include <multibase/basic_codec.h>

namespace multibase {

std::string encode(const std::string_view& input, encoding base,
                   bool include_encoding = true);

std::size_t encode(const std::string_view& input, std::string_view& output,
                   encoding base, bool include_encoding = true);

std::size_t encoded_size(const std::string_view& input, encoding base,
                         bool include_encoding = true);

std::string decode(const std::string_view& input,
                   encoding base = encoding::base_unknown);

std::size_t decode(const std::string_view& input, std::string_view& output,
                   encoding base = encoding::base_unknown);

std::size_t decoded_size(const std::string_view& input,
                         encoding base = encoding::base_unknown);

encoding base(const std::string_view& input,
              encoding base = encoding::base_unknown);

}  // namespace multibase
