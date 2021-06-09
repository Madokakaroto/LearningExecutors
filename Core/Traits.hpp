#pragma once

namespace std::execution
{
    template<template
    <
        template <typename...> class Tuple,
        template <typename ...> class Variant
    > class>
    struct has_value_types;

    template <template <template <typename...> class> class>
    struct has_error_types;

    template <typename T, template <typename ...> class Tmpl>
    struct is_instance_of : false_type {};
    template <template <typename ...> class Tmpl, typename ... Args>
    struct is_instance_of<Tmpl<Args...>, Tmpl> : true_type {};
    template <typename T, template <typename ...> class Tmpl>
    inline constexpr bool is_instance_of_v = is_instance_of<T, Tmpl>::value;

    template <typename S>
    struct sender_traits;
}