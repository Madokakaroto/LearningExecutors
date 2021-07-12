#pragma once

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

    namespace connect_n
    {
        template <typename S, typename R>
        concept default_impl =
            receiver<R> &&
            sender<S> &&
            requires(S&& s, R&& r)
            {
                { forward<S>(s).connect(forward<R>(r)) } -> operation_state;
            };

        template <typename S, typename R>
        concept customise_point =
            receiver<R> &&
            sender<S> &&
            requires(S&& s, R&& r)
            {
                connect(forward<S>(s), forward<R>(r));
            };

        struct func_type
        {
            template <typename S, typename R>
                requires(default_impl<S, R> && !customise_point<S, R>)
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(forward<S>(s).connect(forward<R>(r))))
            {
                return forward<S>(s).connect(forward<R>(r));
            }

            template <typename S, typename R>
                requires(customise_point<S, R>)
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(connect(forward<S>(s), forward<R>(r))))
            {
                return connect(forward<S>(s), forward<R>(r));
            }

            /*template <typename S, typename R>
                requires(as_operation_impl<S, R> && !(default_impl<S, R> && customise_point<S, R>))
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(as_operation<S, R>{ forward<S>(s), forward<R>(r) }))
            {
                return as_operation<S, R>{ forward<S>(s), forward<R>(r) };
            }*/
        };

        inline constexpr func_type connect{};
    }

    using connect_n::connect;

    template <typename S, typename R>
    concept sender_to =
        sender<S> &&
        receiver<R> &&
        requires(S&& s, R&& r)
        {
            execution::connect(forward<S>(s), forward<R>(r));
        };

    template <typename S, typename R>
    using connect_result_t = invoke_result_t<decltype(execution::connect), S, R>;
}