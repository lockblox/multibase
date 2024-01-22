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
