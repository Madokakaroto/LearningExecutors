#pragma once

namespace std::execution
{
    namespace properties_detail
    {
        template <typename E, typename P>
        concept static_query = decay_t<P>::template static_query_v<decay_t<E>> == decay_t<P>::value();

        template <typename E, typename P>
        inline constexpr bool is_nothrow_static_query_v =
            is_nothrow_copy_constructible_v<decltype(decay_t<P>::template static_query_v<decay_t<E>>)>;
    }

    namespace require_concept_n
    {
        template <typename E, typename P>
        concept basic_requirements = is_applicable_property_v<decay_t<E>, decay_t<P>>
            && decay_t<P>::is_requirable_concept;

        template <typename E, typename P>
        concept has_member_require_concept =
            requires(E&& e, P&& p)
            {
                forward<E>(e).require_concept(forward<P>(p));
            };

        template <typename E, typename P>
        concept customise_point =
            requires(E&& e, P&& p)
            {
                require_concept(forward<E>(e), forward<P>(p));
            };

        struct func_type
        {
            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                properties_detail::static_query<E, P>)
            decltype(auto) operator() (E&& e, P&&) const noexcept
            {
                return forward<E>(e);
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                customise_point<E, P>)
            decltype(auto) operator() (E&& e, P&& p) const
                noexcept(noexcept(require_concept(forward<E>(e), forward<P>(p))))
            {
                return require_concept(forward<E>(e), forward<P>(p));
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                !customise_point<E, P> &&
                has_member_require_concept<E, P>)
            decltype(auto) operator() (E&& e, P&& p) const
                noexcept(noexcept(forward<E>(e).require_concept(forward<P>(p))))
            {
                return forward<E>(e).require_concept(forward<P>(p));
            }
        };

        inline constexpr func_type require_concept{};
    }

    namespace require_n
    {
        template <typename E, typename P>
        concept basic_requirements = is_applicable_property_v<decay_t<E>, decay_t<P>> &&
            decay_t<P>::is_requirable;

        template <typename E, typename P>
        concept has_member_require =
            requires(E&& e, P&& p)
            {
                forward<E>(e).require(forward<P>(p));
            };

        template <typename E, typename P>
        concept customise_point =
            requires(E&& e, P&& p)
            {
                require(forward<E>(e), forward<P>(p));
            };

        struct func_type
        {
            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                properties_detail::static_query<E, P>)
            decltype(auto) operator()(E&& e, P&&) const noexcept
            {
                return forward<E>(e);
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                customise_point<E, P>)
            decltype(auto) operator()(E&& e, P&& p) const
                noexcept(noexcept(require(forward<E>(e), forward<P>(p))))
            {
                return require(forward<E>(e), forward<P>(p));
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                !customise_point<E, P> &&
                has_member_require<E, P>)
            decltype(auto) operator()(E&& e, P&& p) const
                noexcept(noexcept(forward<E>(e).require(forward<P>(p))))
            {
                return forward<E>(e).require(forward<P>(p));
            }

            template <typename E, typename P0, typename P1, typename ... Pn>
                requires(basic_requirements<E, P0>)
            decltype(auto) operator() (E&& e, P0&& p0, P1&& p1, Pn&& ... pn) const
                noexcept(noexcept((*this)((*this)(forward<E>(e), forward<P0>(p0)), forward<P1>(p1), forward<Pn>(pn)...)))
            {
                return (*this)((*this)(forward<E>(e), forward<P0>(p0)), forward<P1>(p1), forward<Pn>(pn)...);
            }
        };

        inline constexpr func_type require{};
    }

    namespace prefer_n
    {
        template <typename E, typename P>
        concept basic_requirements = is_applicable_property_v<decay_t<E>, decay_t<P>> &&
            decay_t<P>::is_preferable;

        template <typename E, typename P>
        concept has_member_prefer =
            requires(E&& e, P&& p)
            {
                forward<E>(e).prefer(forward<P>(p));
            };

        template <typename E, typename P>
        concept customise_point =
            requires(E&& e, P&& p)
            {
                prefer(forward<E>(e), forward<P>(p));
            };

        struct func_type
        {
            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                properties_detail::static_query<E, P>)
            decltype(auto) operator()(E&& e, P&&) const noexcept
            {
                return forward<E>(e);
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                customise_point<E, P>)
            decltype(auto) operator()(E&& e, P&& p) const
                noexcept(noexcept(prefer(forward<E>(e), forward<P>(p))))
            {
                return prefer(forward<E>(e), forward<P>(p));
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !properties_detail::static_query<E, P> &&
                !customise_point<E, P> &&
                has_member_prefer<E, P>)
            decltype(auto) operator()(E&& e, P&& p) const
                noexcept(noexcept(forward<E>(e).prefer(forward<P>(p))))
            {
                return forward<E>(e).prefer(forward<P>(p));
            }

            template <typename E, typename P0, typename P1, typename ... Pn>
                requires(basic_requirements<E, P0>)
            decltype(auto) operator() (E&& e, P0&& p0, P1&& p1, Pn&& ... pn) const
                noexcept(noexcept((*this)((*this)(forward<E>(e), forward<P0>(p0)), forward<P1>(p1), forward<Pn>(pn)...)))
            {
                return (*this)((*this)(forward<E>(e), forward<P0>(p0)), forward<P1>(p1), forward<Pn>(pn)...);
            }
        };

        inline constexpr func_type prefer{};
    }

    namespace query_n
    {
        template <typename E, typename P>
        concept basic_requirements = is_applicable_property_v<decay_t<E>, decay_t<P>>;

        template <typename E, typename P>
        concept has_static_query =
            requires
            {
                decay_t<P>::template static_query_v<decay_t<E>>;
            };

        template <typename E, typename P>
        concept has_member_query =
            requires(E&& e, P&& p)
            {
                forward<E>(e).query(forward<P>(p));
            };

        template <typename E, typename P>
        concept customise_point =
            requires(E&& e, P&& p)
            {
                query(forward<E>(e), forward<P>(p));
            };

        struct func_type
        {
            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                has_static_query<E, P>)
            decltype(auto) operator() (E&& e, P&&) const
                noexcept(properties_detail::is_nothrow_static_query_v<E, P>)
            {
                return decay_t<P>::template static_query_v<decay_t<E>>;
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !has_static_query<E, P> &&
                customise_point<E, P>)
            decltype(auto) operator() (E&& e, P&& p) const
                noexcept(noexcept(query(forward<E>(e), forward<P>(p))))
            {
                return query(forward<E>(e), forward<P>(p));
            }

            template <typename E, typename P> requires(
                basic_requirements<E, P> &&
                !has_static_query<E, P> &&
                !customise_point<E, P> &&
                has_member_query<E, P>)
            decltype(auto) operator() (E&& e, P&& p) const
                noexcept(noexcept(forward<E>(e).query(forward<P>(p))))
            {
                return forward<E>(e).query(forward<P>(p));
            }
        };

        inline constexpr func_type query{};
    }

    using require_concept_n::require_concept;
    using require_n::require;
    using prefer_n::prefer;
    using query_n::query;
}