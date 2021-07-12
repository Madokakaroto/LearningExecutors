#pragma once

namespace std::execution
{
    template <typename F, typename>
    struct as_receiver
    {
        void set_value() noexcept(is_nothrow_invocable_v<F&>)
        {
            invoke(f_);
        }

        template <typename E>
        void set_error(E&&) noexcept
        {
            terminate();
        }

        void set_done() noexcept {}

        F f_;
    };

    namespace submit_n
    {
        template <typename S, typename R>
        struct submit_state
        {
            struct submit_receiver
            {
                submit_state* p_;

                template <typename ... Args> requires(receiver_of<R, Args...>)
                void set_value(Args&& ... args) && noexcept(is_nothrow_receiver_of_v<R, Args...>)
                {
                    execution::set_value(std::move(p_->r_), forward<Args>(args)...);
                    delete p_;
                }

                template <typename E> requires(receiver<R, E>)
                void set_error(E&& e) && noexcept
                {
                    execution::set_error(std::move(p_->r_), forward<E>(e));
                    delete p_;
                }

                void set_done() && noexcept
                {
                    execution::set_done(std::move(p_->r_));
                    delete p_;
                }
            };

            remove_cvref_t<R> r_;
            connect_result_t<S, submit_receiver> state_;

            submit_state(S&& s, R&& r)
                : r_(move(r))
                , state_(execution::connect(move(s), submit_receiver{ this }))
            {
            }
        };

        template <typename S, typename R>
        concept default_impl =
            requires(S&& s, R&& r)
            {
                forward<S>(s).submit(forward<R>(r));
            };

        template <typename S, typename R>
        concept customise_point =
            requires(S&& s, R&& r)
            {
                submit(forward<S>(s), forward<R>(r));
            };

        struct func_type
        {
            template <typename S, typename R>
                requires(sender_to<S, R> && default_impl<S, R> && !customise_point<S, R>)
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(forward<S>(s).submit(forward<R>(r))))
            {
                return forward<S>(s).submit(forward<R>(r));
            }

            template <typename S, typename R>
                requires(sender_to<S, R> && customise_point<S, R>)
            decltype(auto) operator() (S&& s, R&& r) const
                noexcept(noexcept(submit(forward<S>(s), forward<R>(r))))
            {
                return submit(forward<S>(s), forward<R>(r));
            }

            template <typename S, typename R>
                requires(sender_to<S, R> && !(default_impl<S, R> && customise_point<S, R>))
            decltype(auto) operator() (S&& s, R&& r) const
            {
                execution::start((new submit_state<S, R>{ forward<S>(s), forward<R>(r) })->state_);
            }
        };
    }

    inline constexpr submit_n::func_type submit{};
}