#pragma once

namespace std::execution
{
    struct sender_base {};

    struct invocable_archetype { void operator()() & noexcept; };

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