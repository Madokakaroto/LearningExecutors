#pragma once

namespace std::execution
{
    namespace execute_n
    {
        template <typename E, typename F>
        concept default_impl =
            requires(E&& e, F&& f)
            {
                forward<E>(e).execute(forward<F>(f));
            };

        template <typename E, typename F>
        concept customise_point =
            requires(E&& e, F&& f)
            {
                execute(forward<E>(e), forward<F>(f));
            };

        struct func_type
        {
            template <typename E, typename F>
                requires(lvalue_invocable<F> && customise_point<E, F>)
            decltype(auto) operator() (E&& e, F&& f) const
                noexcept(noexcept(execute(forward<E>(e), forward<F>(f))))
            {
                return execute(forward<E>(e), forward<F>(f));
            }

            template <typename E, typename F>
                requires(lvalue_invocable<F> && default_impl<E, F> && !customise_point<E, F>)
            decltype(auto) operator() (E&& e, F&& f) const
                noexcept(noexcept(forward<E>(e).execute(forward<F>(f))))
            {
                return forward<E>(e).execute(forward<F>(f));
            }

            template <typename S, typename F>
                requires(lvalue_invocable<F> && !default_impl<S, F> && !customise_point<S, F> &&
                    sender_to<S, as_receiver<remove_cvref_t<F>, S>>)
            decltype(auto) operator() (S&& s, F&& f) const
                // noexcept(noexcept(execution::submit(forward<S>(s), as_receiver<remove_cvref_t<F>, S>{ forward<S>(s) })))
            {
                using as_receiver_t = as_receiver<remove_cvref_t<F>, S>;
                return execution::submit(forward<S>(s), as_receiver_t{ forward<F>(f) });
            }
        };
    }

    inline constexpr execute_n::func_type execute{};

    template <typename E, typename F>
    using execute_result_t = invoke_result_t<decltype(execution::execute), E, F>;
}