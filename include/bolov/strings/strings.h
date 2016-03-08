#pragma once

#include <cctype>

#include <utility>
#include <vector>
#include <iterator>

#include <gsl.h>

#include <bolov/gslx.h>

namespace bolov {
namespace str {

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
auto displace(T&& views)
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

}
}
