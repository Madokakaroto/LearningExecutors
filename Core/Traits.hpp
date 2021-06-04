#pragma once

namespace std::execution
{
    template <has_sender_types S>
    struct sender_traits<S>
    {
        template
        <
            template <typename...> class Tuple,
            template <typename...> class Variant
        >
        using value_types = typename S::template value_types<Tuple, Variant>;

        template <template <typename...> class Variant>
        using error_types = typename S::template error_types<Variant>;

        static constexpr bool sends_done = S::sends_done;
    };

    template <typename T> requires (has_sender_types<T> && executor_of_impl<T, invocable_archetype>)
    struct sender_traits<T>
    {
        template
        <
            template <typename...> class Tuple,
            template <typename...> class Variant
        >
        using value_types = Variant<Tuple<>>;

        template <template <typename...> class Variant>
        using error_types = Variant<exception_ptr>;
    };

    template <is_sender_base S> requires (!has_sender_types<S>)
    struct sender_traits<S> {};
}