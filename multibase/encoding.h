#pragma once

namespace multibase {

using encoding_t = unsigned char;

enum class encoding : encoding_t {
  base_invalid = 0,
  base_16 = 102,
  base_16_upper = 70,
  base_32 = 98,
  base_32_upper = 66,
  base_58_btc = 122,
  base_64 = 109
};

}  // namespace multibase
