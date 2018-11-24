#include <multibase/Base.h>

namespace multibase {

std::string codec::encode(const std::string_view& input) {
  auto output = std::string(encoded_size(input), 0);
  auto view = std::string_view(output);
  encode(input, view);
  return output;
}

std::string codec::decode(const std::string_view& input) {
  auto output = std::string(decoded_size(input), 0);
  auto view = std::string_view(output);
  decode(input, view);
  return output;
}

std::array<std::unique_ptr<codec>, 256>& codec::registry() {
  static std::array<std::unique_ptr<codec>, 256> codecs_;
  return codecs_;
}

}  // namespace multibase
