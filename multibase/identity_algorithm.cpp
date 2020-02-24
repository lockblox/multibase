#include <multibase/identity_algorithm.h>

namespace multibase {

std::string identity_algorithm::encoder::process(std::string_view input) {
  return std::string{input};
}

std::string identity_algorithm::decoder::process(std::string_view input) {
  return std::string{input};
}

}  // namespace multibase
