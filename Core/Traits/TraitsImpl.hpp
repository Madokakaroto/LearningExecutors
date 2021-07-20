#pragma once

namespace std::execution
{
    // impl of sender traits
    template <typename S> requires(has_sender_types<S>)
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

    template <typename S> requires(!has_sender_types<S> && executor<S>)
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

    template <typename S> requires (!has_sender_types<S> && is_sender_base<S>)
    struct sender_traits<S> {};
    // end of impl of sender traits

    template <typename S, typename R>
        requires(requires(S&& s, R&& r)
        {
            { execution::connect(forward<S>(s), forward<R>(r)) } -> operation_state;
        })
    struct is_connect_invocable<S, R> : true_type {};

    template <typename E, typename F>
        requires(requires(E&& e, F&& f)
        {
            execution::execute(forward<E>(e), forward<F>(f));
        })
    struct is_execute_invocable<E, F> : true_type{};

    template <typename O>
        requires(requires(O&& o)
        {
            execution::start(forward<O>(o));
        })
    struct is_start_invocable<O> : true_type {};

    template <typename E>
        requires(requires(E&& e)
        {
            execution::schedule(forward<E>(e));
        })
    struct is_schedule_invocable<E> : true_type {};
}