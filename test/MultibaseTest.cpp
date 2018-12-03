#include <multibase/multibase.h>
#include <iomanip>
#include "gtest/gtest.h"

TEST(Multibase, Encoding) {
  auto expected("0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::base16{};
  auto result = codec.encode(std::string_view(input), false);
  EXPECT_EQ(expected, result);
  std::fill(result.begin(), result.end(), 0);
  auto output = std::string_view(result);
  auto encoding = multibase::encoding::base_16;
  EXPECT_THROW(multibase::encode(input, output, encoding), std::out_of_range);
  EXPECT_EQ(output.size(), multibase::encode(input, output, encoding, false));
  EXPECT_EQ(expected, result);
}

TEST(Multibase, MultibaseEncoding) {
  auto expected("f0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::base16{};
  auto result = codec.encode(std::string_view(input));
  EXPECT_EQ(expected, result);
}

TEST(Multibase, Decoding) {
  auto expected = std::string{0, 1, 2, 4, 8, 16, 127};
  auto input("0001020408107f");
  auto codec = multibase::base16{};
  auto result(codec.decode(std::string_view(input)));
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeEncoding) {
  auto expected("0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto encoding = multibase::encoding::base_16;
  auto result = multibase::codec(encoding).encode(input, false);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeDecoding) {
  auto expected = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto input("0001020408107ff3");
  auto encoding = multibase::encoding::base_16;
  auto result = multibase::codec(encoding).decode(input);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeMultibaseDecoding) {
  auto expected = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto input("f0001020408107ff3");
  auto result = multibase::decode(input);
  EXPECT_EQ(expected, result);
}
