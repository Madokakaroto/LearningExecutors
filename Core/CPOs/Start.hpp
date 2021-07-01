#pragma once

namespace std::execution
{
    namespace start_n
    {
        template <typename O>
        concept default_impl =
            requires(O&& o)
            {
                forward<O>(o).start();
            };

        template <typename O>
        concept customise_point =
            requires(O&& o)
            {
                start(forward<O>(o));
            };

        struct func_type
        {
            template <typename O> requires(default_impl<O> && !customise_point<O>)
            void operator() (O&& o) const noexcept(noexcept(forward<O>(o).start()))
            {
                forward<O>(o).start();
            }

            template <typename O> requires(customise_point<O>)
            void operator() (O&& o) const noexcept(noexcept(start(forward<O>(o))))
            {
                start(forward<O>(o));
            }
        };

        inline constexpr func_type start{};
    }

    using start_n::start;
}