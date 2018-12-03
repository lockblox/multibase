#include "codec.h"
#include <multibase/codec.h>

#include <multibase/codec_impl.h>

namespace multibase {

codec::codec(encoding base) : pImpl(codec::impl::registry()[base].get()) {
  if (!pImpl)
    throw std::out_of_range("No codec implementation for encoding " +
                            std::to_string(static_cast<int>(base)));
}

std::string codec::encode(const std::string_view& input,
                          bool include_encoding) {
  return pImpl->encode(input, include_encoding);
}

std::size_t codec::encode(const std::string_view& input,
                          std::string_view& output, bool include_encoding) {
  return pImpl->encode(input, output, include_encoding);
}

std::size_t codec::encoded_size(const std::string_view& input,
                                bool include_encoding) {
  return pImpl->encoded_size(input, include_encoding);
}

std::string codec::decode(const std::string_view& input) {
  return pImpl->decode(input);
}

std::size_t codec::decode(const std::string_view& input,
                          std::string_view& output) {
  return pImpl->decode(input, output);
}

std::size_t codec::decoded_size(const std::string_view& input) {
  return pImpl->decoded_size(input);
}

encoding codec::base() const {
  return pImpl->base();
}

}  // namespace multibase
