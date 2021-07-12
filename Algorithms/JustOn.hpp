#pragma once

namespace std::execution
{
    namespace just_on_n
    {
        template <typename Sch, typename ... Args>
        concept has_just_on_impl =
            requires(Sch&& sch, Args&& ... args)
            {
                forward<Sch>(sch).just_on(forward<Args>(args)...);
            };

        template <typename Sch, typename ... Args>
        concept customise_point =
            requires(Sch&& sch, Args&& ... args)
            {
                just_on(forward<Sch>(sch), forward<Args>(args)...);
            };

        template <typename Sch, typename ... Args>
        concept basic_constraints =
            scheduler<Sch> && (move_constructible<remove_cvref_t<Args>> && ...);

        struct func_type
        {
            template <typename Sch, typename ... Args>
                requires(basic_constraints<Sch, Args...> && customise_point<Sch, Args...>)
            decltype(auto) operator() (Sch&& sch, Args&& ... args) const
                noexcept(noexcept(just_on(declval<Sch>(), declval<Args>()...)))
            {
                return just_on(forward<Sch>(sch), forward<Args>(args)...);
            }

            template <typename Sch, typename ... Args> requires(
                basic_constraints<Sch, Args...> &&
                has_just_on_impl<Sch, Args...> &&
                !customise_point<Sch, Args...>)
            decltype(auto) operator() (Sch&& sch, Args&& ... args) const
                noexcept(noexcept(declval<Sch>().just_on(declval<Args>()...)))
            {
                return forward<Sch>(sch).just_on(forward<Args>()...);
            }

            template <typename Sch, typename ... Args> requires(
                basic_constraints<Sch, Args...> &&
                !has_just_on_impl<Sch, Args...> &&
                !customise_point<Sch, Args...>)
            decltype(auto) operator() (Sch&& sch, Args&& ... args) const
                noexcept(noexcept(execution::on(execution::just(forward<Args>(args)...), forward<Sch>(sch))))
            {
                return execution::on(execution::just(forward<Args>(args)...), forward<Sch>(sch));
            }
        };
    }

    inline constexpr just_n::func_type just_on{};
}