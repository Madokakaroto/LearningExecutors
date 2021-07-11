#pragma once

namespace std::execution
{
    template <typename R, typename E>
    struct error_receiver
    {
        R r_;
        E e_;

        error_receiver(R&& r, E&& e)
            : r_(move(r))
            , e_(move(e))
        {}

        void set_value() noexcept
        {
            execution::set_error(move(r_), move(e_));
        }

        template <typename Error>
        void set_error(Error&& e) noexcept
        {
            execution::set_error(move(r_), forward<Error>(e));
        }

        void set_done() noexcept
        {
            execution::set_done(move(r_));
        }
    };
}