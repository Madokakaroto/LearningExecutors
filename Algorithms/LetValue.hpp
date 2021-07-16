#pragma once

namespace std::execution
{
    namespace let_value_n
    {
        template <typename S, typename F>
        concept has_let_value_impl =
            requires(S&& s, F&& f)
            {
                forward<S>(s).let_value(forward<F>(f));
            };

        template <typename S, typename F>
        concept customise_point =
            requires(S&& s, F&& f)
            {
                let_value(forward<S>(s), forward<F>(f));
            };

        struct func_type
        {
            template <sender S, typename F> requires(customise_point<S, F>)
            decltype(auto) operator() (S&& s, F&& f) const
                noexcept(noexcept(let_value(declval<S>(), declval<F>())))
            {
                return let_value(forward<S>(s), forward<F>(f));
            }

            template <sender S, typename F>
                requires(!customise_point<S, F> && has_let_value_impl<S, F>)
            decltype(auto) operator() (S&& s, F&& f) const
                noexcept(noexcept(declval<S>().let_value(declval<F>())))
            {
                return forward<S>(s).let_value(forward<F>(f));
            }

            template <sender S, typename F>
            decltype(auto) operator() (S&& s, F&& f) const noexcept
            {
                return execution::on(
                    execution::transform(forward<S>(s), forward<F>(f)),
                    execution::immediate_context.get_scheduler());
            }

            template <typename F>
            auto operator() (F&& f) const noexcept
            {
                return _pipe_operand_type{ *this, forward<F>(f) };
            }
        };
    }

    inline constexpr let_value_n::func_type let_value{};
}