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

#pragma warning(disable : 4068)
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

#include <benchmark/benchmark.h>

#include <algorithm>   // for fill_n, __copy_fn
#include <cstdio>      // for snprintf, size_t
#include <cstring>     // for memcpy
#include <functional>  // for identity
#include <iterator>    // for back_insert_iterator
#include <limits>      // for numeric_limits
#include <random>
#include <sstream>
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include <range/v3/iterator/basic_iterator.hpp>  // for operator!=

#include <multibase/codec.hpp>
#include <multibase/encoding.hpp>  // for encoding

namespace {
auto constexpr output_size = 2097152;

std::string get_shuffled_input() {
  constexpr int input_size = 1024 * 1024;
  std::string input(input_size, 0);
  std::minstd_rand simple_rand;  // NOLINT
  std::ranges::for_each(input, [&simple_rand](char letter) {
    return letter = static_cast<char>(simple_rand() %
                                      std::numeric_limits<char>::max());
  });
  return input;
}

void BM_Base_Encode(benchmark::State& state) {  // NOLINT
  auto input = get_shuffled_input();
  auto buffer = std::string(multibase::base_16::encoded_size(input), 0);
  while (state.KeepRunning()) {
    multibase::base_16::encode(input, buffer);
  }
}

void BM_Memcpy(benchmark::State& state) {  // NOLINT
  auto input = get_shuffled_input();
  const auto size = input.size();
  std::vector<char> output(output_size, 0);
  while (state.KeepRunning()) {
    std::memcpy(output.data(), input.data(), size);
  }
}

void BM_Multibase_Encode(benchmark::State& state) {  // NOLINT
  // Perform setup here
  static const std::string input = "hello world";
  std::vector<char> output(output_size, 0);
  while (state.KeepRunning()) {
    auto output_it =
        multibase::encode(input, multibase::encoding::base_16, false);
    std::string output_string{};
    std::ranges::copy(output, std::back_inserter(output_string));
  }
}

void BM_C_Encode(benchmark::State& state) {  // NOLINT
  auto input = get_shuffled_input();
  auto input_len = input.size();
  std::vector<char> output(output_size, 0);
  while (state.KeepRunning()) {
    for (std::size_t i = 0, j = 0; i < input_len; ++i, j += 2) {
      [[maybe_unused]] auto _ =  // NOLINTNEXTLINE
          std::snprintf(&output[j], output.size() - j, "%02x",
                        static_cast<unsigned char>(input[i]));
    }
  }
}

void BM_Stream_Encode(benchmark::State& state) {  // NOLINT
  auto input = get_shuffled_input();
  std::string buffer;
  while (state.KeepRunning()) {
    std::ostringstream ostream;
    for (auto chr : input) {
      ostream << std::hex << static_cast<int>(chr);
    }
    buffer = ostream.str();
    (void)buffer;
  }
}

void BM_Chunk_Encode(benchmark::State& state) {  // NOLINT
  std::vector<char> output(output_size, 0);
  auto input = get_shuffled_input();
  while (state.KeepRunning()) {
    multibase::base_16::encode(input, output);
  }
}
}  // namespace

BENCHMARK(BM_Multibase_Encode);
BENCHMARK(BM_Base_Encode);
BENCHMARK(BM_C_Encode);
BENCHMARK(BM_Chunk_Encode);
BENCHMARK(BM_Memcpy);
BENCHMARK(BM_Stream_Encode);
BENCHMARK_MAIN();
