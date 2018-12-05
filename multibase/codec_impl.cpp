#include "codec_impl.h"
#include <multibase/basic_codec.h>
#include <multibase/codec_impl.h>

namespace multibase {

std::size_t codec::impl::encoding_size(bool include_encoding) {
  return include_encoding ? sizeof(encoding_t) : 0;
}

std::size_t codec::impl::encoded_size(const std::string_view& input,
                                      bool include_encoding) {
  return get_encoded_size(input) + encoding_size(include_encoding);
}

std::size_t codec::impl::write_encoding(std::string_view& output,
                                        bool include_encoding) {
  if (!include_encoding) return 0;
  auto bytes_written = encoding_size(include_encoding);
  if (bytes_written > output.size())
    throw std::out_of_range("Output buffer too small");
  auto data = get_encoding();
  std::copy(reinterpret_cast<char*>(&data),
            reinterpret_cast<char*>(&data + sizeof(data)),
            const_cast<char*>(output.data()));
  return bytes_written;
}

std::string codec::impl::encode(const std::string_view& input,
                                bool include_encoding) {
  auto result = std::string(encoded_size(input, include_encoding), 0);
  auto output = std::string_view(result);
  auto size = encode(input, output, include_encoding);
  size += include_encoding ? 1 : 0;
  result.resize(size);
  return result;
}

std::string codec::impl::encode(const std::string_view& input) {
  return encode(input, true);
}

std::size_t codec::impl::encode(const std::string_view& input,
                                std::string_view& output,
                                bool include_encoding) {
  auto basic_size = get_encoded_size(input);
  auto size = basic_size + encoding_size(include_encoding);
  if (output.size() < size) throw std::out_of_range("Output buffer too small");
  auto begin = &output[0];
  auto view = std::string_view(begin + write_encoding(output, include_encoding),
                               basic_size);
  return encode(input, view);
}

std::size_t codec::impl::decoded_size(const std::string_view& input) {
  return get_decoded_size(input);
}

std::string codec::impl::decode(const std::string_view& input) {
  auto size = decoded_size(input);
  auto output = std::string(size, 0);
  auto view = std::string_view(output);
  output.resize(decode(input, view));
  return output;
}

encoding codec::impl::base() { return get_encoding(); }

codec::impl::registry::data_type& codec::impl::registry::data() {
  static data_type data_ =
      data_type{{encoding::base_16, std::make_unique<base_16>()}};
  return data_;
}

codec::impl::registry::mapped_type& codec::impl::registry::operator[](
    const codec::impl::registry::key_type& key) {
  return data()[key];
}

}  // namespace multibase
