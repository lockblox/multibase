#ifndef PORTABILITY_H
#define PORTABILITY_H

#if defined(_MSC_VER)
#define MULTIBASE_CPLUSPLUS _MSVC_LANG
#else
#define MULTIBASE_CPLUSPLUS __cplusplus
#endif

#if defined(__clang__)
#if __has_feature(__cpp_consteval)
#define MULTIBASE_CONSTEVAL consteval
#else
#define MULTIBASE_CONSTEVAL constexpr
#endif
#else
// C++20 consteval
#if MULTIBASE_CPLUSPLUS >= 202002L
#define MULTIBASE_CONSTEVAL consteval
#else
#define MULTIBASE_CONSTEVAL constexpr
#endif
#endif

#endif  // PORTABILITY_H
