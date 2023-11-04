#include <benchmark/benchmark.h>
#include <multibase/basic_algorithm.h>
#include <multibase/codec.h>

#include <sstream>

static char output[2097152];
static constexpr int len = 1024*1024;
static std::string input(len, 0);

void shuffle_input() {
  std::ranges::for_each(input, [](char c) {
    return c=static_cast<char>(std::rand()%255);
  });
}

static void BM_Base_Encode(benchmark::State& state) {
  shuffle_input();
  multibase::base_16::encoder algo;
  for (auto s : state) {
    auto output = algo.process(input);
    BENCHMARK_DONT_OPTIMIZE(output);
    (void)output;
    (void)s;
  }
}

static void BM_Memcpy(benchmark::State& state) {
  shuffle_input();
  size_t size = sizeof(input);
  for (auto s : state) {
    memcpy((void*)output, (void*)input.data(), size);
    BENCHMARK_DONT_OPTIMIZE(output);
    (void)output;
    (void)s;
  }
}

static void BM_Multibase_Encode(benchmark::State& state) {
  // Perform setup here
  shuffle_input();
  static const std::string input = "hello world";
  for (auto s : state) {
    auto output_it = multibase::encode(input, multibase::encoding::base_16, false);
    std::string output_string{};
    std::ranges::copy(output, std::back_inserter(output_string));
    BENCHMARK_DONT_OPTIMIZE(output_string);
  }
}

static void BM_C_Encode(benchmark::State& state) {
  shuffle_input();
  int len = sizeof(input);
  for (auto s : state) {
    (void)s;
    for (int i = 0, j = 0; i < len; ++i, j += 2)
      sprintf(output + j, "%02x", input[i] & 0xff);
  }
}

static void BM_Stream_Encode(benchmark::State& state) {
  shuffle_input();
  std::string output;
  for (auto s : state) {
    std::ostringstream os;
    for (auto c : input) {
      os << std::hex << static_cast<int>(c);
    }
    output = os.str();
    BENCHMARK_DONT_OPTIMIZE(output);
  }
}

static void BM_Chunk_Encode(benchmark::State& state) {
  shuffle_input();
  for (auto s : state) {
    multibase::base_16::encode(input, output);
  }
}

BENCHMARK(BM_Multibase_Encode);
BENCHMARK(BM_Base_Encode);
BENCHMARK(BM_C_Encode);
BENCHMARK(BM_Chunk_Encode);
BENCHMARK(BM_Memcpy);
BENCHMARK(BM_Stream_Encode);
BENCHMARK_MAIN();