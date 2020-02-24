#pragma once

namespace multibase {

enum class encoding : unsigned char {
  base_unknown = 0,
  base_16 = 'f',
  base_16_upper = 'F',
  base_32 = 'b',
  base_32_upper = 'B',
  base_58_btc = 'Z',
  base_64 = 'm'
};

}  // namespace multibase
