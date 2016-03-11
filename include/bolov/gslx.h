#pragma once

#include <cstddef>
#include <cstdint>

#include <utility>

#include <gsl.h>

#include <bolov/stdx.h>

namespace bolov {
namespace gslx {

// --- size

using size_t = std::ptrdiff_t;

constexpr auto size_cast(std::size_t size) noexcept -> gslx::size_t
{
    return static_cast<gslx::size_t>(size);
}

template <class T>
constexpr auto size_of = size_cast(sizeof(T));

template <class T>
constexpr auto align_of = size_cast(alignof(T));

template <class T>
constexpr auto size(const T& obj) -> gslx::size_t
{
    return size_cast(stdx::size(obj));
}

// --- as_basic_string_span

template <class Char_t, gslx::size_t N>
auto as_basic_string_span(gsl::basic_string_span<Char_t, N> str)
    -> gsl::basic_string_span<Char_t, N>
{
    return str;
}

template <class Char_ptr_t>
auto as_basic_string_span(Char_ptr_t ptr)
    -> std::enable_if_t<stdx::is_pointer_v<Char_ptr_t>,
                        gsl::basic_string_span<std::remove_pointer_t<Char_ptr_t>>>
{
    Expects(ptr != nullptr);
    return {ptr, gslx::size_cast(stdx::char_traits_length(ptr))};
}

template <class CharT, gslx::size_t N>
auto as_basic_string_span(stdx::c_array_t<CharT, N>& arr) -> gsl::basic_string_span<CharT, N - 1>
{
    Expects(N > 0 && arr[N - 1] == '\0');
    return arr;
}

template <class Char_t, class Traits, class Allocator>
auto as_basic_string_span(std::basic_string<Char_t, Traits, Allocator>& str)
    -> gsl::basic_string_span<Char_t>
{
    return {const_cast<Char_t*>(str.data()), gslx::size_cast(str.size())};
}

template <class Char_t, class Traits, class Allocator>
auto as_basic_string_span(const std::basic_string<Char_t, Traits, Allocator>& str)
    -> gsl::basic_string_span<const Char_t>
{
    return {str.data(), gslx::size_cast(str.size())};
}

template <class Char_t, class Traits, class Allocator>
auto as_basic_string_span(std::basic_string<Char_t, Traits, Allocator>&& str) = delete;

// --- as_const_basic_string_span

template <class Char_t, gslx::size_t N>
auto as_const_basic_string_span(gsl::basic_string_span<Char_t, N> str)
    -> gsl::basic_string_span<const Char_t, N>
{
    return str;
}

template <class... Args>
auto as_const_basic_string_span(Args&&... args)
    -> decltype(as_const_basic_string_span(as_basic_string_span(std::forward<Args>(args)...)))
{
    return as_const_basic_string_span(as_basic_string_span(std::forward<Args>(args)...));
}

// --- is_included_in

template <class T, gslx::size_t Rank1 = -1, gslx::size_t Rank2 = -1>
auto is_included_in(gsl::span<T, Rank1> included, gsl::span<T, Rank2> view) -> bool
{
    return std::less_equal<>{}(stdx::begin_ptr(view), stdx::begin_ptr(included)) &&
           std::less_equal<>{}(stdx::end_ptr(included), stdx::end_ptr(view));
}
template <class Char_t, gslx::size_t Rank1 = -1, gslx::size_t Rank2 = -1>
auto is_included_in(gsl::basic_string_span<Char_t, Rank1> included,
                    gsl::basic_string_span<Char_t, Rank2> view) -> bool
{
    return std::less_equal<>{}(stdx::begin_ptr(view), stdx::begin_ptr(included)) &&
           std::less_equal<>{}(stdx::end_ptr(included), stdx::end_ptr(view));
}
}
}
