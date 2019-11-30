#include <multibase/identity_codec.h>

namespace multibase {

bool identity_codec::is_valid(const cstring_span& input, codec_impl::impl_tag) {
  return !input.empty() && input[0] == 0;
}

codec_impl::size_type identity_codec::encode(const cstring_span& input,
                                             string_span& output,
                                             codec_impl::impl_tag) {
  std::copy(input.begin(), input.end(), output.begin());
  return output.size();
}

codec_impl::size_type identity_codec::decode(const cstring_span& input,
                                             string_span& output,
                                             codec_impl::impl_tag tag) {
  return encode(input, output, tag);
}

encoding identity_codec::get_encoding() { return encoding::base_unknown; }

codec_impl::size_type identity_codec::get_encoded_size(
    const cstring_span& input) {
  return input.size();
}

codec_impl::size_type identity_codec::get_decoded_size(
    const cstring_span& input) {
  return input.size();
}

}  // namespace multibase
