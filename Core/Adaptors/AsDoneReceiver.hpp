#pragma once

namespace std::execution
{
    template <typename R>
    struct done_receiver
    {
        R r_;

        explicit done_receiver(R&& r)
            : r_(move(r))
        {}

        void set_value() && noexcept
        {
            execution::set_done(move(r_));
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