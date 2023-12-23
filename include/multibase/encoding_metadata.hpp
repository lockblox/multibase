#ifndef MULTIBASE_ENCODING_METADATA_HPP
#define MULTIBASE_ENCODING_METADATA_HPP

#include <string_view>

#include <magic_enum.hpp>

#include <multibase/encoding.hpp>
#include <multibase/encoding_case.hpp>
#include <multibase/encoding_traits.hpp>

namespace multibase {
class encoding_metadata {
 public:
  explicit constexpr encoding_metadata(enum encoding t_encoding);

  static encoding_metadata from_name(std::string_view name);

  [[nodiscard]] constexpr encoding base() const;
  [[nodiscard]] constexpr std::string_view name() const;
  [[nodiscard]] constexpr std::string_view alphabet() const;
  [[nodiscard]] constexpr char padding() const;
  [[nodiscard]] constexpr bool is_case_sensitive() const;
  [[nodiscard]] constexpr encoding_case type_case() const;

 private:
  template <enum encoding t_encoding>
  constexpr void init() {
    encoding_ = {t_encoding};
    name_ = {magic_enum::enum_name(t_encoding)},
    alphabet_ = {encoding_traits<t_encoding>::alphabet.data(),
                 encoding_traits<t_encoding>::alphabet.size()};
    padding_ = {encoding_traits<t_encoding>::padding};
    encoding_case_ = {encoding_traits<t_encoding>::type_case};
    is_case_sensitive_ = {encoding_traits<t_encoding>::is_case_sensitive};
  }

  enum encoding encoding_ { encoding::base_none };
  std::string_view name_;
  std::string_view alphabet_;
  char padding_{0};
  enum encoding_case encoding_case_ { encoding_case::none };
  bool is_case_sensitive_{};
};  // namespace multibase

constexpr encoding_metadata::encoding_metadata(enum encoding t_encoding) {
  switch (t_encoding) {
    case encoding::base_none:
      init<encoding::base_none>();
      break;
    case encoding::base_2:
      init<encoding::base_2>();
      break;
    case encoding::base_8:
      init<encoding::base_8>();
      break;
    case encoding::base_10:
      init<encoding::base_10>();
      break;
    case encoding::base_16:
      init<encoding::base_16>();
      break;
    case encoding::base_16_upper:
      init<encoding::base_16_upper>();
      break;
    case encoding::base_32:
      init<encoding::base_32>();
      break;
    case encoding::base_32_upper:
      init<encoding::base_32_upper>();
      break;
    case encoding::base_32_pad:
      init<encoding::base_32_pad>();
      break;
    case encoding::base_32_pad_upper:
      init<encoding::base_32_upper>();
      break;
    case encoding::base_32_hex:
      init<encoding::base_32_hex>();
      break;
    case encoding::base_32_hex_upper:
      init<encoding::base_32_hex_upper>();
      break;
    case encoding::base_32_hex_pad:
      init<encoding::base_32_hex_pad>();
      break;
    case encoding::base_32_hex_pad_upper:
      init<encoding::base_32_hex_pad_upper>();
      break;
    case encoding::base_32_z:
      init<encoding::base_32_z>();
      break;
    case encoding::base_36:
      init<encoding::base_36>();
      break;
    case encoding::base_36_upper:
      init<encoding::base_36_upper>();
      break;
    case encoding::base_58_flickr:
      init<encoding::base_58_flickr>();
      break;
    case encoding::base_58_btc:
      init<encoding::base_58_btc>();
      break;
    case encoding::base_64:
      init<encoding::base_64>();
      break;
    case encoding::base_64_pad:
      init<encoding::base_64_pad>();
      break;
    case encoding::base_64_url:
      init<encoding::base_64_url>();
      break;
    case encoding::base_64_url_pad:
      init<encoding::base_64_url_pad>();
      break;
  }
}

constexpr std::string_view encoding_metadata::name() const { return name_; }

constexpr char encoding_metadata::padding() const { return padding_; }

constexpr std::string_view encoding_metadata::alphabet() const {
  return alphabet_;
}

constexpr encoding encoding_metadata::base() const { return encoding_; }

constexpr bool encoding_metadata::is_case_sensitive() const {
  return is_case_sensitive_;
}

constexpr encoding_case encoding_metadata::type_case() const {
  return encoding_case_;
}
}  // namespace multibase

#endif  // MULTIBASE_ENCODING_METADATA_HPP
