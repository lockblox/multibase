#include <multibase/Base.h>
#include <iomanip>
#include "gtest/gtest.h"

TEST(Multibase, encoding) {
  {
    auto expected("0001020408107ff3");
    auto input = std::string{0, 1, 2, 4, 8, 16, 127, -13};
    auto codec = multibase::base16{};
    auto result = codec.encode(std::string_view(input));
    EXPECT_EQ(expected, result);
  }
}

TEST(Multibase, decoding) {
  {
    auto expected = std::string{0, 1, 2, 4, 8, 16, 127};
    auto input("0001020408107f");
    auto codec = multibase::base16{};
    auto result(codec.decode(std::string_view(input)));
    EXPECT_EQ(expected, result);
  }
}
