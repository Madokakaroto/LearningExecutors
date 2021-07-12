#pragma once

namespace std::execution
{
    namespace just_n
    {
        template <typename ... Args>
        concept default_impl = (move_constructible<Args> && ...);

        template <typename ... Args>
        concept customise_point =
            requires(Args&& ... args)
            {
                just(forward<Args>(args)...);
            };

        template <move_constructible ... Args>
        struct _sender_type
        {
            //  for sender traits
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = Variant<Tuple<Args...>>;

            template
            <
                template <typename ...> class Variant
            >
            using error_types = Variant<exception_ptr>;

            static constexpr bool sends_done = false;
            // end of sender traits

            // values member
            std::tuple<Args...> tuple_;

            explicit(sizeof...(Args) == 1) constexpr _sender_type(Args ... args)
                : tuple_(move(args)...) { }

            template <receiver_of<Args...> R>
            struct _op
            {
                R r_;
                std::tuple<Args...> tuple_;
                void start() && noexcept
                {
                    if constexpr(is_nothrow_invocable_v<decltype(execution::set_value), R&&, Args...>)
                    {
                        apply([this](Args&& ... args) mutable
                        {
                            execution::set_value(move(r_), forward<Args>(args)...);
                        }, move(tuple_));
                    }
                    else
                    {
                        try
                        {
                            apply([this](Args&& ... args) mutable
                            {
                                execution::set_value(move(r_), forward<Args>(args)...);
                            }, move(tuple_));
                        }
                        catch(...)
                        {
                            execution::set_error(move(r_), current_exception());
                        }
                    }
                }
            };

            template <receiver_of<Args...> R> requires(copy_constructible<Args> && ...)
            auto connect(R r) const &
                noexcept(is_nothrow_constructible_v<_op<R>, R&&, std::tuple<Args...>>) -> _op<R>
            {
                return _op<R>{ move(r), tuple_ };
            }

            template <receiver_of<Args...> R>
            auto connect(R r) && noexcept -> _op<R>
            {
                return _op<R>{ move(r), move(tuple_) };
            }
        };

        struct func_type
        {
            template <typename ... Args> requires(customise_point<Args...>)
            decltype(auto) operator() (Args&& ... args) const
                noexcept(noexcept(just(declval<Args>()...)))
            {
                return just(forward<Args>(args)...);
            }

            template <typename ... Args>
                requires(!customise_point<Args...> && default_impl<Args...>)
            constexpr decltype(auto) operator() (Args&& ... args) const noexcept
            {
                using sender_type = _sender_type<remove_cvref_t<Args>...>;
                return sender_type{ forward<Args>(args)... };
            }
        };
    }

    inline constexpr just_n::func_type just{};
}