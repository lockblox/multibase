#include <multibase/Base.h>
#include <iomanip>
#include <iostream>
#include <sstream>

int main(void) {
  std::cout << "Radix: "
            << multibase::base_traits<multibase::BASE_16>::charset.size()
            << std::endl;
  auto charset =
      std::string(multibase::base_traits<multibase::BASE_16>::charset.begin(),
                  multibase::base_traits<multibase::BASE_16>::charset.end());

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
      std::string_view(reinterpret_cast<char*>(input.data()), input.size());

  auto encoded = multibase::base<multibase::BASE_16>::encode(input_view);
  std::cout << "Encoded: " << encoded << std::endl;
  auto decoded = multibase::base<multibase::BASE_16>::decode(encoded);
  std::cout << "Decoded: ";
  for (auto o : decoded) {
    std::cout << std::hex << (int)(o) << ",";
  }
  std::cout << std::endl;

  return 0;
}
