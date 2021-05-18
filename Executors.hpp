#pragma once

// traits
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

    template<typename S>
    concept has_sender_types = requires
    {
        typename has_value_types<S::template value_types>;
        typename has_error_types<S::template error_types>;
        typename bool_constant<S::sends_done>;
    };

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

    struct sender_base {};

    template <typename S>
    concept is_sender_base = is_base_of_v<sender_base, S>;

    template <is_sender_base S> requires (!has_sender_types<S>)
    struct sender_traits<S> {};
}

// algorithm
namespace std::execution
{
    template <typename R, typename ... Args>
    inline auto set_value(R&& r, Args&& ... args)
        noexcept(noexcept(move(r).set_value(forward<Args>(args)...))) ->
        decltype(move(r).set_value(forward<Args>(args)...))
    {
        return move(r).set_value(forward<Args>(args)...);
    }

    template <typename R, typename E> requires
        requires(R&& r, E&& e) { { move(r).set_error(e) } noexcept; }
    inline decltype(auto) set_error(R&& r, E&& e) noexcept
    {
        return move(r).set_error(forward<E>(e));
    }

    template <typename R> requires
        requires(R&& r) { { move(r).set_done() } noexcept; }
    inline decltype(auto) set_done(R&& r) noexcept
    {
        return move(r).set_done();
    }
}

namespace std::execution
{
    template <typename R, typename E = exception_ptr>
    concept receiver =
        move_constructible<remove_cvref_t<R>> &&
        constructible_from<remove_cvref_t<R>, R> &&
        requires(remove_cvref_t<R>&& r, E&& e)
        {
            { set_done(move(r)) } noexcept;
            { set_error(move(r), forward<E>(e)) } noexcept;
        };

    template <typename R, typename ... Args>
    concept receiver_of =
        receiver<R> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            set_value(move(r), forward<Args>(args)...);
        };

    template <typename R, typename ... Args>
    concept nothrow_receiver_of =
        receiver_of<R, Args...> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            { set_value(move(r), forward<Args>(args)...) } noexcept;
        };

    template <typename R, typename ... Args>
    inline constexpr bool is_nothrow_receiver_of_v = nothrow_receiver_of<R, Args...>;

    template <typename S>
    concept sender =
        move_constructible<remove_cvref_t<S>> &&
        !requires
        {
            typename sender_traits<remove_cvref_t<S>>::__unspecialized;
        };

    template <typename S, typename R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        requires(S&& s, R&& r)
        {
            connect(std::move(s), std::move(r));
        };

    template <typename E, typename F>
    concept executor_of_impl =
        invocable<add_lvalue_reference_t<remove_cvref_t<F>>> &&
        constructible_from<remove_cvref_t<F>, F> &&
        move_constructible<remove_cvref_t<F>> &&
        copy_constructible<E> &&
        is_nothrow_copy_constructible_v<E> &&
        equality_comparable<E> &&
        requires(E const& e, F&& f)
        {
            execute(e, std::forward<F>(f));
        };
}