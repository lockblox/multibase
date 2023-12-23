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

#include <algorithm>    // for __copy_fn, __for_eac...
#include <cstdio>       // for getchar, EOF
#include <exception>    // for exception
#include <functional>   // for identity
#include <iostream>     // for operator<<, endl, cout
#include <iterator>     // for ostream_iterator
#include <stdexcept>    // for invalid_argument
#include <string>       // for string, operator<<
#include <string_view>  // for string_view

#include <CLI/App.hpp>                           // for App, CLI11_PARSE
#include <CLI/Option.hpp>                        // for Option
#include <magic_enum.hpp>                        // for enum_values
#include <range/v3/iterator/basic_iterator.hpp>  // for operator!=
#include <range/v3/range_fwd.hpp>                // for cardinality

#include "multibase/codec.hpp"              // for decode, encode
#include "multibase/encoding_metadata.hpp"  // for encoding_metadata

namespace multibase {
enum class encoding : char;
}

int main(int argc, char** argv) {
  CLI::App app{"Convert between text and multibase encoding"};
  auto is_decoder = false;
  std::string base_name;
  auto is_list = false;
  auto is_multibase = true;

  app.add_flag("-l,--list", is_list, "list supported encodings");
  auto encoding_option =
      app.add_option("-e,--encoding", base_name, "base of codec");
  app.add_option("-m, --multibase", is_multibase, "Include multibase prefix");
  app.add_option(
      "-d,--decode", is_decoder,
      "Decode data\nAssuming multibase prefix if encoding is omitted\nIf "
      "decode option is not provided, input will be encoded");
  app.require_option(1, 2);

  CLI11_PARSE(app, argc, argv)

  if (is_list) {
    std::ranges::for_each(magic_enum::enum_values<multibase::encoding>(),
                          [](auto base) {
                            auto metadata = multibase::encoding_metadata{base};
                            std::cout << metadata.name() << "\n";
                          });
  }

  try {
    if (is_decoder && !is_multibase && encoding_option->count() == 0) {
      throw std::invalid_argument{
          "Encoding must be set when decoded non-multibase"};
    }
    auto metadata = multibase::encoding_metadata::from_name(base_name);

    std::string input;
    for (int ch; (ch = std::getchar()) != EOF;) {
      input.push_back(static_cast<char>(ch));
    }
    if (is_decoder) {
      auto data = multibase::decode(input);
      std::ranges::copy(data, std::ostream_iterator<unsigned char>(std::cout));
    } else {
      std::cout << multibase::encode(input, metadata.base(), is_multibase);
    }
    std::cout << "\n";
  } catch (std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  return 0;
}
