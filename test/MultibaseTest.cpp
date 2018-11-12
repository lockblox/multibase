#include <multibase/Base.h>
#include <iomanip>
#include "gtest/gtest.h"

TEST(Multibase, log2) {
  auto input = 256;
  auto expected = 8;
  auto result = multibase::log2(input);
  EXPECT_EQ(expected, result);
}

TEST(Multibase, encoding) {
  {
    auto expected("0001020408107ff3");
    auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
    auto input_view =
        std::string_view(reinterpret_cast<char*>(input.data()), input.size());
    auto result(multibase::base<multibase::BASE_16>::encode(input_view));
    EXPECT_EQ(expected, result);
  }
}

TEST(Multibase, decoding) {
  {
    auto expected = std::vector<unsigned char>{0, 1, 2, 4, 8, 16, 127};
    auto input("0001020408107f");
    auto result(multibase::base<multibase::BASE_16>::decode(input));
    EXPECT_EQ(expected, result);
  }
}
