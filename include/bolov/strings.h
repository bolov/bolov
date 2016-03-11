#pragma once

#include <cctype>

#include <utility>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>

#include <gsl.h>

#include <bolov/gslx.h>

namespace bolov {
namespace str {

template <class Char_t, gslx::size_t N>
auto contains(gsl::basic_string_span<const Char_t, N> str, Char_t ch) -> bool
{
    return std::find(stdx::begin_ptr(str), stdx::end_ptr(str), ch) != stdx::end_ptr(str);
}

namespace detail {
template <class T>
using As_basic_string_span_char_t =
    typename decltype(gslx::as_basic_string_span(std::declval<T>()))::value_type;

template <class T>
using As_span_value_t = typename decltype(gsl::as_span(std::declval<T>()))::value_type;
}

template <class T>
auto contains(T&& str, detail::As_basic_string_span_char_t<T&&> ch) -> bool
{
    return contains(gslx::as_const_basic_string_span(std::forward<T>(str)), ch);
}

template <class Char_t, gslx::size_t N1, gslx::size_t N2>
auto contains(gsl::basic_string_span<const Char_t, N1> str,
              gsl::basic_string_span<const Char_t, N2> searched_str) -> bool
{
    return std::search(stdx::begin_ptr(str), stdx::end_ptr(str), stdx::begin_ptr(searched_str),
                       stdx::end_ptr(searched_str)) != stdx::end_ptr(str);
}

template <class T1, class T2>
auto contains(T1&& str, T2&& searched_str)
    -> stdx::First_of<bool, decltype(gslx::as_const_basic_string_span(std::forward<T1>(str))),
                      decltype(gslx::as_const_basic_string_span(std::forward<T2>(searched_str)))>
{
    return contains(gslx::as_const_basic_string_span(std::forward<T1>(str)),
                    gslx::as_const_basic_string_span(std::forward<T2>(searched_str)));
}

template <class CharT, gslx::size_t N1, gslx::size_t N2>
auto displace(gsl::span<gsl::basic_string_span<CharT, N2>, N1> views)
    -> std::vector<std::basic_string<std::remove_const_t<CharT>>>
{
    auto ret_v = std::vector<std::basic_string<std::remove_const_t<CharT>>>{};
    ret_v.reserve(views.size());
    std::transform(std::begin(views), std::end(views), std::back_inserter(ret_v),
                   [](auto view) { return gsl::to_string(view); });
    return ret_v;
}

template <class T>
auto displace(T&& views) -> decltype(displace(gsl::as_span(std::forward<T>(views))))
{
    return displace(gsl::as_span(std::forward<T>(views)));
}

namespace detail {
auto is_space(char c) { return std::isspace(c); }
}

template <class CharT, gslx::size_t N, class F>
auto split(gsl::basic_string_span<CharT, N> str, F is_delim)
    -> std::vector<gsl::basic_string_span<CharT>>
{
    static constexpr auto sk_average_token_size = 5;

    const auto end = str.end();

    auto token_begin = str.begin();
    auto token_end = str.begin();

    auto advance_token_begin =
        [&]() -> bool { return (token_begin = std::find_if_not(token_end, end, is_delim)) != end; };
    auto advance_token_end =
        [&]() -> bool { return (token_end = std::find_if(token_begin, end, is_delim)) != end; };

    auto tokens = std::vector<gsl::basic_string_span<CharT>>{};
    tokens.reserve(str.size() / sk_average_token_size);

    while (advance_token_begin()) {
        advance_token_end();
        tokens.emplace_back(&*token_begin, std::distance(token_begin, token_end));
    }
    tokens.shrink_to_fit();
    return tokens;
}

template <class T, class F = decltype(detail::is_space)>
auto split(T&& str, F is_delim = detail::is_space)
{
    return split(gslx::as_basic_string_span(std::forward<T>(str)), is_delim);
}

template <class CharT, gslx::size_t N, class F>
auto get_trimmed_head(gsl::basic_string_span<CharT, N> str, F pred) -> gsl::basic_string_span<CharT>
{
    const auto head = std::find_if_not(std::begin(str), std::end(str), pred);
    if (head == std::end(str))
        return str;
    return str.last(std::distance(head, std::end(str)));
}
template <class CharT, gslx::size_t N, class F>
auto get_trimmed_tail(gsl::basic_string_span<CharT, N> str, F pred) -> gsl::basic_string_span<CharT>
{
    const auto rtail = std::find_if_not(std::rbegin(str), std::rend(str), pred);
    if (rtail == std::rend(str))
        return str;
    const auto tail = rtail.base();
    return str.first(std::distance(std::begin(str), tail));
}

template <class T, class F = decltype(detail::is_space)>
auto get_trimmed_head(T&& str, F pred = detail::is_space)
{
    return get_trimmed_head(gslx::as_basic_string_span(std::forward<T>(str)), pred);
}
template <class T, class F = decltype(detail::is_space)>
auto get_trimmed_tail(T&& str, F pred = detail::is_space)
{
    return get_trimmed_tail(gslx::as_basic_string_span(std::forward<T>(str)), pred);
}

template <class T, class F = decltype(detail::is_space)>
auto get_trimmed(T&& str, F pred = detail::is_space)
{
    return get_trimmed_head(get_trimmed_tail(std::forward<T>(str), pred), pred);
}


template <class Char_t>
auto trim(std::basic_string<Char_t>& str, gsl::basic_string_span<Char_t> trimmed) -> void
{
    Expects(gslx::is_included_in(trimmed, {str}));

    std::copy(stdx::begin_ptr(trimmed), stdx::end_ptr(trimmed), stdx::begin_ptr(str));

    str.erase(0 + trimmed.size());
}
template <class Char_t, class F = decltype(detail::is_space)>
auto trim_head(std::basic_string<Char_t>& str, F pred = detail::is_space) -> void
{
    trim(str, get_trimmed_head(str, pred));
}
template <class Char_t, class F = decltype(detail::is_space)>
auto trim_tail(std::basic_string<Char_t>& str, F pred = detail::is_space) -> void
{
    trim(str, get_trimmed_tail(str, pred));
}
template <class Char_t, class F = decltype(detail::is_space)>
auto trim(std::basic_string<Char_t>& str, F pred = detail::is_space) -> void
{
    trim(str, get_trimmed(str, pred));
}
}
}
