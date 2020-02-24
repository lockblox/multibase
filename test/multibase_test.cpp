#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <multibase/codec.h>

#include <iomanip>

TEST(Multibase, EmptyEncoding) {
  auto expected = std::string("00000000");
  auto input = std::vector<char>{0, 0, 0, 0};
  auto encoding = multibase::encoding::base_16;
  auto codec = multibase::codec{encoding};
  auto encoded = codec.encode(input.begin(), input.end(), false);
  auto result = std::string{encoded.begin(), encoded.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

TEST(Multibase, Encoding) {
  auto expected = std::string("0001020408107ff3");
  auto input = std::vector<char>{0, 1, 2, 4, 8, 16, 127, -13};
  auto encoding = multibase::encoding::base_16;
  auto codec = multibase::codec{encoding};
  auto encoded = codec.encode(input.begin(), input.end(), false);
  auto result = std::string{encoded.begin(), encoded.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

TEST(Multibase, MultibaseEncoding) {
  auto expected = std::string("f0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::codec{multibase::encoding::base_16};
  auto encoded = codec.encode(input.begin(), input.end());
  auto result = std::string{encoded.begin(), encoded.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

TEST(Multibase, Decoding) {
  auto expected = std::string{1, 2, 4, 8, 16, 127, -13};
  auto input = std::string("01020408107ff3");
  auto codec = multibase::codec{multibase::encoding::base_16};
  auto decoded = codec.decode(input.begin(), input.end());
  auto result = std::string{decoded.begin(), decoded.end()};
  EXPECT_THAT(result, ::testing::Eq(expected));
}

TEST(Multibase, RuntimeEncoding) {
  auto expected = std::string("0001020408107ff3");
  auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
  auto encoding = multibase::encoding::base_16;
  auto codec = multibase::codec{encoding};
  auto result = codec.encode(input, false);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeDecoding) {
  auto expected = std::string{0, 0, 1, 2, 4, 8, 16, 127, -13};
  auto input = std::string("000001020408107ff3");
  auto encoding = multibase::encoding::base_16;
  auto output = std::string();
  auto codec = multibase::codec{encoding};
  auto result = codec.decode(input);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, RuntimeMultibaseDecoding) {
  auto expected = std::string{0, 0, 1, 2, 4, 8, 16, 127, -13};
  auto input = std::string{"f000001020408107ff3"};
  auto result = multibase::decode(input.begin(), input.end());
  auto result_string = std::string{};
  std::copy(result.begin(), result.end(), std::back_inserter(result_string));
  EXPECT_THAT(result_string, ::testing::Eq(expected));
}

TEST(Multibase, Base58Encode) {
  auto input = std::string("elephant");
  auto expected = std::string{"ZHxwBpKd9UKM"};
  auto codec = multibase::codec{multibase::encoding::base_58_btc};
  auto result = codec.encode(input);
  auto result_string = std::string{result.begin(), result.end()};
  EXPECT_THAT(result_string, ::testing::Eq(expected));
}

TEST(Multibase, Base58Decode) {
  auto input = std::string("ZHxwBpKd9UKM");
  auto expected = std::string{"elephant"};
  auto range = multibase::decode(input.begin(), input.end());
  EXPECT_THAT(range, ::testing::Eq(expected));
}

TEST(Multibase, IdentityEncode) {
  auto input = std::vector<char>{1, 2, 4, 8, 16, 127, -13};
  auto expected = std::vector<char>{0, 1, 2, 4, 8, 16, 127, -13};
  auto codec = multibase::codec{multibase::encoding::base_unknown};
  auto result = codec.encode(input.begin(), input.end());
  EXPECT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
}

TEST(Multibase, IdentityDecode) {
  auto input = std::vector<char>{0, 1, 2, 4, 8, 16, 127, -13};
  auto expected = std::vector<char>{1, 2, 4, 8, 16, 127, -13};
  auto result = multibase::decode(input);
  EXPECT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
}

TEST(Multibase, InvalidCharacters) {
  auto input = std::string("Z\\=+BpKd9UKM");
  EXPECT_THROW(multibase::decode(input), std::invalid_argument);
}
