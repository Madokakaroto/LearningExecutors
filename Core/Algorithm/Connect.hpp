#pragma once

namespace std::execution
{
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

        template <typename S, typename R>
        concept as_operation_impl =
            negation_v<is_instance_of<remove_cvref_t<R>, as_invocable>> &&
            receiver_of<R> &&
            executor_of_impl<remove_cvref_t<S>, as_invocable<remove_cvref_t<R>, S>>;

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

            template <typename S, typename R>
                requires(as_operation_impl<S, R> && !(default_impl<S, R> && customise_point<S, R>))
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(as_operation<S, R>{ forward<S>(s), forward<R>(r) }))
            {
                return as_operation<S, R>{ forward<S>(s), forward<R>(r) };
            }
        };

        inline func_type const connect{};
    }
}