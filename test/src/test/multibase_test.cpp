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
#pragma warning(disable : 6326)
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

#include <span>  // for span

#include <algorithm>    // for copy, generate, __fo...
#include <cctype>       // for tolower, toupper
#include <cstdlib>      // for rand, size_t
#include <functional>   // for identity
#include <iostream>     // for operator<<, ostream
#include <iterator>     // for back_insert_iterator
#include <limits>       // for numeric_limits
#include <random>       // for random_device
#include <stdexcept>    // for invalid_argument
#include <string>       // for basic_string, string
#include <string_view>  // for operator<<
#include <vector>       // for allocator, vector

#include "gmock/gmock.h"  // for MakePredicateFormatt...
#include "gtest/gtest.h"  // for Test, TestInfo (ptr ...

#include <magic_enum.hpp>                        // for enum_name
#include <magic_enum_utility.hpp>                // for enum_for_each
#include <range/v3/iterator/basic_iterator.hpp>  // for operator!=
#include <range/v3/range_fwd.hpp>                // for cardinality

#include <multibase/codec.hpp>              // for decode, base_64, encode
#include <multibase/encoding.hpp>           // for encoding
#include <multibase/encoding_case.hpp>      // for encoding_case
#include <multibase/encoding_metadata.hpp>  // for encoding_metadata
#include <multibase/log.hpp>                // for log2

