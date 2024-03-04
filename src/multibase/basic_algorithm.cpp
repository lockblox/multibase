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

#include <multibase/basic_algorithm.hpp>

namespace multibase {

template <>
std::string_view
basic_algorithm<encoding::base_none, encoding_traits<encoding::base_none>>::
    encode(std::span<const std::byte> chunk, std::span<char> output) {
  auto result = std::ranges::transform(
      chunk, output.begin(), [](auto byte) { return static_cast<char>(byte); });
  auto output_view = std::string_view{
      output.data(),
      static_cast<std::size_t>(std::distance(output.begin(), result.out))};
  return output_view;
}

template <>
std::span<std::byte> basic_algorithm<
    encoding::base_none,
    encoding_traits<encoding::base_none>>::decode(std::string_view chunk,
                                                  std::span<std::byte> output) {
  auto result = std::ranges::transform(chunk, output.begin(), [](auto chr) {
    return std::byte{static_cast<unsigned char>(chr)};
  });
  return std::span{output.data(), static_cast<std::size_t>(std::distance(
                                      output.begin(), result.out))};
}

}  // namespace multibase
