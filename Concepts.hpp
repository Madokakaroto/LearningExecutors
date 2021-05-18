#pragma once

#ifdef USE_STD_CONCEPTS_LIB
#include <concepts>
#else
namespace std
{
    template <typename T>
    concept destructible = std::is_nothrow_destructible_v<T>;

    template <typename T, typename ... Args>
    concept constructible_from =
        destructible<T> &&
        std::is_constructible_v<T, Args...>;

    template <typename T>
    concept move_constructible = std::is_move_constructible_v<T>;

    template <typename F, typename ... Args>
    concept invocable =
        requires (F&& f, Args&& ... args)
        {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
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
            { std::invoke(std::forward<F>(f), std::forward<Args>(args)...) } noexcept;
        };
}
