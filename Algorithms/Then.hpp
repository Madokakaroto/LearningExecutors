#pragma once

namespace std::execution
{
    namespace then_n
    {
        template <receiver R, typename F>
        struct _receiver_type
        {
            R r_;
            F f_;

            // Customize set_value by invoking the callable and passing the result to the base class
            template <typename ... Args> requires receiver_of<R, invoke_result_t<F, Args...>>
            void set_value(Args&& ... args) &&
                //noexcept(is_nothrow_invocable_v<decltype(execution::set_value), _receiver_type&&, F&&, Args...>)
                noexcept(is_nothrow_invocable_v<F, Args...> && is_nothrow_receiver_of_v<R, invoke_result_t<F, Args...>>)
            {
                execution::set_value(move(r_), invoke(move(f_, forward<Args>(args)...)));
            }

            template <typename ... Args> requires receiver_of<R> && is_void_v<invoke_result_t<F, Args...>>
            void set_value(Args&& ... args) &&
                noexcept(is_nothrow_invocable_v<F, Args...> && is_nothrow_receiver_of_v<R>)
            {
                invoke(move(f_), forward<Args>(args)...);
                execution::set_value(move(r_));
            }

            template <typename E> requires receiver<R, remove_cvref_t<E>>
            void set_error(E&& e) && noexcept
            {
                execution::set_error(move(r_), forward<E>(e));
            }

            void set_done() && noexcept
            {
                execution::set_done(move(r_));
            }
        };

        template <sender S, typename F>
        struct _sender_type : sender_base
        {
            S s_;
            F f_;

            template <receiver R> requires(sender_to<S, _receiver_type<R, F>>)
            auto connect(R r) && -> connect_result_t<S, _receiver_type<R, F>>
            {
                using receiver_type = _receiver_type<R, F>;
                return execution::connect(move(s_), receiver_type{ move(r), move(f_) });
            }
        };

        template <typename S, typename F>
        concept customise_point =
            requires(S s, F f)
            {
                then(move(s), move(f));
            };

        constexpr struct func_type
        {
            // default implementation no constraints
            template <typename S, typename F>
            auto operator()(S s, F f) const noexcept
            {
                using sender_type = _sender_type<S, F>;
                return sender_type{ {}, move(s), move(f) };
            }

            template <typename S, typename F> requires customise_point<S, F>
            decltype(auto) operator()(S s, F f) const noexcept(noexcept(then(move(s), move(f))))
            {
                return then(move(s), move(f));
            }
        } then{};
    }

    using then_n::then;
}