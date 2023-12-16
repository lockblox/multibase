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

#include <fmt/core.h>  // for format

#include <array>        // for array
#include <iomanip>      // for operator<<, setfill
#include <iostream>     // for operator<<, endl, cout
#include <string>       // for string, operator<<
#include <string_view>  // for string_view
#include <vector>       // for vector

#include <range/v3/iterator/basic_iterator.hpp>  // for operator!=
#include <range/v3/range_fwd.hpp>                // for cardinality

#include <multibase/codec.hpp>            // for decode, encode
#include <multibase/encoding.hpp>         // for encoding
#include <multibase/encoding_traits.hpp>  // for encoding_traits

int main() {
  static constexpr auto encoding = multibase::encoding::base_16;
  std::cout << "Radix: "
            << multibase::encoding_traits<encoding>::alphabet.size()
            << std::endl;
  auto charset =
      std::string(multibase::encoding_traits<encoding>::alphabet.begin(),
                  multibase::encoding_traits<encoding>::alphabet.end());

  std::cout << "Charset: " << charset << std::endl;
  auto input = std::vector<unsigned char>{1, 2, 4, 8, 16, 127};
  std::cout << "Input: ";
  for (auto c : input) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(c) << ",";
  }
  auto input_string = std::string{input.begin(), input.end()};
  std::cout << std::endl;
  std::cout << "String input: " << input_string << std::endl;

  auto input_view =
      std::string_view(reinterpret_cast<char*>(input.data()), input.size());

  auto encoded = multibase::encode(input_view, encoding);
  std::cout << "Encoded: " << encoded << std::endl;
  auto decoded = multibase::decode(encoded);
  std::cout << "Decoded: ";
  for (auto o : decoded) {
    std::cout << std::hex << static_cast<int>(o) << ",";
  }
  std::cout << std::endl;

  return 0;
}
