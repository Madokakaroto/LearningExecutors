#pragma once

namespace std::execution
{
    template <typename R, typename ... Vs> requires receiver_of<R, Vs...>
    struct value_receiver
    {
        R r_;
        std::tuple<remove_cvref_t<Vs>...> vs_;

        value_receiver(R&& r, Vs ... vs)
            : r_{ move(r) }
            , vs_(forward<Vs>(vs)...)
        {}

        void set_value() && //noexcept(is_nothrow_invocable_v<decltype(execution::set_value), R&&, Vs&&...>)
        {
            apply([this](Vs&& ... vs) mutable noexcept
            {
                execution::set_value(move(r_), forward<Vs>(vs)...);
            }, move(vs_));
        }

        template <typename E>
        void set_error(E&& e) && noexcept
        {
            execution::set_error(move(r_), forward<E>(e));
        }

        void set_done() && noexcept
        {
            execution::set_done(move(r_));
        }
    };

    template <typename R> requires receiver<R>
    struct value_receiver<R>
    {
        R r_;

        value_receiver(R&& r)
            : r_(move(r))
        {}

        void set_value() &&
        {
            execution::set_value(move(r_));
        }

        template <typename E>
        void set_error(E&& e) && noexcept
        {
            execution::set_error(move(r_), forward<E>(e));
        }

        void set_done() && noexcept
        {
            execution::set_done(move(r_));
        }
    };
}