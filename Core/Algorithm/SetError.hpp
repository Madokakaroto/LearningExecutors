#pragma once

namespace std::execution
{
    namespace set_error_n
    {
        template <typename R, typename E = exception_ptr>
        concept default_impl =
            requires(R&& r, E&& e)
            {
                { forward<R>(r).set_error(forward<E>(e)) } noexcept;
            };

        template <typename R, typename E = exception_ptr>
        concept customise_point =
            requires(R&& r, E&& e)
            {
                { set_error(forward<R>(r), forward<E>(e)) } noexcept;
            };

        struct func_type
        {
            template <typename R, typename E> requires(default_impl<R, E> && !customise_point<R, E>)
            decltype(auto) operator() (R&& r, E&& e) const noexcept
            {
                return forward<R>(r).set_error(forward<E>(e));
            }

            template <typename R, typename E> requires(customise_point<R, E>)
            decltype(auto) operator() (R&& r, E&& e) const noexcept
            {
                return set_error(forward<R>(r), forward<E>(e));
            }
        };

        inline constexpr func_type set_error{};
    }

    using set_error_n::set_error;
}