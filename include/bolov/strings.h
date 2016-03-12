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

namespace detail {
template <class T>
using As_basic_string_span_char_t =
    typename decltype(gslx::as_basic_string_span(std::declval<T>()))::value_type;

template <class T>
using As_span_value_t = typename decltype(gsl::as_span(std::declval<T>()))::value_type;
}

template <class String_span_t>
auto contains(const String_span_t& str,
              detail::As_basic_string_span_char_t<const String_span_t&> ch)
    -> stdx::First_of<bool, decltype(gslx::as_const_basic_string_span(str))>
{
    auto span = gslx::as_const_basic_string_span(str);
    return std::find(stdx::begin_ptr(span), stdx::end_ptr(span), ch) != stdx::end_ptr(span);
}

template <class String_span_t1, class String_span_t2>
auto contains(const String_span_t1& str, const String_span_t2& searched_str)
    -> stdx::First_of<
          bool,
          decltype(std::declval<detail::As_basic_string_span_char_t<const String_span_t1&>>() ==
                   std::declval<detail::As_basic_string_span_char_t<const String_span_t2&>>())>
{
    const auto span = gslx::as_const_basic_string_span(str);
    const auto searched_span = gslx::as_const_basic_string_span(searched_str);
    return std::search(stdx::begin_ptr(span), stdx::end_ptr(span), stdx::begin_ptr(searched_span),
                       stdx::end_ptr(searched_span)) != stdx::end_ptr(span);
}

template <class Span_of_string_span_t, class Char_t = detail::As_basic_string_span_char_t<
                                           detail::As_span_value_t<const Span_of_string_span_t&>>>
auto displace(const Span_of_string_span_t& container)
    -> std::vector<std::basic_string<std::remove_const_t<Char_t>>>
{
    auto span = gsl::as_span(container);

    auto displaced = std::vector<std::basic_string<std::remove_const_t<Char_t>>>{};
    displaced.reserve(span.size());
    std::transform(std::begin(span), std::end(span), std::back_inserter(displaced),
                   [](auto to_displace) { return gsl::to_string(to_displace); });
    return displaced;
}

namespace detail {
auto is_space(char c) { return std::isspace(c); }
}

template <class String_span_t, class F = decltype(detail::is_space),
          class Char_t = detail::As_basic_string_span_char_t<String_span_t&&>>
auto get_split(String_span_t&& str, F is_delim = detail::is_space)
    -> stdx::First_of<std::vector<gsl::basic_string_span<Char_t>>,
                      decltype(gslx::as_basic_string_span(std::forward<String_span_t>(str))),
                      decltype(std::declval<bool&>() = is_delim(std::declval<Char_t>()))>
{
    auto span = gslx::as_basic_string_span(str);

    static constexpr auto sk_average_token_size = 5;

    const auto end = span.end();

    auto token_begin = span.begin();
    auto token_end = span.begin();

    auto advance_token_begin =
        [&]() -> bool { return (token_begin = std::find_if_not(token_end, end, is_delim)) != end; };
    auto advance_token_end =
        [&]() -> bool { return (token_end = std::find_if(token_begin, end, is_delim)) != end; };

    auto tokens = std::vector<gsl::basic_string_span<Char_t>>{};
    tokens.reserve(span.size() / sk_average_token_size);

    while (advance_token_begin()) {
        advance_token_end();
        tokens.emplace_back(&*token_begin, std::distance(token_begin, token_end));
    }
    tokens.shrink_to_fit();
    return tokens;
}

template <class String_span_t, class Delim_string_span_t,
          class Char_t = detail::As_basic_string_span_char_t<const String_span_t&>,
          class Delim_char_t = detail::As_basic_string_span_char_t<const Delim_string_span_t&>>
auto get_split(const String_span_t& str, const Delim_string_span_t& delims)
    -> stdx::First_of<std::vector<gsl::basic_string_span<Char_t>>,
                      decltype(contains(delims, std::declval<Char_t>()))>
{
    return get_split(gslx::as_basic_string_span(str),
                     [&delims](Char_t ch) { return contains(delims, ch); });
}

template <class String_span_t, class F = decltype(detail::is_space),
          class Char_t = detail::As_basic_string_span_char_t<String_span_t&&>>
auto get_trimmed_head(String_span_t&& str, F pred = detail::is_space)
    -> stdx::First_of<gsl::basic_string_span<Char_t>,
                      decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    auto span = gslx::as_basic_string_span(std::forward<String_span_t>(str));

    const auto head = std::find_if_not(std::begin(span), std::end(span), pred);
    if (head == std::end(span))
        return span;
    return span.last(std::distance(head, std::end(span)));
}

template <class String_span_t, class F = decltype(detail::is_space),
          class Char_t = detail::As_basic_string_span_char_t<String_span_t&&>>
auto get_trimmed_tail(String_span_t&& str, F pred = detail::is_space)
    -> stdx::First_of<gsl::basic_string_span<Char_t>,
                      decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    auto span = gslx::as_basic_string_span(std::forward<String_span_t>(str));

    const auto rtail = std::find_if_not(std::rbegin(span), std::rend(span), pred);
    if (rtail == std::rend(span))
        return span;
    const auto tail = rtail.base();
    return span.first(std::distance(std::begin(span), tail));
}

template <class String_span_t, class F = decltype(detail::is_space),
          class Char_t = detail::As_basic_string_span_char_t<String_span_t&&>>
auto get_trimmed(String_span_t&& str, F pred = detail::is_space)
    -> stdx::First_of<gsl::basic_string_span<Char_t>,
                      decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    return get_trimmed_head(get_trimmed_tail(std::forward<String_span_t>(str), pred), pred);
}

template <class Char_t, class Traits, class Allocator, gslx::size_t N>
auto trim(std::basic_string<Char_t, Traits, Allocator>& str,
          gsl::basic_string_span<Char_t, N> trimmed) -> void
{
    Expects(gslx::is_included_in(trimmed, {str}));

    std::copy(stdx::begin_ptr(trimmed), stdx::end_ptr(trimmed), stdx::begin_ptr(str));
    str.erase(0 + trimmed.size());
}
template <class Char_t, class Traits, class Allocator, class F = decltype(detail::is_space)>
auto trim_head(std::basic_string<Char_t, Traits, Allocator>& str, F pred = detail::is_space)
    -> stdx::First_of<void, decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    trim(str, get_trimmed_head(str, pred));
}
template <class Char_t, class Traits, class Allocator, class F = decltype(detail::is_space)>
auto trim_tail(std::basic_string<Char_t, Traits, Allocator>& str, F pred = detail::is_space)
    -> stdx::First_of<void, decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    trim(str, get_trimmed_tail(str, pred));
}
template <class Char_t, class Traits, class Allocator, class F = decltype(detail::is_space)>
auto trim(std::basic_string<Char_t, Traits, Allocator>& str, F pred = detail::is_space)
    -> stdx::First_of<void, decltype(std::declval<bool&>() = pred(std::declval<Char_t>()))>
{
    trim(str, get_trimmed(str, pred));
}
}
}
