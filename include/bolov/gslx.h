#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include <gsl.h>

#include <bolov/stdx.h>
#include <bolov/types/type_traits.h>

namespace bolov {
namespace gslx {

using size_t = std::ptrdiff_t;

constexpr auto size_cast(std::size_t size) noexcept -> gslx::size_t
{
    return static_cast<gslx::size_t>(size);
}

template <class T>
constexpr auto size_of = size_cast(sizeof(T));

template <class T>
constexpr auto align_of = size_cast(alignof(T));

template <class T, class Enable = stdx::void_t<decltype(std::declval<T>().size())>>
constexpr auto size(const T& obj) -> gslx::size_t
{
    return size_cast(obj.size());
}

template <class T, std::size_t N>
constexpr auto size(const T (&)[N]) noexcept -> gslx::size_t
{
    return size_cast(N);
}

template <class CharT, gslx::size_t N>
auto span_to_basic_string_span(gsl::span<CharT, N> s)
    -> std::enable_if_t<tt::is_char<CharT>, gsl::basic_string_span<CharT>>
{
    return s;
}

template <class... Args>
auto as_basic_string_span(Args&&... args)
{
    return span_to_basic_string_span(gsl::as_span(std::forward<Args>(args)...));
}

}
}
