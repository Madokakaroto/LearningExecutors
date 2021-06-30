#pragma once

namespace std::execution
{
    template <typename S>
    struct sender_traits
    {
        using __unspecialized = void;
    };

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

    template <typename S> requires(!has_sender_types<S> && executor_of_impl<S, as_invocable<void_receiver, S>>)
    //template <typename S> requires (!has_sender_types<S> && executor_of_impl<S, invocable_archetype>)
    struct sender_traits<S>
    {
        template
        <
            template <typename...> class Tuple,
            template <typename...> class Variant
        >
        using value_types = Variant<Tuple<>>;

        template <template <typename...> class Variant>
        using error_types = Variant<exception_ptr>;

        static constexpr bool sends_done = true;
    };

    template <is_sender_base S> requires (!has_sender_types<S>)
    struct sender_traits<S> {};

    template <typename S, typename R>
    using connect_result_t = invoke_result_t<decltype(connect), S, R>;
}