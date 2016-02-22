#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include <bolov/stdx.h>

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
}
}
