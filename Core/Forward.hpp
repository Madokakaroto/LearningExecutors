#pragma once

namespace std::execution
{
    struct sender_base {};

    struct invocable_archetype
    {
        invocable_archetype() = delete;
        void operator()() & noexcept;
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