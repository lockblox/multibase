#include <multibase/multibase.h>
#include <iomanip>
#include "gtest/gtest.h"

TEST(Multibase, Encoding) {
  auto expected = std::string("01020408107ff3");
  auto input = std::vector<char>{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::base_16{};
  auto begin = reinterpret_cast<char*>(&input[0]);
  auto view = std::string_view(begin, input.size());
  auto result = codec.encode(view, false);
  EXPECT_EQ(expected, result);
  std::fill(result.begin(), result.end(), 0);
  auto output = std::string_view(result);
  auto encoding = multibase::encoding::base_16;
  EXPECT_THROW(multibase::encode(view, output, encoding), std::out_of_range);
  result.resize(multibase::encoded_size(view, encoding));
  output = std::string_view(result);
  EXPECT_EQ(expected.size(), multibase::encode(view, output, encoding, false));
  EXPECT_TRUE(std::equal(expected.begin(), expected.end(), result.begin()));
}

TEST(Multibase, MultibaseEncoding) {
  auto expected = std::string("f01020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::base_16{};
  auto result = codec.encode(std::string_view(input));
  EXPECT_EQ(expected, result);
}

TEST(Multibase, Decoding) {
  auto expected = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto input = std::string("01020408107ff3");
  auto codec = multibase::base_16{};
  auto result(codec.decode(std::string_view(input)));
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeEncoding) {
  auto expected = std::string("01020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto encoding = multibase::encoding::base_16;
  auto result = multibase::codec(encoding).encode(input, false);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeDecoding) {
auto expected = std::string{0, 0, 0, 0, 0, 0, 1, 2, 4, 8, 16, 127, -13};
auto input = std::string("0000001020408107ff3");
  auto encoding = multibase::encoding::base_16;
  auto result = multibase::codec(encoding).decode(input);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeMultibaseDecoding) {
  auto expected = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto input = std::string("f01020408107ff3");
  auto result = multibase::decode(input);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, Base58Encode) {
  auto input = "elephant";
  auto expected = "HxwBpKd9UKM";
  auto codec = multibase::base_58_btc{};
  auto result = codec.encode(input, false);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, Base58Decode) {
  auto input = "HxwBpKd9UKM";
  auto expected = "elephant";
  auto codec = multibase::base_58_btc{};
  auto result = codec.decode(input);
  EXPECT_EQ(expected, result);
}
