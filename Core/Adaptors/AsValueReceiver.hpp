#pragma once

namespace std::execution
{
    template <typename R, typename ... Vs> //requires(receiver_of<R, Vs...>)
    struct value_receiver
    {
        std::tuple<R, Vs...> rvs_;

        explicit(sizeof...(Vs) == 0) value_receiver(R&& r, Vs&& ... vs)
            : rvs_{ move(r), move(vs)... }
        {}

        void set_value() && noexcept(is_nothrow_invocable_v<decltype(execution::set_value), R&&, Vs&&...>)
        {
            apply(execution::set_value, move(rvs_));
        }

        template <typename E>
        void set_error(E&& e) && noexcept
        {
            execution::set_error(get<0>(move(rvs_)), forward<E>(e));
        }

        void set_done() && noexcept
        {
            execution::set_done(get<0>(move(rvs_)));
        }
    };
}