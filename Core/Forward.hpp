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

    struct invocable_archetype
    {
        void operator()() & noexcept;
    };
}