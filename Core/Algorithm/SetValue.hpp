#pragma once

namespace std::execution
{
    namespace set_value_n
    {
        template <typename R, typename ... Args>
        concept default_impl =
            requires(R&& r, Args&&... args)
            {
                forward<R>(r).set_value(forward<Args>(args)...);
            };

        template <typename R, typename ... Args>
        concept customise_point =
            requires(R&& r, Args&&... args)
            {
                set_value(forward<R>(r), forward<Args>(args)...);
            };

        struct func_type
        {
            template <typename R, typename ... Args> requires (default_impl<R, Args...>)
            decltype(auto) operator() (R&& r, Args&& ... args) const
                noexcept(noexcept(forward<R>(r).set_value(forward<Args>(args)...)))
            {
                return forward<R>(r).set_value(forward<Args>(args)...);
            }

            template <typename R, typename ... Args> requires (customise_point<R, Args...> && !default_impl<R, Args...>)
            decltype(auto) operator() (R&& r, Args&& ... args) const
                noexcept(noexcept(set_value(forward<R>(r), forward<Args>(args)...)))
            {
                return set_value(forward<R>(r), forward<Args>(args)...);
            }
        };

        inline constexpr func_type set_value{};
    }

    using set_value_n::set_value;
}