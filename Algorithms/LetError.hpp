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
                try
                {
                    execution::set_value(move(r_), forward<Args>(args)...);
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
                    this->set_error(err);
                }
            }
        };

        template <typename S, typename F>
        struct _sender_type
        {
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
                return execution::connect(move(*this), receiver_type{ forward<R>(r), move(f_) });
            }
        };
    }
}