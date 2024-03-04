#ifndef MULTIBASE_BASE_NONE_HPP
#define MULTIBASE_BASE_NONE_HPP

#include <algorithm>
#include <limits>
#include <optional>
#include <span>
#include <string_view>

namespace multibase {

class base_none {
 public:
  static std::size_t encoded_size(std::size_t len) { return len; }

  static std::string_view encode(std::span<const std::byte> input,
                                 std::span<char> output) {
    std::ranges::transform(input, output.begin(),
                           [](auto byte) { return static_cast<char>(byte); });
    return std::string_view{output.data(), input.size()};
  }

  static char encode(std::byte byte) { return static_cast<char>(byte); }

  static std::size_t decoded_size(std::size_t len) { return len; }

  static std::span<std::byte> decode(std::string_view input,
                                     std::span<std::byte> output) {
    std::ranges::transform(input, output.begin(), [](auto chr) {
      return static_cast<std::byte>(chr);
    });
    return std::span{output.data(), input.size()};
  }

  static std::byte decode(char chr) { return static_cast<std::byte>(chr); }

  static std::optional<std::size_t> encoded_chunk_size() {
    return std::numeric_limits<std::size_t>::max();
  }

  static std::optional<std::size_t> decoded_chunk_size() {
    return std::numeric_limits<std::size_t>::max();
  }
};

}  // namespace multibase

#endif
