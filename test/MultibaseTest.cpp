#include <multibase/multibase.h>
#include <iomanip>
#include "gtest/gtest.h"

TEST(Multibase, Encoding) {
  auto expected("0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::base16{};
  auto result = codec.encode(std::string_view(input), false);
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
