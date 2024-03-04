// Copyright 2023 Lockblox
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <multibase/codec.hpp>

#include <optional>   // for optional
#include <stdexcept>  // for invalid_argument

#include <fmt/core.h>  // for format

#include <magic_enum.hpp>  // for enable_if_t, enum_cast

namespace multibase {
enum class encoding : char;
}  // namespace multibase

namespace multibase {

codec::codec(encoding base) {
  switch (base) {
    using enum multibase::encoding;
    case base_none:
      init<::multibase::base_none>();
      break;
    case base_2:
      init<::multibase::base_2>();
      break;
    case base_8:
      init<::multibase::base_8>();
      break;
    case base_10:
      init<::multibase::base_10>();
      break;
    case base_16:
      init<::multibase::base_16>();
      break;
    case base_16_upper:
      init<::multibase::base_16_upper>();
      break;
    case encoding::base_32:
      init<::multibase::base_32>();
      break;
    case encoding::base_32_upper:
      init<::multibase::base_32_upper>();
      break;
    case encoding::base_32_pad:
      init<::multibase::base_32_pad>();
      break;
    case encoding::base_32_pad_upper:
      init<::multibase::base_32_pad_upper>();
      break;
    case encoding::base_32_hex:
      init<::multibase::base_32_hex>();
      break;
    case encoding::base_32_hex_upper:
      init<::multibase::base_32_hex_upper>();
      break;
    case encoding::base_32_hex_pad:
      init<::multibase::base_32_hex_pad>();
      break;
    case encoding::base_32_hex_pad_upper:
      init<::multibase::base_32_hex_pad_upper>();
      break;
    case encoding::base_32_z:
      init<::multibase::base_32_z>();
      break;
    case encoding::base_36:
      init<::multibase::base_36>();
      break;
    case encoding::base_36_upper:
      init<::multibase::base_36_upper>();
      break;
    case encoding::base_58_flickr:
      init<::multibase::base_58_flickr>();
      break;
    case encoding::base_58_btc:
      init<::multibase::base_58_btc>();
      break;
    case encoding::base_64:
      init<::multibase::base_64>();
      break;
    case encoding::base_64_pad:
      init<::multibase::base_64_pad>();
      break;
    case encoding::base_64_url:
      init<::multibase::base_64_url>();
      break;
    case encoding::base_64_url_pad:
      init<::multibase::base_64_url_pad>();
      break;
  }
}

std::size_t codec::encoded_size(std::size_t len) { return encoded_size_(len); }

std::string_view codec::encode(std::span<const std::byte> input,
                               std::span<char> output) {
  return encode_(input, output);
}

std::size_t codec::decoded_size(std::size_t len) { return decoded_size_(len); }

std::byte codec::decode(char chr) { return decode_byte_(chr); }

std::span<std::byte> codec::decode(std::string_view input,
                                   std::span<std::byte> output) {
  return decode_(input, output);
}

[[nodiscard]] std::optional<std::size_t> codec::encoded_chunk_size() const {
  return encoded_chunk_size_();
}

[[nodiscard]] std::optional<std::size_t> codec::decoded_chunk_size() const {
  return decoded_chunk_size_();
}

char encode(encoding base) {
  return static_cast<std::underlying_type_t<multibase::encoding>>(base);
}

encoding decode(char byte) {
  auto base = magic_enum::enum_cast<encoding>(byte);
  if (!base) {
    throw std::invalid_argument{fmt::format("Unsupported base {}", byte)};
  }
  return *base;
}

}  // namespace multibase
