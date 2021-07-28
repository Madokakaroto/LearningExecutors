#pragma once

namespace std::execution
{
    namespace let_error_n
    {
        template <typename R, typename F>
        struct _receiver_type
        {
            R r_;
            F f_;

            template <typename ... Args> requires receiver_of<R, Args...>
            void set_value(Args&& ... args) && noexcept
            {
                execution::set_value(move(r_), forward<Args>(args)...);
            }

            template <typename E>
            void set_error(E&& e) && noexcept
            {
                try
                {
                    if constexpr(is_invocable_v<F, E>)
                    {
                        propagate<invoke_result_t<F, E>>(forward<E>(e));
                    }
                    else if constexpr(same_as<remove_cvref_t<E>, exception_ptr>)
                    {
                        using traits_type = callable_traits<F>;
                        using error_type = tuple_element_t<0, typename traits_type::arguments_type>;
                        rethrow_and_propagte<error_type>(e);
                    }
                    else
                    {
                        execution::set_error(move(r_), forward<E>(e));
                    }
                }
                catch(...)
                {
                    execution::set_error(move(r_), current_exception());
                }
            }

            void set_done() && noexcept
            {
                execution::set_done(move(r_));
            }

        private:
            template <typename Ret, typename E>
            void propagate(E&& e)
            {
                if constexpr(!is_void_v<Ret>)
                {
                    _value_receiver<R, Ret> receiver{ move(r_), invoke(f_, forward<E>(e)) };
                    execution::submit(execution::schedule(immediate_context.get_scheduler()), move(receiver));
                }
                else
                {
                    invoke(f_, forward<E>(e));
                    _value_receiver<R> receiver{ move(r_) };
                    execution::submit(execution::schedule(immediate_context.get_scheduler()), move(receiver));
                }
            }

            template <typename Error>
            void rethrow_and_propagte(exception_ptr const& e)
            {
                try
                {
                    rethrow_exception(e);
                }
                catch(Error const& err)
                {
                    move(*this).set_error(err);
                }
            }
        };

        template <typename S, typename F>
        struct _sender_type
        {
            // value types for typed sender
            template
            <
                template <typename ...> class Variant,
                template <typename ...> class Tuple
            >
            using value_types = Variant<Tuple<typename callable_traits<F>::result_type>>;

            // error types for typed sender
            // TODO ... a better error types
            template
            <
                template <typename ...> class Variant
            >
            using error_types = typename sender_traits<S>::template error_types<Variant>;

            static constexpr bool sends_done = sender_traits<S>::sends_done;

            S s_;
            F f_;

            _sender_type(S&& s, F&& f) noexcept
                : s_(move(s))
                , f_(move(f))
            {
            }

            template <typename R>
            auto connect(R&& r) && noexcept -> connect_result_t<S, _receiver_type<remove_cvref_t<R>, F>>
            {
                using receiver_type = _receiver_type<remove_cvref_t<R>, F>;
                return execution::connect(move(s_), receiver_type{ forward<R>(r), move(f_) });
            }
        };

        template <typename S, typename F>
            concept has_let_error_impl =
            requires(S&& s, F&& f)
            {
                forward<S>(s).let_error(forward<F>(f));
            };

        template <typename S, typename F>
            concept customise_point =
            requires(S&& s, F&& f)
            {
                let_error(forward<S>(s), forward<F>(f));
            };

        struct func_type
        {
            template <sender S, typename F> requires(customise_point<S, F>)
            decltype(auto) operator() (S&& s, F&& f) const
                noexcept(noexcept(let_error(declval<S>(), declval<F>())))
            {
                return let_error(forward<S>(s), forward<F>(f));
            }

            template <sender S, typename F>
                requires(!customise_point<S, F> && has_let_error_impl<S, F>)
            decltype(auto) operator() (S&& s, F&& f) const
                noexcept(noexcept(declval<S>().let_error(declval<F>())))
            {
                return forward<S>(s).let_error(forward<F>(f));
            }

            template <sender S, typename F>
            decltype(auto) operator() (S&& s, F&& f) const noexcept
            {
                return _sender_type{ forward<S>(s), forward<F>(f) };
            }

            template <typename F>
            auto operator() (F&& f) const noexcept
            {
                return _pipe_operand_type{ *this, forward<F>(f) };
            }
        };
    }

    inline constexpr let_error_n::func_type let_error{};
}