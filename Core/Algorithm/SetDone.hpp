#pragma once

namespace std::execution
{
    namespace set_done_n
    {
        template <typename R>
        concept default_impl =
            requires(R&& r)
            {
                { forward<R>(r).set_done() } noexcept;
            };

        template <typename R>
        concept customise_point =
            requires(R&& r)
            {
                { set_done(forward<R>(r)) } noexcept;
            };

        struct func_type
        {
            template <typename R> requires(default_impl<R> && !customise_point<R>)
            decltype(auto) operator() (R&& r) const noexcept
            {
                return forward<R>().set_done();
            }

            template <typename R> requires(customise_point<R>)
            decltype(auto) operator() (R&& r) const noexcept
            {
                return set_done(forward<R>(r));
            }
        };

        inline constexpr func_type set_done{};
    }

    using set_done_n::set_done;
}