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

#include "multibase/encoding_metadata.hpp"

#include <ranges>  // for __find_if_fn, find_if

#include <fmt/core.h>  // for format

#include <array>       // for array
#include <functional>  // for identity
#include <stdexcept>   // for invalid_argument
#include <string>      // for char_traits

#include <magic_enum.hpp>

namespace multibase {

enum class encoding : char;

encoding_metadata encoding_metadata::from_name(std::string_view name) {
  auto encodings = magic_enum::enum_values<encoding>();
  auto match_encoding = [name](auto base) {
    auto metadata = encoding_metadata{base};
    return name == metadata.name();
  };
  if (auto it = std::ranges::find_if(encodings, match_encoding);
      it != encodings.end()) {
    return encoding_metadata{*it};
  }
  throw std::invalid_argument{fmt::format("No such encoding: {}", name)};
}

}  // namespace multibase
