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
                        invoke(f_, forward<E>(e));
                    }
                    else
                    {
                        //execution::set_error()
                    }
                }
                catch(...)
                {
                    execution::set_error(move(r_), current_exception());
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