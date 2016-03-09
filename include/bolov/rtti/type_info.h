#pragma once

#include <cstdlib>

#include <string>
#include <memory>
#include <typeinfo>
#include <algorithm>

#include <cxxabi.h>

namespace bolov {
namespace rtti {

inline auto demangle(const std::string& mangled_name) -> std::string
{
    int status;
    auto demangled_uptr = std::unique_ptr<char, decltype(::free)*>{
        ::abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status), ::free};
    return status == 0 ? demangled_uptr.get() : mangled_name;
}

namespace detail {

template <class T> struct Pixie {};

inline auto disenchant(const std::string& enchanted_name) -> std::string
{
    auto first = enchanted_name.find('<') + 1;
    auto last = enchanted_name.rfind('>');
    return enchanted_name.substr(first, last - first);
}

template <class T>
auto type_name_enchanted = demangle(typeid(Pixie<T>).name());
}

template <class T>
auto type_name = detail::disenchant(detail::type_name_enchanted<T>);
}
}

