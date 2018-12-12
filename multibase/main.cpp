#include <multibase/basic_codec.h>
#include <multibase/multibase.h>
#include <iomanip>
#include <iostream>
#include <sstream>

int main(void) {
  static constexpr auto encoding = multibase::encoding::base_16;
  std::cout << "Radix: " << multibase::traits<encoding>::charset.size()
            << std::endl;
  auto charset = std::string(multibase::traits<encoding>::charset.begin(),
                             multibase::traits<encoding>::charset.end());

  std::cout << "Charset: " << charset << std::endl;
  auto input = std::vector<unsigned char>{1, 2, 4, 8, 16, 127};
  std::cout << "Input: ";
  for (auto c : input) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(c)
              << ",";
  }
  std::cout << std::endl;
  std::cout << "String input: " << std::hex
            << std::string(input.begin(), input.end()) << std::endl;

  auto input_view =
      cstring_span(reinterpret_cast<char*>(input.data()), input.size());

  auto codec = multibase::basic_codec<encoding>{};
  auto encoded = codec.encode(input_view, false);
  std::cout << "Encoded: " << encoded << std::endl;
  auto decoded = codec.decode(encoded);
  std::cout << "Decoded: ";
  for (auto o : decoded) {
    std::cout << std::hex << (int)(o) << ",";
  }
  std::cout << std::endl;

  return 0;
}