namespace test {

struct encoded_testcase {
  multibase::encoding base;
  std::string encoded;
  bool multiformat{true};
} __attribute__((aligned(64)));  // NOLINT

struct testcase {
  std::string name;
  std::vector<unsigned char> decoded;
  std::vector<encoded_testcase> encodings;
} __attribute__((aligned(128)));  // NOLINT

static std::ostream& operator<<(std::ostream& ostream,
                                const testcase& t_testcase) {
  ostream << t_testcase.name;
  return ostream;
}

class codec : public testing::TestWithParam<testcase> {};

// cppcheck-suppress syntaxError
TEST_P(codec, encoding) {  // NOLINT
  const auto& testcase = GetParam();
  std::ranges::for_each(testcase.encodings, [&](const auto& encoded_data) {
    auto metadata = multibase::encoding_metadata{encoded_data.base};
    auto encoded = encoded_data.encoded;
    if (!metadata.is_case_sensitive()) {
      if (metadata.type_case() == multibase::encoding_case::lower) {
        std::ranges::transform(encoded, encoded.begin(),
                               [](auto chr) { return std::tolower(chr); });
      } else if (metadata.type_case() == multibase::encoding_case::upper) {
        std::ranges::transform(encoded, encoded.begin(),
                               [](auto chr) { return std::toupper(chr); });
      }
    }
    std::cout << magic_enum::enum_name(encoded_data.base) << "\n";
    auto decoded_bytes = std::as_bytes(std::span{testcase.decoded});
    auto encoded_result = multibase::encode(decoded_bytes, encoded_data.base,
                                            encoded_data.multiformat);
    EXPECT_THAT(encoded_result.size(), encoded.size());
    EXPECT_THAT(encoded_result, encoded);
  });
}

TEST_P(codec, decoding) {  // NOLINT
  const auto& testcase = GetParam();
  std::ranges::for_each(testcase.encodings, [&](const auto& encoded_data) {
    std::cout << magic_enum::enum_name(encoded_data.base) << "\n";
    auto decoded_expected = testcase.decoded;
    auto decoded_result =
        encoded_data.multiformat
            ? multibase::decode(encoded_data.encoded)
            : multibase::decode(encoded_data.encoded, encoded_data.base);
    auto decoded_str = std::string{};
    std::ranges::transform(decoded_result, std::back_inserter(decoded_str),
                           [](auto byte) { return static_cast<char>(byte); });
    EXPECT_THAT(decoded_str.size(), decoded_expected.size());
    using ::testing::ElementsAre;
    EXPECT_THAT(decoded_str, ::testing::ElementsAreArray(decoded_expected));
  });
}

TEST(Multibase, InvalidCharacters) {  // NOLINT
  auto input = std::string("z\\=+BpKd9UKM");
  EXPECT_THROW(multibase::decode(input), std::invalid_argument);  // NOLINT
}

TEST(Multibase, BlockSize) {  // NOLINT
  // each decoded character is a 256 (8-bit) value
  // each encoded character is a 64 (6-bit) value
  // 4 encoded characters = 6+6+6+6 = 24 bits
  // 3 decoded characters = 8+8+8 = 24 bits
  auto elephant = std::string{"elephant"};

  using std::string_literals::operator""s;
  using enum multibase::encoding;

  EXPECT_THAT(multibase::base_64::encoded_size("elephant"s), 12);
  EXPECT_THAT(multibase::encode("elephant"s, base_64, false), "ZWxlcGhhbnQ");

  EXPECT_THAT(multibase::base_64::encoded_size("elephant"), 12);
  EXPECT_THAT(multibase::encode("elephant", base_64, false), "ZWxlcGhhbnQA");

  EXPECT_THAT(multibase::base_58_btc::encoded_size("elephant"s), 11);
  EXPECT_THAT(multibase::encode("elephant"s, base_58_btc, false),
              "HxwBpKd9UKM");

  EXPECT_THAT(multibase::base_58_btc::encoded_size("elephant"), 13);
  EXPECT_THAT(multibase::encode("elephant", base_58_btc, false),
              "2HstAjsCYPZyH");
  std::string output;
  multibase::encode("elephant", std::back_inserter(output), base_58_btc, false);
  EXPECT_THAT(output, "2HstAjsCYPZyH");

  auto encoded_size = multibase::base_64::encoded_size("elephant");
  auto encoded = std::string{};
  encoded.resize(encoded_size);
  encoded = multibase::base_64::encode("elephant", std::span{encoded});
  EXPECT_THAT(encoded, testing::StrEq("ZWxlcGhhbnQ"));
  encoded.clear();
  multibase::encode("elephant", std::back_inserter(encoded), base_64, false);
  EXPECT_THAT(encoded, "ZWxlcGhhbnQA");
  encoded.clear();
  multibase::encode(std::string{"elephant"}, std::back_inserter(encoded),
                    base_64, false);
  EXPECT_THAT(encoded, "ZWxlcGhhbnQ");

  auto decoded = std::vector<std::byte>{};
  multibase::decode("ZWxlcGhhbnQ", std::back_inserter(decoded), base_64);
  // EXPECT_THAT(decoded, "elephant");

  decoded.clear();
  auto encoded_buffer = std::string{"ZWxlcGhhbnQA"};
  multibase::decode(
      std::string_view{encoded_buffer.data(), encoded_buffer.size() - 1},
      std::back_inserter(decoded), base_64);
  using ::testing::ElementsAre;
  auto decoded_str = std::string{};
  std::ranges::transform(decoded, std::back_inserter(decoded_str),
                         [](auto byte) { return static_cast<char>(byte); });
  EXPECT_THAT(decoded_str, elephant);
  decoded.clear();
  encoded_buffer.back() = 'B';
  // multibase::decode(encoded_view, std::back_inserter(decoded), base_64);
  // EXPECT_THAT(decoded, "elephant");
}

TEST(Multibase, log2) {  // NOLINT
  EXPECT_THAT(multibase::log2(58), 5);
  EXPECT_THAT(multibase::log2(64), 6);
  EXPECT_THAT(multibase::log2(256), 8);

  auto testcases = {
      "bafybeiab6ussnleizgveel2uyig7bl6yxzbgxec7jbcbbqugj53wqq7o23",
      "bafybeiab6ussnleizgveel2uyig7bl6yxzbgxec7jbcbbqugj53wqq7o2y",
      "bafybeiatk2djo7k76lwlyffode33a4vpmleez4vqtrttb6ixyaqv3ixl7e",
      "bafybeiatk2djo7k76lwlyffode33a4vpmleez4vqtrttb6ixyaqv3ixl7h",
      "bafybeiatqn2wouohelg2mfalz6ujmoqdhzp4bnsidhlo2645u27yx2e25u",
      "bafybeiatqn2wouohelg2mfalz6ujmoqdhzp4bnsidhlo2645u27yx2e25x",
      "bafybeib2ouovh24hcizjhnv3xk5wuazwu6urrfgreo4wvyikqg56p6lssq",
      "bafybeib2ouovh24hcizjhnv3xk5wuazwu6urrfgreo4wvyikqg56p6lsst",
      "bafybeibru5jaooun3le664xubcfn37mfa5icmfqpogt4pljye5b3bbhz54",
      "bafybeibru5jaooun3le664xubcfn37mfa5icmfqpogt4pljye5b3bbhz57",
      "bafybeibyjwzzberkr3lutkjjuwlvtmd4k3laipv3vdiaxc3pkni7ake7ha",
      "bafybeibyjwzzberkr3lutkjjuwlvtmd4k3laipv3vdiaxc3pkni7ake7hd",
      "bafybeibypqcpl7qgzdaxzkwd4kgbcyy5jbaqmet64ovtod4zbuzat6chzu",
      "bafybeibypqcpl7qgzdaxzkwd4kgbcyy5jbaqmet64ovtod4zbuzat6chzx",
      "bafybeicmfv6pieekowdzuvpgzqrpwruis5qjllgyjm57cozmjdqe2jyn6i",
      "bafybeicmfv6pieekowdzuvpgzqrpwruis5qjllgyjm57cozmjdqe2jyn6l",
      "bafybeicwl7hsaxdhetfzz4c3txnnocdtkwczxaz2i5mwyc7goraookmbba",
      "bafybeicwl7hsaxdhetfzz4c3txnnocdtkwczxaz2i5mwyc7goraookmbbb",
      "bafybeid7ctk2gcrhjokyljiwq4wyd2usmdg5qlfokyfmyaezemsiwptsme",
      "bafybeid7ctk2gcrhjokyljiwq4wyd2usmdg5qlfokyfmyaezemsiwptsmh",
      "bafybeidfq65jd7lw2awazd2cy3uiclrqjv7patrsqhxdhkmqcxu37ihy7a",
      "bafybeidfq65jd7lw2awazd2cy3uiclrqjv7patrsqhxdhkmqcxu37ihy7d",
      "bafybeidfruh6lnzzvcaru3bffnuxnpvtjeapekomc4ehjhr7myl4n5ppbm",
      "bafybeidfruh6lnzzvcaru3bffnuxnpvtjeapekomc4ehjhr7myl4n5ppbp",
      "bafybeie2pg35ccpcnvsylesq634msypcbhbouwmble6qhzskjwvixqmeou",
      "bafybeie2pg35ccpcnvsylesq634msypcbhbouwmble6qhzskjwvixqmeov",
      "bafybeiepvkj2subyitavspfky7w5nodlvssfaypevimaljf4s3wz32dwb3",
      "bafybeiepvkj2subyitavspfky7w5nodlvssfaypevimaljf4s3wz32dwby",
      "bafybeifqdk6c6le3hn2663ktnabmgcnv447m2epafpokqyxnly65nygndm",
      "bafybeifqdk6c6le3hn2663ktnabmgcnv447m2epafpokqyxnly65nygndn",
      "bafybeig4swjz7dres3n4qfwziufw2d3skzdsvljv64tugdkayrcwb34yjq",
      "bafybeig4swjz7dres3n4qfwziufw2d3skzdsvljv64tugdkayrcwb34yjt",
      "bafybeigi6zad4teafc2krhuhqykbcshe7cfgygve6xhaunkv4wfeuup5i3",
      "bafybeigi6zad4teafc2krhuhqykbcshe7cfgygve6xhaunkv4wfeuup5iy"};
  std::ranges::for_each(testcases, [](const auto& input) {
    auto decoded = multibase::decode(std::string{input});
    EXPECT_FALSE(decoded.empty());
  });
}

TEST(Multibase, RandomData) {  // NOLINT
  std::random_device random;
  auto random_byte = [&random]() { return static_cast<std::byte>(random()); };
  std::vector<std::byte> data(static_cast<std::size_t>(
      random() % std::numeric_limits<unsigned char>::max()));
  std::generate(begin(data), end(data), random_byte);
  magic_enum::enum_for_each<multibase::encoding>(
      [&](multibase::encoding enum_val) {
        auto encoded = multibase::encode(data, enum_val);
        auto decoded = multibase::decode(encoded);
        EXPECT_THAT(decoded, data);
      });
}

INSTANTIATE_TEST_SUITE_P(  // NOLINT
    multibase, codec,
    ::testing::Values(
        testcase{"elephant",
                 {'e', 'l', 'e', 'p', 'h', 'a', 'n', 't'},
                 {{multibase::encoding::base_16, "656c657068616e74", false},
                  {multibase::encoding::base_58_btc, "zHxwBpKd9UKM", true},
                  {multibase::encoding::base_64, "mZWxlcGhhbnQ", true},
                  {multibase::encoding::base_64_pad, "MZWxlcGhhbnQ=", true}}},
        testcase{
            "case_insensitivity",
            {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'},
            {{multibase::encoding::base_16, "f68656c6c6f20776F726C64", true},
             {multibase::encoding::base_16_upper, "F68656c6c6f20776F726C64",
              true},
             {multibase::encoding::base_32, "bnbswy3dpeB3W64TMMQ", true},
             {multibase::encoding::base_32_upper, "Bnbswy3dpeB3W64TMMQ", true},
             {multibase::encoding::base_32_hex, "vd1imor3f41RMUSJCCG", true},
             {multibase::encoding::base_32_hex_upper, "Vd1imor3f41RMUSJCCG",
              true},
             {multibase::encoding::base_32_pad,
              "cnbswy3dpeB3W64TMMQ======", true},
             {multibase::encoding::base_32_pad_upper,
              "Cnbswy3dpeB3W64TMMQ======", true},
             {multibase::encoding::base_32_hex_pad,
              "td1imor3f41RMUSJCCG======", true},
             {multibase::encoding::base_32_hex_pad_upper,
              "Td1imor3f41RMUSJCCG======", true},
             {multibase::encoding::base_36, "kfUvrsIvVnfRbjWaJo", true},
             {multibase::encoding::base_36_upper, "KfUVrSIVVnFRbJWAJo", true}}},
        testcase{
            "basic",
            {'y', 'e', 's', ' ', 'm', 'a', 'n', 'i', ' ', '!'},
            {{multibase::encoding::base_2,
              "0011110010110010101110011001000000110110101100001011011100110100"
              "10010000000100001"},
             {multibase::encoding::base_8, "7362625631006654133464440102"},
             {multibase::encoding::base_10, "9573277761329450583662625"},
             {multibase::encoding::base_16, "f796573206d616e692021"},
             {multibase::encoding::base_16_upper, "F796573206D616E692021"},
             {multibase::encoding::base_32, "bpfsxgidnmfxgsibb"},
             {multibase::encoding::base_32_upper, "BPFSXGIDNMFXGSIBB"},
             {multibase::encoding::base_32_hex, "vf5in683dc5n6i811"},
             {multibase::encoding::base_32_hex_upper, "VF5IN683DC5N6I811"},
             {multibase::encoding::base_32_pad, "cpfsxgidnmfxgsibb"},
             {multibase::encoding::base_32_pad_upper, "CPFSXGIDNMFXGSIBB"},
             {multibase::encoding::base_32_hex_pad, "tf5in683dc5n6i811"},
             {multibase::encoding::base_32_hex_pad_upper, "TF5IN683DC5N6I811"},
             {multibase::encoding::base_32_z, "hxf1zgedpcfzg1ebb"},
             {multibase::encoding::base_36, "k2lcpzo5yikidynfl"},
             {multibase::encoding::base_36_upper, "K2LCPZO5YIKIDYNFL"},
             {multibase::encoding::base_58_flickr, "Z7Pznk19XTTzBtx"},
             {multibase::encoding::base_58_btc, "z7paNL19xttacUY"},
             {multibase::encoding::base_64, "meWVzIG1hbmkgIQ"},
             {multibase::encoding::base_64_pad, "MeWVzIG1hbmkgIQ=="},
             {multibase::encoding::base_64_url, "ueWVzIG1hbmkgIQ"},
             {multibase::encoding::base_64_url_pad, "UeWVzIG1hbmkgIQ=="}}},
        testcase{
            "leading_zero",
            {'\0', 'y', 'e', 's', ' ', 'm', 'a', 'n', 'i', ' ', '!'},
            {{multibase::encoding::base_2,
              "0000000000111100101100101011100110010000001101101011000010110111"
              "0011010010010000000100001"},
             {multibase::encoding::base_8, "7000745453462015530267151100204"},
             {multibase::encoding::base_10, "90573277761329450583662625"},
             {multibase::encoding::base_16, "f00796573206d616e692021"},
             {multibase::encoding::base_16_upper, "F00796573206D616E692021"},
             {multibase::encoding::base_32, "bab4wk4zanvqw42jaee"},
             {multibase::encoding::base_32_upper, "BAB4WK4ZANVQW42JAEE"},
             {multibase::encoding::base_32_hex, "v01smasp0dlgmsq9044"},
             {multibase::encoding::base_32_hex_upper, "V01SMASP0DLGMSQ9044"},
             {multibase::encoding::base_32_pad, "cab4wk4zanvqw42jaee======"},
             {multibase::encoding::base_32_pad_upper,
              "CAB4WK4ZANVQW42JAEE======"},
             {multibase::encoding::base_32_hex_pad,
              "t01smasp0dlgmsq9044======"},
             {multibase::encoding::base_32_hex_pad_upper,
              "T01SMASP0DLGMSQ9044======"},
             {multibase::encoding::base_32_z, "hybhskh3ypiosh4jyrr"},
             {multibase::encoding::base_36, "k02lcpzo5yikidynfl"},
             {multibase::encoding::base_36_upper, "K02LCPZO5YIKIDYNFL"},
             {multibase::encoding::base_58_flickr, "Z17Pznk19XTTzBtx"},
             {multibase::encoding::base_58_btc, "z17paNL19xttacUY"},
             {multibase::encoding::base_64, "mAHllcyBtYW5pICE"},
             {multibase::encoding::base_64_pad, "MAHllcyBtYW5pICE="},
             {multibase::encoding::base_64_url, "uAHllcyBtYW5pICE"},
             {multibase::encoding::base_64_url_pad, "UAHllcyBtYW5pICE="}}},
        testcase{
            "two_leading_zeros",
            {0, 0, 'y', 'e', 's', ' ', 'm', 'a', 'n', 'i', ' ', '!'},
            {{multibase::encoding::base_2,
              "0000000000000000001111001011001010111001100100000011011010110000"
              "101101110"
              "011010010010000000100001"},
             {multibase::encoding::base_8, "700000171312714403326055632220041"},
             {multibase::encoding::base_10, "900573277761329450583662625"},
             {multibase::encoding::base_16, "f0000796573206d616e692021"},
             {multibase::encoding::base_16_upper, "F0000796573206D616E692021"},
             {multibase::encoding::base_32, "baaahszltebwwc3tjeaqq"},
             {multibase::encoding::base_32_upper, "BAAAHSZLTEBWWC3TJEAQQ"},
             {multibase::encoding::base_32_hex, "v0007ipbj41mm2rj940gg"},
             {multibase::encoding::base_32_hex_upper, "V0007IPBJ41MM2RJ940GG"},
             {multibase::encoding::base_32_pad, "caaahszltebwwc3tjeaqq===="},
             {multibase::encoding::base_32_pad_upper,
              "CAAAHSZLTEBWWC3TJEAQQ===="},
             {multibase::encoding::base_32_hex_pad,
              "t0007ipbj41mm2rj940gg===="},
             {multibase::encoding::base_32_hex_pad_upper,
              "T0007IPBJ41MM2RJ940GG===="},
             {multibase::encoding::base_32_z, "hyyy813murbssn5ujryoo"},
             {multibase::encoding::base_36, "k002lcpzo5yikidynfl"},
             {multibase::encoding::base_36_upper, "K002LCPZO5YIKIDYNFL"},
             {multibase::encoding::base_58_flickr, "Z117Pznk19XTTzBtx"},
             {multibase::encoding::base_58_btc, "z117paNL19xttacUY"},
             {multibase::encoding::base_64, "mAAB5ZXMgbWFuaSAh"},
             {multibase::encoding::base_64_pad, "MAAB5ZXMgbWFuaSAh"},
             {multibase::encoding::base_64_url, "uAAB5ZXMgbWFuaSAh"},
             {multibase::encoding::base_64_url_pad, "UAAB5ZXMgbWFuaSAh"}}},
        testcase{
            "all_zeros",
            {0, 0, 0, 0, 0, 0, 0},
            {{multibase::encoding::base_2,
              "000000000000000000000000000000000000000000000000000000000"},
             {multibase::encoding::base_8, "70000000000000000000"},
             {multibase::encoding::base_10, "90000000"},
             {multibase::encoding::base_16, "f00000000000000"},
             {multibase::encoding::base_16_upper, "F00000000000000"},
             {multibase::encoding::base_32, "baaaaaaaaaaaa"},
             {multibase::encoding::base_32_upper, "BAAAAAAAAAAAA"},
             {multibase::encoding::base_32_hex, "v000000000000"},
             {multibase::encoding::base_32_hex_upper, "V000000000000"},
             {multibase::encoding::base_32_pad, "caaaaaaaaaaaa===="},
             {multibase::encoding::base_32_pad_upper, "CAAAAAAAAAAAA===="},
             {multibase::encoding::base_32_hex_pad, "t000000000000===="},
             {multibase::encoding::base_32_hex_pad_upper, "T000000000000===="},
             {multibase::encoding::base_32_z, "hyyyyyyyyyyyy"},
             {multibase::encoding::base_36, "k0000000"},
             {multibase::encoding::base_36_upper, "K0000000"},
             {multibase::encoding::base_58_flickr, "Z1111111"},
             {multibase::encoding::base_58_btc, "z1111111"},
             {multibase::encoding::base_64, "mAAAAAAAAAA"},
             {multibase::encoding::base_64_pad, "MAAAAAAAAAA=="},
             {multibase::encoding::base_64_url, "uAAAAAAAAAA"},
             {multibase::encoding::base_64_url_pad, "UAAAAAAAAAA=="}}}),
    [](const auto& test_info) { return test_info.param.name; });
// todo deliberately under-allocate for encode/decode functions
}  // namespace test
