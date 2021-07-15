#pragma once

namespace std::execution
{
    using task_t = std::function<void()>;

    struct sender_base {};

    // The name execution::invocable_archetype is an implementation-defined type such that invocable<execution::invocable_archetype&> is true.
    // A program that creates an instance of execution::invocable_archetype is ill-formed.
    struct invocable_archetype
    {
        invocable_archetype() = delete;
        void operator()() & noexcept;
    };

    struct void_receiver // exposition only
    {
        void set_value() noexcept;
        void set_error(exception_ptr) noexcept;
        void set_done() noexcept;
    };

    template <typename, typename>
    struct as_receiver;

    template <typename, typename>
    struct as_invocable;

    // has value types
    template<template
    <
        template <typename...> class Tuple,
        template <typename ...> class Variant
    > class>
    struct has_value_types;

    // has error types
    template <template <template <typename...> class> class>
    struct has_error_types;

    // get value types
    template <typename ValueTypes>
    struct get_value_types;

    // get error types
    template <typename ErrorTypes>
    struct get_error_types;

    struct invalid_sender_traits {};

    template <typename S>
    struct sender_traits : invalid_sender_traits
    {
        using __unspecialized = void;
    };
}