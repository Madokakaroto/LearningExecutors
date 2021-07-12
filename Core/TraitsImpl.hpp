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

    /*template <typename S, typename R>
    concept sender_to_impl =
        requires(S&& s, R&& r)
        {
            execution::connect(forward<S>(s), forward<R>(r));
        };

    template <typename S, typename R> requires sender_to_impl<S, R>
    struct is_sender_to_impl<S, R> : true_type {};*/

    /*template <typename E, typename F>
    concept impl_of_executor_of_impl =
        requires(E&& e, F&& f)
        {
            execution::execute(forward<E>(e), forward<F>(f));
        };*/

    //template <typename E, typename F>
    //struct is_executor_of_impl<E, F, void_t<decltype(execution::execute(declval<E>(), declval<F>()))>> : true_type {};
}