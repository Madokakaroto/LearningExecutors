#pragma once

namespace std::execution
{
    template <typename R, typename ... Vs> requires(receiver_of<R, Vs...>)
    struct value_receiver
    {
        R r_;
        std::tuple<Vs...> vs_;

        explicit(sizeof...(Vs) == 1) value_receiver(R&& r, Vs&& ... vs)
            : r_(move(r))
            , vs_({ forward<Vs>(vs)... })
        {}

        void set_value() noexcept(is_nothrow_invocable_v<decltype(execution::set_value), R&&, Vs&&...>)
        {
            apply(execution::set_value, move(r_), move(vs_));
        }

        template <typename E>
        void set_error(E&& e) noexcept
        {
            execution::set_error(move(r_), forward<E>(e));
        }

        void set_done() noexcept
        {
            execution::set_done(move(r_));
        }
    };
}