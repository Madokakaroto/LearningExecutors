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

    template <typename S>
    struct sender_traits;

    struct sender_base {};

    struct invocable_archetype { void operator()() & noexcept; };

    namespace connect_n
    {
        extern inline const struct func_type connect;
    }
    using connect_n::connect;

    namespace execute_n
    {
        extern inline const struct func_type execute;
    }
    using execute_n::execute;
}