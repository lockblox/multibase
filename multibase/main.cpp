#include <multibase/basic_algorithm.h>
#include <multibase/codec.h>

#include <iomanip>
#include <iostream>
#include <sstream>

int main() {
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
  auto input_string = std::string{input.begin(), input.end()};
  std::cout << std::endl;
  std::cout << "String input: " << input_string << std::endl;

  auto input_view =
      std::string_view(reinterpret_cast<char*>(input.data()), input.size());

  auto encoded =
      multibase::encode(input_view.begin(), input_view.end(), encoding);
  std::cout << "Encoded: " << encoded << std::endl;
  auto decoded = multibase::decode(encoded);
  std::cout << "Decoded: ";
  for (auto o : decoded) {
    std::cout << std::hex << (int)(o) << ",";
  }
  std::cout << std::endl;

  return 0;
}
