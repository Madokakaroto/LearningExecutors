#pragma once

namespace std::execution
{
    namespace on_n
    {
        template <typename S, typename Sch>
        concept has_on_impl =
            requires(S&& s, Sch&& sch)
            {
                forward<S>(s).on(forward<Sch>(sch));
            };

        template <typename S, typename Sch>
        concept customise_point_impl =
            requires(S&& s, Sch&& sch)
            {
                on(forward<S>(s), forward<Sch>(sch));
            };

        struct func_type
        {
            template <sender S, scheduler Sch>
            auto operator() (S&& s, Sch&& sch) const noexcept
            {
                return _propagate_sender{ forward<S>(s), sch.schedule() };
            }

            template <sender S, scheduler Sch> requires(has_on_impl<S, Sch> && !customise_point_impl<S, Sch>)
            decltype(auto) operator() (S&& s, Sch&& sch) const
                noexcept(noexcept(declval<S>().on(declval<Sch>())))
            {
                return forward<S>(s).on(forward<Sch>(sch));
            }

            template <sender S, scheduler Sch> requires(customise_point_impl<S, Sch>)
            decltype(auto) operator() (S&& s, Sch&& sch) const
                noexcept(noexcept(on(declval<S>(), declval<Sch>())))
            {
                return on(forward<S>(s), forward<Sch>(sch));
            }

            template <scheduler Sch>
            auto operator() (Sch&& sch) const noexcept
            {
                return _pipe_operand_type{ *this, forward<Sch>(sch) };
            }
        };
    }

    inline constexpr on_n::func_type on{};

    template <typename S, typename Sch>
    using on_result_t = invoke_result_t<decltype(execution::on), S, Sch>;
}