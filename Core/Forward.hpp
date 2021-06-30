#pragma once

namespace std::execution
{
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

    namespace connect_n
    {
        extern struct func_type const connect;
    }
    using connect_n::connect;

    namespace execute_n
    {
        extern struct func_type const execute;
    }
    using execute_n::execute;
}