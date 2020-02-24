#include "multibase/codec.h"

#include <multibase/basic_algorithm.h>
#include <multibase/identity_algorithm.h>

#include <cassert>

namespace multibase {

std::unique_ptr<codec::algorithm> codec::encoder(encoding base) {
  switch (base) {
    case encoding::base_unknown:
      return std::make_unique<identity_algorithm::encoder>();
    case encoding::base_16:
      return std::make_unique<base_16::encoder>();
    case encoding::base_58_btc:
      return std::make_unique<base_58_btc::encoder>();
    default:
      return nullptr;
  }
}

std::unique_ptr<codec::algorithm> codec::decoder(encoding base) {
  switch (base) {
    case encoding::base_unknown:
      return std::make_unique<identity_algorithm::decoder>();
    case encoding::base_16:
      return std::make_unique<base_16::decoder>();
    case encoding::base_58_btc:
      return std::make_unique<base_58_btc::decoder>();
    default:
      return nullptr;
  }
}

codec::codec(encoding base)
    : base_(base), encoder_(encoder(base)), decoder_(decoder(base)) {}

encoding codec::base() const { return base_; }

}  // namespace multibase
