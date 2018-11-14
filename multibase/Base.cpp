#include <multibase/Base.h>

namespace multibase {

std::array<codec, 256>& codecs() {
  static std::array<codec, 256> codecs_;
  return codecs_;
}

base<BASE_16> base16;

}  // namespace multibase
