#pragma once

#ifdef USE_STD_CONCEPTS_LIB
#include <concepts>
#else
namespace std
{
    template <typename T>
    concept destructible = is_nothrow_destructible_v<T>;

    template <typename T, typename ... Args>
    concept constructible_from =
        destructible<T> &&
        is_constructible_v<T, Args...>;

    template <typename From, typename To>
    concept convertible_to =
        is_convertible_v<From, To> &&
        requires(add_rvalue_reference_t<From> from)
        {
            static_cast<To>(from);
        };

    template <typename T>
    concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

    template <typename T>
    concept copy_constructible =
        move_constructible<T> &&
        constructible_from<T, T&> && convertible_to<T&, T> &&
        constructible_from<T, T const&> && convertible_to<T const&, T> &&
        constructible_from<T, T const> && convertible_to<T const, T>;

    namespace detail
    {
        template <typename T>
        concept boolean_testable_impl = convertible_to<T, bool>;
    }

    template <typename T>
    concept boolean_testable =
        detail::boolean_testable_impl<T> &&
        requires(T&& t)
        {
            { !forward<T>(t) } -> detail::boolean_testable_impl;
        };

    namespace detail
    {
        template <typename T, typename U>
        concept weak_equality_compareable = requires(
            add_lvalue_reference_t<add_const_t<remove_cvref_t<T>>> t,
        add_lvalue_reference_t<add_const_t<remove_cvref_t<U>>> u)
        {
            { t == u } -> boolean_testable;
            { t != u } -> boolean_testable;
            { u == t } -> boolean_testable;
            { u != t } -> boolean_testable;
        };
    }

    template <typename T>
    concept equality_comparable = detail::weak_equality_compareable<T, T>;

    template <typename F, typename ... Args>
    concept invocable =
        requires (F&& f, Args&& ... args)
        {
            invoke(forward<F>(f), forward<Args>(args)...);
        };
}
#endif

// extended concepts
namespace std
{
    template <typename F, typename ... Args>
    concept nothrow_invocable =
        requires(F&& f, Args&& ... args)
        {
            { invoke(forward<F>(f), forward<Args>(args)...) } noexcept;
        };
}
