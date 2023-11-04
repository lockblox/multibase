#pragma once
#include <istream>
#include <ostream>

namespace multibase {

enum class encoding : char {
  base_none = 0,
  base_2 = '0',
  base_8 = '7',
  base_10 = '9',
  base_16 = 'f',
  base_16_upper = 'F',
  base_32 = 'b',
  base_32_upper = 'B',
  base_32_pad = 'c',
  base_32_pad_upper = 'C',
  base_32_hex= 'v',
  base_32_hex_upper = 'V',
  base_32_hex_pad = 't',
  base_32_hex_pad_upper ='T',
  base_32_z = 'h',
  base_36='k',
  base_36_upper ='K',
  base_58_flickr ='Z',
  base_58_btc = 'z',
  base_64 = 'm',
  base_64_pad='M',
  base_64_url='u',
  base_64_url_pad='U'
};

}  // namespace multibase
