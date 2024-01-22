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

#include <algorithm>    // for __copy_fn, __for_eac...
#include <exception>    // for exception
#include <fstream>      // for basic_ifstream
#include <functional>   // for identity
#include <iostream>     // for operator<<, endl, cout
#include <iterator>     // for ostream_iterator
#include <stdexcept>    // for invalid_argument
#include <string>       // for string, operator<<
#include <string_view>  // for string_view
#include <vector>       // for vector

#include <CLI/App.hpp>                           // for App, CLI11_PARSE
#include <CLI/Option.hpp>                        // for Option
#include <magic_enum.hpp>                        // for enum_values
#include <range/v3/iterator/basic_iterator.hpp>  // for operator!=
#include <range/v3/range_fwd.hpp>                // for cardinality
#include <range/v3/view/subrange.hpp>            // for subrange

#include "multibase/codec.hpp"              // for decode, encode
#include "multibase/encoding_metadata.hpp"  // for encoding_metadata

namespace multibase {
enum class encoding : char;
}  // namespace multibase

int main(int argc, char** argv) {
  CLI::App app{"Convert between text and multibase encoding"};
  auto is_decoder = false;
  std::string base_name;
  auto is_list = false;
  auto is_multibase = true;
  std::vector<std::string> filenames;

  app.add_flag("-l,--list", is_list, "list supported encodings");
  auto* encoding_option =
      app.add_option("-e,--encoding", base_name, "base of codec");
  app.add_option("-m, --multibase", is_multibase, "Include multibase prefix");
  app.add_flag(
      "-d,--decode", is_decoder,
      "Decode data\nAssuming multibase prefix if encoding is omitted\nIf "
      "decode option is not provided, input will be encoded");
  app.require_option(1, 3);
  app.add_option("files", filenames, "A list of filenames")->expected(-1);

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
          "Encoding must be set when decoding non-multibase"};
    }
    if (!is_decoder && encoding_option->count() == 0) {
      throw std::invalid_argument{"Missing encoding"};
    }
    if (is_decoder && encoding_option->count() == 1) {
      is_multibase = false;
    }
    auto metadata = multibase::encoding_metadata::from_name(base_name);

    using istream_iterator = std::istream_iterator<unsigned char>;
    using ostream_iterator = std::ostream_iterator<char>;

    // create list of input streams, will be 1 entry for std::cin
    auto ifstreams = std::vector<std::ifstream>{};
    auto inputs = std::vector<istream_iterator>{};
    if (filenames.empty()) {
      std::cin >> std::noskipws;
      inputs.emplace_back(std::cin);
    } else {
      std::ranges::transform(
          filenames, std::back_inserter(inputs),
          [&ifstreams](const auto& file) {
            ifstreams.emplace_back(file);
            if (!ifstreams.back().is_open()) {
              throw std::invalid_argument{"No such file or directory"};
            }
            ifstreams.back() >> std::noskipws;
            return istream_iterator{ifstreams.back()};
          });
    }

    std::ranges::for_each(inputs, [&](const auto& istream) {
      auto input = ranges::subrange(istream, istream_iterator{});
      auto output = ostream_iterator{std::cout};
      if (is_decoder) {
        if (is_multibase) {
          multibase::decode(input, output);
        } else {
          multibase::decode(input, output, metadata.base());
        }
      } else {
        multibase::encode(input, output, metadata.base(), is_multibase);
      }
    });
    std::cout << "\n";
  } catch (std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  return 0;
}
