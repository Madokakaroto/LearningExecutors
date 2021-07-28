#pragma once

namespace std::execution
{
    template <typename R, typename ... Vs> requires(receiver_of<R, Vs...>)
    struct _value_receiver
    {
        using tuple_value_t = tuple<remove_cvref_t<Vs>...>;

        R r_;
        tuple_value_t value_;

        template <typename ... Args> requires(is_nothrow_constructible_v<tuple_value_t, Args&&...>)
        _value_receiver(R&& r, Args&& ... args)
            : r_(move(r))
            , value_{ forward<Args>(args)... }
        {}

        void set_value() && noexcept
        {
            apply([this](Vs&& ... vs) mutable noexcept
            {
                execution::set_value(move(r_), move(vs)...);
            }, move(value_));
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