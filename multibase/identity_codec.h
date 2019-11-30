#pragma once

#include <multibase/codec_impl.h>

namespace multibase {
class identity_codec : public codec_impl {
 protected:
  bool is_valid(const cstring_span& input, impl_tag tag) override;
  size_type encode(const cstring_span& input, string_span& output,
                   impl_tag tag) override;
  size_type decode(const cstring_span& input, string_span& output,
                   impl_tag tag) override;
  encoding get_encoding() override;
  size_type get_encoded_size(const cstring_span& input) override;
  size_type get_decoded_size(const cstring_span& input) override;
};
}  // namespace multibase
