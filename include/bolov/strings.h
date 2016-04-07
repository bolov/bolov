#pragma once

#include <cctype>

#include <utility>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <gsl.h>

#include <bolov/gslx.h>
#include <bolov/rtti/type_info.h>

namespace bolov {
namespace str {

namespace detail {
template <class T>
using Char_t = typename decltype(gslx::as_basic_string_span(std::declval<T>()))::value_type;

template <class T>
using Value_t = typename decltype(gsl::as_span(std::declval<T>()))::value_type;
}

template <class String_span_t>
auto contains(const String_span_t& str, detail::Char_t<const String_span_t&> ch)
    -> stdx::First_of<bool, decltype(gslx::as_const_basic_string_span(str))>
{
    auto span = gslx::as_const_basic_string_span(str);
    return std::find(stdx::begin_ptr(span), stdx::end_ptr(span), ch) != stdx::end_ptr(span);
}

template <class String_span_t1, class String_span_t2>
auto contains(const String_span_t1& str, const String_span_t2& searched_str)
    -> stdx::First_of<bool, decltype(std::declval<detail::Char_t<const String_span_t1&>>() ==
                                     std::declval<detail::Char_t<const String_span_t2&>>())>
{
    const auto span = gslx::as_const_basic_string_span(str);
    const auto searched_span = gslx::as_const_basic_string_span(searched_str);
    return std::search(stdx::begin_ptr(span), stdx::end_ptr(span), stdx::begin_ptr(searched_span),
                       stdx::end_ptr(searched_span)) != stdx::end_ptr(span);
}

template <class Span_of_string_span_t,
          class Char_t = detail::Char_t<detail::Value_t<const Span_of_string_span_t&>>>
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
          class Char_t = detail::Char_t<String_span_t&&>>
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
          class Char_t = detail::Char_t<const String_span_t&>,
          class Delim_char_t = detail::Char_t<const Delim_string_span_t&>>
auto get_split(const String_span_t& str, const Delim_string_span_t& delims)
    -> stdx::First_of<std::vector<gsl::basic_string_span<Char_t>>,
                      decltype(contains(delims, std::declval<Char_t>()))>
{
    return get_split(gslx::as_basic_string_span(str),
                     [&delims](Char_t ch) { return contains(delims, ch); });
}

template <class String_span_t, class F = decltype(detail::is_space),
          class Char_t = detail::Char_t<String_span_t&&>>
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
          class Char_t = detail::Char_t<String_span_t&&>>
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
          class Char_t = detail::Char_t<String_span_t&&>>
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

namespace detail {
template <class Char_t, class Traits, class Allocator>
auto to_ascii(const std::basic_string<Char_t, Traits, Allocator>& str) -> std::string
{
    std::string ascii;
    ascii.reserve(str.size());

    for (auto ch : str) {
        if (ch >= 0 && ch <= 127) {
            ascii.push_back(ch);
        } else {
            ascii.push_back('?');
        }
    }
    return ascii;
}

auto to_ascii(std::string& str) -> std::string & { return str; }
auto to_ascii(const std::string& str) -> const std::string & { return str; }

template <class Char_t, class Cast_t, class Enable = void>
struct String_cast_traits {
};

template <class Cast_t>
struct String_cast_traits<
    char, Cast_t, std::enable_if_t<stdx::is_integral_v<Cast_t> && stdx::is_signed_v<Cast_t>>> {
    static auto strto(const char* str, char** str_end, int base = 0) -> long long
    {
        return std::strtoll(str, str_end, base);
    }
};
template <class Cast_t>
struct String_cast_traits<
    char, Cast_t, std::enable_if_t<stdx::is_integral_v<Cast_t> && stdx::is_unsigned_v<Cast_t>>> {
    static auto strto(const char* str, char** str_end, int base = 0) -> unsigned long long
    {
        return std::strtoull(str, str_end, base);
    }
};
template <>
struct String_cast_traits<char, float> {
    static auto strto(const char* str, char** str_end, int = 0) -> float
    {
        return std::strtof(str, str_end);
    }
};
template <>
struct String_cast_traits<char, double> {
    static auto strto(const char* str, char** str_end, int = 0) -> double
    {
        return std::strtod(str, str_end);
    }
};
template <class Cast_t>

struct String_cast_traits<
    wchar_t, Cast_t, std::enable_if_t<stdx::is_integral_v<Cast_t> && stdx::is_signed_v<Cast_t>>> {
    static auto strto(const wchar_t* str, wchar_t** str_end, int base = 0) -> long long
    {
        return std::wcstoll(str, str_end, base);
    }
};
template <class Cast_t>
struct String_cast_traits<
    wchar_t, Cast_t, std::enable_if_t<stdx::is_integral_v<Cast_t> && stdx::is_unsigned_v<Cast_t>>> {
    static auto strto(const wchar_t* str, wchar_t** str_end, int base = 0) -> unsigned long long
    {
        return std::wcstoull(str, str_end, base);
    }
};
template <>
struct String_cast_traits<wchar_t, float> {
    static auto strto(const wchar_t* str, wchar_t** str_end, int = 0) -> float
    {
        return std::wcstof(str, str_end);
    }
};
template <>
struct String_cast_traits<wchar_t, double> {
    static auto strto(const wchar_t* str, wchar_t** str_end, int = 0) -> double
    {
        return std::wcstod(str, str_end);
    }
};

} // namespace detail

template <class Cast_t, class Char_t, class Traits, class Allocator>
auto string_to(const std::basic_string<Char_t, Traits, Allocator>& str)
    -> stdx::First_of<Cast_t, std::enable_if_t<stdx::is_arithmetic_v<Cast_t>>>
{
    using namespace std::string_literals;

    Char_t* last;
    errno = 0;
    auto value = detail::String_cast_traits<Char_t, Cast_t>::strto(stdx::begin_ptr(str), &last);

    if (last < stdx::end_ptr(str))
        throw std::out_of_range{"string '"s + detail::to_ascii(str) + "' does not represent a `"s +
                                rtti::type_name<Cast_t> + "` value"};

    try {
        return gsl::narrow<Cast_t>(value);
    } catch (gsl::narrowing_error&) {
        throw std::out_of_range{"value in string '"s + detail::to_ascii(str) +
                                "' is out of range for `"s + rtti::type_name<Cast_t> + "`"};
    } catch(...) {
        throw;
    }
}

template <class Cast_t, class String_span_t>
auto string_to(const String_span_t& str)
    -> stdx::First_of<Cast_t,
                      std::enable_if_t<stdx::is_arithmetic_v<Cast_t>>,
                      decltype(gslx::as_const_basic_string_span(str))>
{
    return string_to<Cast_t>(gsl::to_string(gslx::as_const_basic_string_span(str)));
}

namespace detail {
template <class Tuple_t, class Char_t, class Traits, class Allocator, std::size_t... I>
auto string_to_tuple(const std::basic_string<Char_t, Traits, Allocator>& str,
                     std::index_sequence<I...>) -> Tuple_t
{
    const auto tokens = get_split(str);
    return Tuple_t{string_to<stdx::Tuple_get_t<Tuple_t, I>>(tokens[I])...};
}
}

template <class... Cast_ts, class Char_t, class Traits, class Allocator>
auto string_to(const std::basic_string<Char_t, Traits, Allocator>& str)
    -> stdx::First_of<std::tuple<Cast_ts...>, std::enable_if_t<(sizeof...(Cast_ts) > 1)>>
{
    return detail::string_to_tuple<std::tuple<Cast_ts...>>(
        str, std::make_index_sequence<sizeof...(Cast_ts)>());
}

} // namespace str
} // namespace bolov
