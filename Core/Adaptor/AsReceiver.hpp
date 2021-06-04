#pragma once

namespace std::execution
{
    template <typename F, typename>
    struct as_receiver
    {
        void set_value() noexcept(is_nothrow_invocable_v<F&>)
        {
            invoke(f_);
        }

        template <typename E>
        void set_error(E&&) noexcept
        {
            terminate();
        }

        void set_done() noexcept {}

        F f_;
    };
}