#pragma once

#pragma warning(disable : 6285)
#include <fmt/format.h>

#include <array>

#include <multibase/encoding.hpp>
#include <multibase/encoding_case.hpp>

namespace multibase {

template <encoding T>
struct encoding_traits {
  constexpr static std::array<char, 0> alphabet = {};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::none;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_2> {
  constexpr static const std::array<char, 2> alphabet = {'0', '1'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::none;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_8> {
  constexpr static const std::array<char, 8> alphabet = {'0', '1', '2', '3',
                                                         '4', '5', '6', '7'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::none;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_10> {
  constexpr static const std::array<char, 10> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::none;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_16> {
  constexpr static const std::array<char, 16> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_16_upper> {
  constexpr static const std::array<char, 16> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32> {
  constexpr static const std::array<char, 32> alphabet = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
      'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '2', '3', '4', '5', '6', '7'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_upper> {
  constexpr static const std::array<char, 32> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
      'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
      'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_hex> {
  constexpr static const std::array<char, 32> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
      'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_hex_pad> {
  constexpr static const std::array<char, 32> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
      'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
      'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_hex_pad_upper> {
  constexpr static const std::array<char, 32> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
      'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_hex_upper> {
  constexpr static const std::array<char, 32> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
      'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_pad> {
  constexpr static const std::array<char, 32> alphabet = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
      'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '2', '3', '4', '5', '6', '7'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_pad_upper> {
  constexpr static const std::array<char, 32> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
      'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
      'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_32_z> {
  constexpr static const std::array<char, 32> alphabet = {
      'y', 'b', 'n', 'd', 'r', 'f', 'g', '8', 'e', 'j', 'k',
      'm', 'c', 'p', 'q', 'x', 'o', 't', '1', 'u', 'w', 'i',
      's', 'z', 'a', '3', '4', '5', 'h', '7', '6', '9'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_36> {
  constexpr static const std::array<char, 36> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::lower;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_36_upper> {
  constexpr static const std::array<char, 36> alphabet = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
      'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::upper;
  constexpr static char is_case_sensitive = false;
};

template <>
struct encoding_traits<encoding::base_58_btc> {
  constexpr static const std::array<char, 58> alphabet = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
      'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
      'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_58_flickr> {
  constexpr static const std::array<char, 58> alphabet = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L',
      'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_64> {
  constexpr static const std::array<char, 64> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_64_pad> {
  constexpr static const std::array<char, 64> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_64_url> {
  constexpr static const std::array<char, 64> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};
  constexpr static char padding = 0;
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

template <>
struct encoding_traits<encoding::base_64_url_pad> {
  constexpr static const std::array<char, 64> alphabet = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};
  constexpr static char padding = '=';
  constexpr static encoding_case type_case = encoding_case::both;
  constexpr static char is_case_sensitive = true;
};

}  // namespace multibase
