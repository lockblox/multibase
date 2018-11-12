#include <multibase/Base.h>

namespace multibase {

const std::array<unsigned char, 256> base_traits<BASE_16>::valset =
    compute_valset<base_traits<BASE_16>>();

}  // namespace multibase
