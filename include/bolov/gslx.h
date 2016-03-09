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

template <class CharT, gslx::size_t N>
auto span_to_basic_string_span(gsl::span<CharT, N> s) -> gsl::basic_string_span<CharT>
{
    return s;
}

template <class... Args>
auto as_basic_string_span(Args&&... args)
{
    return span_to_basic_string_span(gsl::as_span(std::forward<Args>(args)...));
}

template <class CharT, gslx::size_t N>
auto as_basic_string_span(stdx::c_array_t<CharT, N>& arr) -> gsl::basic_string_span<CharT, N - 1>
{
    Expects(N > 0 && arr[N - 1] == '\0');
    return arr;
}

template <class Char_ptr_t>
auto as_basic_string_span(Char_ptr_t ptr)
    -> std::enable_if_t<stdx::is_pointer_v<Char_ptr_t>,
                        gsl::basic_string_span<std::remove_pointer_t<Char_ptr_t>>>
{
    Expects(ptr != nullptr);
    return {ptr, std::strlen(ptr)};
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
