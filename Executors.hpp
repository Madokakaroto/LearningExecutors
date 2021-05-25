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

    struct sender_base {};

    template <typename S>
    concept is_sender_base = is_base_of_v<sender_base, S>;
}

// algorithm
namespace std::execution
{
    inline namespace impl
    {
        struct invocable_archetype
        {
            void operator()() & noexcept;
        };

        namespace set_value_n
        {
            template <typename R, typename ... Args>
            concept default_impl =
                requires(R&& r, Args&&... args)
                {
                    forward<R>(r).set_value(forward<Args>(args)...);
                };

            template <typename R, typename ... Args>
            concept customise_point =
                requires(R&& r, Args&&... args)
                {
                    set_value(forward<R>(r), forward<Args>(args)...);
                };

            template <typename R, typename ... Args>
            concept default_exclude_customise =
                default_impl<R, Args...> && !customise_point<R, Args...>;

            struct func_type
            {
                template <typename R, typename ... Args> requires (default_exclude_customise<R, Args...>)
                decltype(auto) operator() (R&& r, Args&& ... args) const
                    noexcept(noexcept(forward<R>(r).set_value(forward<Args>(args)...)))
                {
                    return forward<R>(r).set_value(forward<Args>(args)...);
                }

                template <typename R, typename ... Args> requires (customise_point<R, Args...>)
                decltype(auto) operator() (R&& r, Args&& ... args) const
                    noexcept(noexcept(set_value(forward<R>(r), forward<Args>(args)...)))
                {
                    return set_value(forward<R>(r), forward<Args>(args)...);
                }
            };
        }

        namespace set_error_n
        {
            template <typename R, typename E = exception_ptr>
            concept default_impl =
                requires(R&& r, E&& e)
                {
                    { forward<R>(r).set_error(forward<E>(e)) } noexcept;
                };

            template <typename R, typename E = exception_ptr>
            concept customise_point =
                requires(R&& r, E&& e)
                {
                    { set_error(forward<R>(r), forward<E>(e)) } noexcept;
                };

            template <typename R, typename E = exception_ptr>
            concept default_exclude_customise =
                default_impl<R, E> && !customise_point<R, E>;

            struct func_type
            {
                template <typename R, typename E> requires(default_exclude_customise<R, E>)
                decltype(auto) operator() (R&& r, E&& e) const noexcept
                {
                    return forward<R>(r).set_error(forward<E>(e));
                }

                template <typename R, typename E> requires(customise_point<R, E>)
                decltype(auto) operator() (R&& r, E&& e) const noexcept
                {
                    return set_error(forward<R>(r), forward<E>(e));
                }
            };
        }

        struct set_done_t
        {
            template <typename R> requires
                requires(R&& r) { { forward<R>(r).set_done() } noexcept; }
            decltype(auto) operator() (R&& r) const noexcept
            {
                return move(r).set_done();
            }
        };

        namespace set_done_n
        {
            template <typename R>
            concept default_impl =
                requires(R&& r)
                {
                    { forward<R>(r).set_done() } noexcept;
                };

            template <typename R>
            concept customise_point =
                requires(R&& r)
                {
                    { set_done(forward<R>(r)) } noexcept;
                };

            template <typename R>
            concept default_exclude_customise =
                default_impl<R> && !customise_point<R>;

            struct func_type
            {
                template <typename R> requires(default_exclude_customise<R>)
                decltype(auto) operator() (R&& r) const noexcept
                {
                    return forward<R>().set_done();
                }

                template <typename R> requires(customise_point<R>)
                decltype(auto) operator() (R&& r) const noexcept
                {
                    return set_done(forward<R>(r));
                }
            };
        }

        inline constexpr set_value_n::func_type set_value{};
        inline constexpr set_error_n::func_type set_error{};
        inline constexpr set_done_n::func_type set_done{};
    }
}

// internal
namespace std::execution
{
    template <typename R, class>
    struct as_invocable
    {
        explicit as_invocable(R& r) noexcept
        : r_(addressof(r)) {}

        as_invocable(as_invocable&& other) noexcept
        : r_(exchange(other.r_, nullptr)) {}

        ~as_invocable()
        {
            if(r_)
            {
                execution::set_done(move(*r_));
            }
        }

        void operator()() & noexcept
        {
            try
            {
                execution::set_value(move(*r_));
            }
            catch (...)
            {
                execution::set_error(move(*r_), current_exception());
            }
        }

        R* r_;
    };
}

namespace std::execution
{
    template <typename R, typename E = exception_ptr>
    concept receiver =
        move_constructible<remove_cvref_t<R>> &&
        constructible_from<remove_cvref_t<R>, R> &&
        requires(remove_cvref_t<R>&& r, E&& e)
        {
            { execution::set_done(move(r)) } noexcept;
            { execution::set_error(move(r), forward<E>(e)) } noexcept;
        };

    template <typename R, typename ... Args>
    concept receiver_of =
        receiver<R> &&
        requires(remove_cvref_t<R>&& r, Args&& ... args)
        {
            execution::set_value(move(r), forward<Args>(args)...);
        };

    template <typename R, typename ... Args>
    concept nothrow_receiver_of =
        receiver_of<R, Args...> &&
        is_nothrow_invocable_v<decltype(set_value), Args...>;

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
            connect(move(s), move(r));
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
            execute(e, forward<F>(f));
        };

    template <typename E>
    concept executor = executor_of_impl<E, invocable_archetype>;

    template <typename O>
    concept operation_state =
        destructible<O> &&
        is_object_v<O> &&
        requires(O& o)
        {
            { start(o) } noexcept;
        };
}

// forward impl
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