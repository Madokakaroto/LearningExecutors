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

            _receiver_type(S&& s, R&& r)
                : s_(move(s))
                , r_(move(r))
            {}

            _receiver_type(S&& s, R const& r)
                : s_(move(s))
                , r_(r)
            {}

            template <typename ... Vs>
            void set_value(Vs&& ... vs) &&
            {
                value_receiver r{ move(r_), forward<Vs>(vs)... };
                execution::submit(move(s_), move(r));
            }

            template <typename E>
            void set_error(E&& e) &&
            {
                error_receiver r{ move(r_), forward<E>(e) };
                execution::submit(move(s_), move(r));
            }

            void set_done() &&
            {
                done_receiver r{ move(r_) };
                execution::submit(move(s_), move(r));
            }
        };

        // S is the original sender
        // Sch is the scheduler where values of the sender are proprgated on
        template <sender S, scheduler Sch>
        struct _sender_type
        {
            S s_;
            schedule_result_t<Sch> ssch_;

            _sender_type(S&& s, Sch&& sch)
                : s_(move(s))
                , ssch_(execution::schedule(move(sch)))
            {
            }

            template <typename R> requires(sender_to<S, R>)
            auto connect(R&& r) && noexcept(is_nothrow_invocable_v<decltype(execution::connect), S, _receiver_type<schedule_result_t<Sch>, remove_cvref_t<R>>>)
            {
                _receiver_type propagate_receiver{ move(ssch_), forward<R>(r) };
                execution::connect(move(s_), move(propagate_receiver));
            }
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
        };

        inline constexpr func_type on{};
    }

    using on_n::on;

    template <typename S, typename Sch>
    using on_result_t = invoke_result_t<decltype(execution::on), S, Sch>;
}