#include "multibase/codec.h"

#include <multibase/basic_algorithm.h>
#include <multibase/identity_algorithm.h>

#include <cassert>

namespace multibase::detail {

std::shared_ptr<algorithm> encoder(encoding base) {
  switch (base) {
    case encoding::base_256:
      return std::make_shared<identity_algorithm::encoder>();
    case encoding::base_16:
      return std::make_shared<base_16::encoder>();
    case encoding::base_58_btc:
      return std::make_shared<base_58_btc::encoder>();
    case encoding::base_64:
      return std::make_shared<base_64::encoder>();
    case encoding::base_64_pad:
      return std::make_shared<base_64_pad::encoder>();
    default:
      return nullptr;
  }
}

std::shared_ptr<algorithm> decoder(encoding base) {
  switch (base) {
    case encoding::base_256:
      return std::make_shared<identity_algorithm::decoder>();
    case encoding::base_16:
      return std::make_shared<base_16::decoder>();
    case encoding::base_58_btc:
      return std::make_shared<base_58_btc::decoder>();
    case encoding::base_64:
      return std::make_shared<base_64::decoder>();
    case encoding::base_64_pad:
      return std::make_shared<base_64_pad::decoder>();
    default:
      return nullptr;
  }
}

}  // namespace multibase::detail
