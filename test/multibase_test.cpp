#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <multibase/codec.h>

#include <iomanip>

namespace test {

struct data {
  multibase::encoding base;
  std::vector<char> input;
  std::string expected;
  bool multiformat;
};

class codec : public testing::TestWithParam<data> {};

TEST_P(codec, encoding) {
  auto data = GetParam();
  auto result = multibase::encode(data.input, data.base, data.multiformat);
  EXPECT_THAT(result, ::testing::Eq(data.expected));
}

TEST_P(codec, decoding) {
  auto data = GetParam();
  auto base = data.multiformat ? multibase::encoding::base_unknown : data.base;
  auto result = multibase::decode(data.expected, base);
  EXPECT_THAT(result, ::testing::Eq(data.input));
}

TEST(Multibase, InvalidCharacters) {
  auto input = std::string("Z\\=+BpKd9UKM");
  EXPECT_THROW(multibase::decode(input), std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
    multibase, codec,
    ::testing::Values(
        data{multibase::encoding::base_16, {0, 0, 0, 0}, "00000000", false},
        data{multibase::encoding::base_16,
             {0, 1, 2, 4, 8, 16, 127, -13},
             "0001020408107ff3",
             false},
        data{multibase::encoding::base_16,
             {0, 1, 2, 4, 8, 16, 127, -13},
             "f0001020408107ff3",
             true},
        data{multibase::encoding::base_16,
             {1, 2, 4, 8, 16, 127, -13},
             "01020408107ff3",
             false},
        data{multibase::encoding::base_58_btc,
             {'e', 'l', 'e', 'p', 'h', 'a', 'n', 't'},
             "ZHxwBpKd9UKM",
             true},
        data{multibase::encoding::base_256,
             {1, 2, 4, 8, 16, 127, -13},
             {0, 1, 2, 4, 8, 16, 127, -13},
             true},
        data{multibase::encoding::base_64,
             {'e', 'l', 'e', 'p', 'h', 'a', 'n', 't'},
             "mZWxlcGhhbnQ",
             true},
        data{multibase::encoding::base_64_pad,
             {'e', 'l', 'e', 'p', 'h', 'a', 'n', 't'},
             "MZWxlcGhhbnQ=",
             true}));

}  // namespace test
