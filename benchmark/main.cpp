#include <benchmark/benchmark.h>
#include <multibase/basic_algorithm.h>
#include <multibase/codec.h>

#include <sstream>

static void BM_Base_Encode(benchmark::State& state) {
  static const std::string input = "hello world";
  multibase::base_16::encoder algo;
  for (auto s : state) {
    auto output = algo.process(input);
    (void)output;
    (void)s;
  }
}

static void BM_Memcpy(benchmark::State& state) {
  const char input[] = "hello world";
  size_t size = strlen(input);
  char output[100];
  for (auto s : state) {
    memcpy(output, input, size);
    (void)output;
    (void)s;
  }
}

static void BM_Multibase_Encode(benchmark::State& state) {
  // Perform setup here
  static const std::string input = "hello world";
  for (auto s : state) {
    auto output = multibase::encode(input, multibase::encoding::base_16, false);
    auto string = std::string{output.begin(), output.end()};
    (void)string;
    (void)s;
  }
}

static void BM_C_Encode(benchmark::State& state) {
  char input[] = "hello world";
  int len = 11;
  char hex[22];
  for (auto s : state) {
    (void)s;
    for (int i = 0, j = 0; i < len; ++i, j += 2)
      sprintf(hex + j, "%02x", input[i] & 0xff);
  }
}

static void BM_Stream_Encode(benchmark::State& state) {
  static std::string input = "hello world";
  std::string output;
  for (auto s : state) {
    std::ostringstream os;
    for (auto c : input) {
      os << std::hex << static_cast<int>(c);
    }
    output = os.str();
  }
}

BENCHMARK(BM_Multibase_Encode);
BENCHMARK(BM_Base_Encode);
BENCHMARK(BM_C_Encode);
BENCHMARK(BM_Memcpy);
BENCHMARK(BM_Stream_Encode);
BENCHMARK_MAIN();