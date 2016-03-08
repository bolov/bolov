#pragma once

#include <bolov/stdx.h>

namespace bolov {
namespace tt {

template <class T, class A1, class... Args>
constexpr auto is_one_of = stdx::is_same_v<T, A1> || is_one_of<T, Args...>;

template <class T, class A1>
constexpr auto is_one_of<T, A1> = stdx::is_same_v<T, A1>;

template <class T>
using remove_cv_reference_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
constexpr auto is_char = is_one_of<remove_cv_reference_t<T>, char, unsigned char, signed char,
                                   wchar_t, unsigned wchar_t, signed wchar_t, char16_t, char32_t>;
}
}
