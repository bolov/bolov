#pragma once

#include <cstddef>
#include <utility>
#include <string>
#include <initializer_list>
#include <type_traits>

namespace bolov {
namespace stdx {

// --- void_t, First_of
namespace detail {
template <class T, class... Args>
struct First_of_impl {
    using First_t = T;
};
}

/*
 * Similar to void_t, exept it maps to the first argument, instead of void
 *
 * especially useful for SFINAE via function return type
 */
template <class... Args>
using First_of = typename detail::First_of_impl<Args...>::First_t;

template <class... Args>
using void_t = First_of<void, Args...>;

// ---- _v templated variables

template <class T>
constexpr bool is_void_v = std::is_void<T>::value;

template <class T>
constexpr bool is_null_pointer_v = std::is_null_pointer<T>::value;
template <class T>
constexpr bool is_integral_v = std::is_integral<T>::value;
template <class T>
constexpr bool is_floating_point_v = std::is_floating_point<T>::value;
template <class T>
constexpr bool is_array_v = std::is_array<T>::value;
template <class T>
constexpr bool is_pointer_v = std::is_pointer<T>::value;
template <class T>
constexpr bool is_lvalue_reference_v = std::is_lvalue_reference<T>::value;
template <class T>
constexpr bool is_rvalue_reference_v = std::is_rvalue_reference<T>::value;
template <class T>
constexpr bool is_member_object_pointer_v = std::is_member_object_pointer<T>::value;
template <class T>
constexpr bool is_member_function_pointer_v = std::is_member_function_pointer<T>::value;
template <class T>
constexpr bool is_enum_v = std::is_enum<T>::value;
template <class T>
constexpr bool is_union_v = std::is_union<T>::value;
template <class T>
constexpr bool is_class_v = std::is_class<T>::value;
template <class T>
constexpr bool is_function_v = std::is_function<T>::value;

template <class T>
constexpr bool is_reference_v = std::is_reference<T>::value;
template <class T>
constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;
template <class T>
constexpr bool is_fundamental_v = std::is_fundamental<T>::value;
template <class T>
constexpr bool is_object_v = std::is_object<T>::value;
template <class T>
constexpr bool is_scalar_v = std::is_scalar<T>::value;
template <class T>
constexpr bool is_compound_v = std::is_compound<T>::value;
template <class T>
constexpr bool is_member_pointer_v = std::is_member_pointer<T>::value;

template <class T>
constexpr bool is_const_v = std::is_const<T>::value;
template <class T>
constexpr bool is_volatile_v = std::is_volatile<T>::value;
template <class T>
constexpr bool is_trivial_v = std::is_trivial<T>::value;
template <class T>
constexpr bool is_trivially_copyable_v = std::is_trivially_copyable<T>::value;
template <class T>
constexpr bool is_standard_layout_v = std::is_standard_layout<T>::value;
template <class T>
constexpr bool is_pod_v = std::is_pod<T>::value;
template <class T>
constexpr bool is_literal_type_v = std::is_literal_type<T>::value;
template <class T>
constexpr bool is_empty_v = std::is_empty<T>::value;
template <class T>
constexpr bool is_polymorphic_v = std::is_polymorphic<T>::value;
template <class T>
constexpr bool is_abstract_v = std::is_abstract<T>::value;
template <class T>
constexpr bool is_final_v = std::is_final<T>::value;
template <class T>
constexpr bool is_signed_v = std::is_signed<T>::value;
template <class T>
constexpr bool is_unsigned_v = std::is_unsigned<T>::value;

template <class T, class... Args>
constexpr bool is_constructible_v = std::is_constructible<T, Args...>::value;
template <class T, class... Args>
constexpr bool is_trivially_constructible_v = std::is_trivially_constructible<T, Args...>::value;
template <class T, class... Args>
constexpr bool is_nothrow_constructible_v = std::is_nothrow_constructible<T, Args...>::value;
template <class T>
constexpr bool is_default_constructible_v = std::is_default_constructible<T>::value;
template <class T>
constexpr bool is_trivially_default_constructible_v =
    std::is_trivially_default_constructible<T>::value;
template <class T>
constexpr bool is_nothrow_default_constructible_v = std::is_nothrow_default_constructible<T>::value;
template <class T>
constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;
template <class T>
constexpr bool is_trivially_copy_constructible_v = std::is_trivially_copy_constructible<T>::value;
template <class T>
constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;
template <class T>
constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;
template <class T>
constexpr bool is_trivially_move_constructible_v = std::is_trivially_move_constructible<T>::value;
template <class T>
constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;
template <class T, class U>
constexpr bool is_assignable_v = std::is_assignable<T, U>::value;
template <class T, class U>
constexpr bool is_trivially_assignable_v = std::is_trivially_assignable<T, U>::value;
template <class T, class U>
constexpr bool is_nothrow_assignable_v = std::is_nothrow_assignable<T, U>::value;
template <class T>
constexpr bool is_copy_assignable_v = std::is_copy_assignable<T>::value;
template <class T>
constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable<T>::value;
template <class T>
constexpr bool is_nothrow_copy_assignable_v = std::is_nothrow_copy_assignable<T>::value;
template <class T>
constexpr bool is_move_assignable_v = std::is_move_assignable<T>::value;
template <class T>
constexpr bool is_trivially_move_assignable_v = std::is_trivially_move_assignable<T>::value;
template <class T>
constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;
template <class T>
constexpr bool is_destructible_v = std::is_destructible<T>::value;
template <class T>
constexpr bool is_trivially_destructible_v = std::is_trivially_destructible<T>::value;
template <class T>
constexpr bool is_nothrow_destructible_v = std::is_nothrow_destructible<T>::value;
template <class T>
constexpr bool has_virtual_destructor_v = std::has_virtual_destructor<T>::value;

template <class T>
constexpr std::size_t alignment_of_v = std::alignment_of<T>::value;
template <class T>
constexpr std::size_t rank_v = std::rank<T>::value;
template <class T, unsigned I = 0>
constexpr std::size_t extent_v = std::extent<T, I>::value;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;
template <class Base, class Derived>
constexpr bool is_base_of_v = std::is_base_of<Base, Derived>::value;
template <class From, class To>
constexpr bool is_convertible_v = std::is_convertible<From, To>::value;

// --- type traits

template <class T, class A1, class... Args>
constexpr auto is_one_of = stdx::is_same_v<T, A1> || is_one_of<T, Args...>;

template <class T, class A1>
constexpr auto is_one_of<T, A1> = is_same_v<T, A1>;

template <class T>
using remove_cv_reference_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
constexpr auto is_char = is_one_of<remove_cv_reference_t<T>, char, unsigned char, signed char,
                                   wchar_t, unsigned wchar_t, signed wchar_t, char16_t, char32_t>;
// --- c_array_t

template <class T, std::size_t N>
using c_array_t = T[N];

// --- size()

template <class C>
constexpr auto size(const C& c) -> decltype(c.size())
{
    return c.size();
}
template <class T, std::size_t N>
constexpr std::size_t size(c_array_t<const T,N>&) noexcept
{
    return N;
}

// -- data()
template <class C>
constexpr auto data(C& c)
{
    return c.data();
}
template <class C>
constexpr auto data(const C& c)
{
    return c.data();
}
template <class T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept
{
    return array;
}
template <class E>
constexpr const E* data(std::initializer_list<E> il) noexcept
{
    return il.begin();
}
template <class Char_t, class Traits, class Allocator>
constexpr auto data(std::basic_string<Char_t, Traits, Allocator>& str)
{
    return &*str.begin();
}

// --- begin, end ptr

template <class C> auto begin_ptr(const C& c) { return data(c); }
template <class C> auto begin_ptr(C& c)       { return data(c); }
template <class C> auto end_ptr(const C& c)   { return data(c) + size(c); }
template <class C> auto end_ptr(C& c)         { return data(c) + size(c); }
}
}
