#pragma once

namespace std::execution
{
    namespace on_n
    {
        // receiver of the sender of the scheduler
        template <typename S, typename R>
        struct _receiver_type
        {
            S s_;                               // senders of the scheduler
            R r_;                               // receiver to connect

            _receiver_type(S s, R r)
                : s_(move(s))
                , r_(move(r))
            {}

            template <typename ... Vs>
            void set_value(Vs&& ... vs) && noexcept
            {
                try
                {
                    value_receiver<R, Vs...> r(move(r_), move(vs)... );
                    //execution::submit(move(s_), move(r));
                }
                catch(...)
                {
                    execution::set_error(move(r_), current_exception());
                }
            }

            template <typename E>
            void set_error(E&& e) && noexcept
            {
                try
                {
                    error_receiver r{ move(r_), forward<E>(e) };
                    execution::submit(move(s_), move(r));
                }
                catch(...)
                {
                    execution::set_error(move(r_), current_exception());
                }
            }

            void set_done() && noexcept
            {
                try
                {
                    done_receiver r{ move(r_) };
                    execution::submit(move(s_), move(r));
                }
                catch(...)
                {
                    execution::set_error(move(r_), current_exception());
                }
            }
        };

        // S is the original sender
        // Sch is the scheduler where values of the sender are propagated
        template <sender S, scheduler Sch>
        struct _sender_type
        {
            S s_;
            schedule_result_t<Sch> ssch_;

            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = typename sender_traits<S>::template value_types<Variant, Tuple>;

            template
            <
                template <typename ...> class Variant
            >
            using error_types = typename sender_traits<S>::template error_types<Variant>;

            static constexpr bool sends_done =
                sender_traits<S>::sends_done ||
                sender_traits<schedule_result_t<Sch>>::sends_done;

            _sender_type(S s, Sch sch)
                : s_(move(s))
                , ssch_(execution::schedule(move(sch)))
            {
            }

            template <typename R> requires(sender_to<S, R>)
            auto connect(R&& r) && noexcept(is_nothrow_invocable_v<decltype(execution::connect), S, _receiver_type<schedule_result_t<Sch>, remove_cvref_t<R>>>)
            {
                _receiver_type propagate_receiver{ move(ssch_), forward<R>(r) };
                return execution::connect(move(s_), move(propagate_receiver));
            }
        };

        template <scheduler Sch>
        struct _pipe_operand
        {
            Sch sch_;
        };

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
                return _sender_type{ forward<S>(s), forward<Sch>(sch) };
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